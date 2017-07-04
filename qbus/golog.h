#ifndef INTERBUS_GOLOG_H
#define INTERBUS_GOLOG_H

#include <QByteArray>

class OnLogRequest;
class GoLog
{
public:

    static GoLog* instance() {
        static GoLog l;
        return &l;
    }

private:
    GoLog();
    ~GoLog();

    friend class OnLogRequest;
    void onRequest(const QByteArray &in, QByteArray *out);

    OnLogRequest* m_onRequest;
};

#endif // INTERBUS_GOLOG_H
