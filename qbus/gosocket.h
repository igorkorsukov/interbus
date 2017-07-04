#ifndef INTERBUS_GOSOCKET_H
#define INTERBUS_GOSOCKET_H


#include <QByteArray>
#include <QHash>

#include "interfaces.h"

namespace InterBus {

class GoSocket : public ISocket
{
public:

    static GoSocket* instance() {
        static GoSocket t;
        return &t;
    }

    void send(int port, const QByteArray &in, OnReply *onReply = 0);
    bool listen(int port, Handler *h);

private:

    GoSocket();
    ~GoSocket();

    static void static_c_call(int p, void* in_p, int in_size, void** out_p, int* out_size);

    void onRequest(int port, void* in_p, int in_size, void** out_p, int* out_size);

    void copyData(const QByteArray &in_ba, void** out_p, int *out_size);

    QHash<int /*port*/, Handler* > m_listeners;
};

}

#endif // INTERBUS_GOSOCKET_H
