#include "golog.h"
#include "log.h"
#include "interfaces.h"
#include "gosocket.h"

using namespace InterBus;

struct OnLogRequest: public ISocket::Handler
{
    GoLog *log;
    OnLogRequest(GoLog *l) : log(l) {}
    void onRequest(const QByteArray &in, QByteArray *out) { log->onRequest(in, out); }
};

GoLog::GoLog()
{
    m_onRequest = new OnLogRequest(this);
    GoSocket::instance()->listen(InterBus::Port_Log, m_onRequest);
}

GoLog::~GoLog()
{
    delete m_onRequest;
}

void GoLog::onRequest(const QByteArray &in, QByteArray *)
{
    LOGI() << in;
}
