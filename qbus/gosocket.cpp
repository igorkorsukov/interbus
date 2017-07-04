#include "gosocket.h"
#include "log.h"

#ifdef USE_GO
#include "libgotoc.h"
#else
void GoToC_Init(void* p0)
{
    Q_UNUSED(p0);
    LOGW() << "NO_GO: GoToC_Init";
}

struct GoToC_Call_return {
    void* r0;
    int r1;
};

struct GoToC_Call_return GoToC_Call(int p0, void* p1, int p2)
{
    Q_UNUSED(p0);
    Q_UNUSED(p1);
    Q_UNUSED(p2);
    LOGW() << "NO_GO: GoToC_Call";
    GoToC_Call_return r;
    r.r0 = 0;
    r.r1 = 0;
    return r;
}
#endif

using namespace InterBus;

typedef void(*C_call)(int, void*, int, void**, int*);

GoSocket::GoSocket()
{
    C_call p = &GoSocket::static_c_call;
    GoToC_Init(reinterpret_cast<void*>(p));
}

GoSocket::~GoSocket()
{

}

// Send
void GoSocket::send(int port, const QByteArray &in, OnReply *onReply)
{
    void *in_p = 0;
    int in_size = 0;
    copyData(in, &in_p, &in_size);

    GoToC_Call_return r = GoToC_Call(port, in_p, in_size);

    if (onReply) {
        QByteArray ret(static_cast<const char*>(r.r0), r.r1);
        onReply->onReply(ret);
    }

    ::free(r.r0);
}

// Receive
bool GoSocket::listen(int port, Handler *h)
{
    if (h) {
        if (m_listeners.contains(port)) {
            LOGE() << QString("gotoc: already listen port: %1").arg(port);
            return false;
        }
        m_listeners[port] = h;
    } else {
        m_listeners.remove(port);
    }
    return true;
}

void GoSocket::static_c_call(int p, void *in_p, int in_size, void **out_p, int *out_size)
{
    LOGD() << "port: " << p << ", in_size:" << in_size;
    GoSocket::instance()->onRequest(p, in_p, in_size, out_p, out_size);
}

void GoSocket::onRequest(int port, void *in_p, int in_size, void **out_p, int *out_size)
{
    Handler* on = m_listeners.value(port, nullptr);
    if (on) {
        QByteArray out;
        QByteArray in = QByteArray::fromRawData(static_cast<const char*>(in_p), in_size);
        on->onRequest(in, &out);
        if (out_p && out_size) {
            copyData(out, out_p, out_size);
        }
    } else {
        LOGW() << "not found handler, port:" << port;
    }
}

void GoSocket::copyData(const QByteArray &in_ba, void** out_p, int *out_size)
{
    if (!in_ba.isEmpty()) {

        int ba_s = in_ba.size();
        void *ba_p = ::malloc(static_cast<unsigned int>(ba_s) + 1u);
        ::memcpy(ba_p, in_ba.data(), static_cast<unsigned int>(ba_s) + 1u);

        *out_p = ba_p;
        *out_size = ba_s;

    } else {
        *out_p = 0;
        *out_size = 0;
    }
}

GoSocket::OnReply::~OnReply(){}
GoSocket::Handler::~Handler(){}
