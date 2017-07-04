#include "goping.h"
#include "log.h"
#include "interfaces.h"
#include "gosocket.h"

using namespace InterBus;

GoPing::GoPing()
{

}

bool GoPing::ping()
{
    struct OnReply: public ISocket::OnReply {
        QByteArray ret;
        void onReply(const QByteArray &r)  {
            ret = r;
        }
    } onReply;

    struct OnRequest: public GoSocket::Handler {
        void onRequest(const QByteArray &in, QByteArray *out) {
            *out = in + QByteArray(" -> pong from qt");
        }
    } onRequest;

    GoSocket* gt = GoSocket::instance();

    gt->listen(InterBus::Port_Ping, &onRequest);
    gt->send(InterBus::Port_Ping, QByteArray("ping from qt"), &onReply);
    gt->listen(InterBus::Port_Ping, nullptr);

    LOGI() << onReply.ret;

    return true;
}
