#ifndef INTERBUS_INTERFACES_H
#define INTERBUS_INTERFACES_H

#include <QByteArray>

namespace InterBus {

enum Port {
    Port_Undefined = 0,
    Port_Ping   = 10,
    Port_Log    = 11,
    Port_Bus    = 20
};

class ISocket
{
public:
    virtual ~ISocket();

    // Send
    struct OnReply {
        virtual ~OnReply();
        virtual void onReply(const QByteArray &ret) = 0;
    };

    virtual void send(int port, const QByteArray &in, OnReply *onReply = 0) = 0;


    // Receive
    struct Handler {
        virtual ~Handler();
        virtual void onRequest(const QByteArray &in, QByteArray *out) = 0;
    };

    virtual bool listen(int port, Handler *h) = 0;

};


}

#endif // INTERBUS_INTERFACES_H
