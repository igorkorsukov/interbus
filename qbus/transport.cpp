#include "transport.h"
#include "log.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include "gosocket.h"

namespace InterBus {

static QString method_key("method");
static QString args_key("params");
static QString error_key("error");
static QString result_key("result");

struct TransportHandler: public ISocket::Handler
{
    Transport *t;
    TransportHandler(Transport *p) : t(p) {}
    void onRequest(const QByteArray &in, QByteArray *out) { t->onRequest(in, out); }
};

Transport::Transport(int port, ISocket *s)
    : m_port(port), m_socket(s), m_socketHandler(nullptr), m_onCalled(nullptr)
{
    if (!m_socket) {
        m_socket = GoSocket::instance();
    }
}

Transport::~Transport()
{
    listen(nullptr);
}

ISocket *Transport::socket() const
{
    return m_socket;
}

Transport::Ret Transport::call(const QString &method, const QJsonArray &args)
{
    QJsonObject msg;
    msg.insert(method_key, method);
    msg.insert(args_key, args);

    struct OnReply: public GoSocket::OnReply {
        QJsonDocument json;
        QJsonParseError jsonErr;
        void onReply(const QByteArray &r)  {
            qDebug() << "reply:" << r;
            json = QJsonDocument::fromJson(r, &jsonErr);
        }
    } onReply;

    //! NOTE Сейчас транспорт синхронный, поэтому всё просто
    socket()->send(m_port, QJsonDocument(msg).toJson(), &onReply);

    Ret ret;
    if (onReply.jsonErr.error != QJsonParseError::NoError) {
        ret.errorString = "gochannel: internal error " + onReply.jsonErr.errorString();
    }

    if (!ret.success()) {
        return ret;
    }

    QJsonObject retMsg = onReply.json.object();
    ret.errorString = retMsg.value(error_key).toString();
    if (!ret.success()) {
        return ret;
    }

    ret.result = retMsg.value(result_key);
    return ret;
}

bool Transport::listen(Handler *h)
{
    m_onCalled = h;
    if (m_onCalled) {

        m_socketHandler = new TransportHandler(this);
        return m_socket->listen(m_port, m_socketHandler);

    } else {

        m_socket->listen(m_port, nullptr);
        delete m_socketHandler;
    }
    return true;
}

void Transport::onRequest(const QByteArray &in, QByteArray *out)
{
    if (!m_onCalled) {
        return;
    }

    QJsonParseError jsonErr;
    QJsonDocument json = QJsonDocument::fromJson(in, &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError) {
        return;
    }

    QJsonObject req = json.object();
    QString method = req.value(method_key).toString();
    QJsonArray args = req.value(args_key).toArray();

    Ret ret = m_onCalled->onCalled(method, args);

    QJsonObject retMsg;
    if (ret.success()) {
        retMsg.insert(result_key, ret.result);
    } else {
        retMsg.insert(error_key, ret.errorString);
    }

    *out = QJsonDocument(retMsg).toJson();
}

Transport::Handler::~Handler(){}

}
