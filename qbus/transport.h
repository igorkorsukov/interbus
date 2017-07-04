#ifndef INTERBUS_TRANSPORT_H
#define INTERBUS_TRANSPORT_H

#include <QString>
#include <QJsonValue>
#include <QJsonArray>

#include "interfaces.h"

namespace InterBus {

class TransportHandler;
class Transport
{
public:

    Transport(int port, ISocket* s = nullptr);
    ~Transport();

    struct Ret {
        QString errorString;
        QJsonValue result;
        bool success() const { return errorString.isEmpty(); }
    };

    Ret call(const QString &method, const QJsonArray &args = QJsonArray());


    struct Handler {
        virtual ~Handler();
        virtual Ret onCalled(const QString &method, const QJsonArray &args) = 0;
    };

    bool listen(Handler *h);

private:

    ISocket* socket() const;

    friend class TransportHandler;
    void onRequest(const QByteArray &in, QByteArray *out);

    int m_port;
    ISocket* m_socket;
    TransportHandler* m_socketHandler;
    Handler* m_onCalled;

};

}

#endif // INTERBUS_TRANSPORT_H
