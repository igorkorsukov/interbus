#ifndef INTERBUS_BUS_H
#define INTERBUS_BUS_H

#include <QObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>
#include <QSet>

namespace InterBus {

class Transport;
class OnBusCalled;
class Bus
{
public:

    static Bus* instance() {
        static Bus b;
        return &b;
    }

    // Call methods

    struct Ret {
        QString errorString;
        QJsonValue result;
        bool success() const { return errorString.isEmpty(); }
    };

    Ret callMethod(const QString &method, const QJsonValue &arg1 = QJsonValue(), const QJsonValue &arg2 = QJsonValue());


    // Bind methods

    struct IMethod {
        virtual ~IMethod();
        virtual QJsonValue call(const QJsonArray&, QString *err) = 0;
    };

    void bindMethod(const QString &method, IMethod *m);
    void unbindMethod(const QString &method);
    void unbindObject(const QString &name);

private:
    explicit Bus();
    ~Bus();

    Transport* transport() const;

    friend class OnBusCalled;
    void onCalled(const QString &method, const QJsonArray &args, QJsonValue *res, QString *err);

    QHash<QString, IMethod* > m_methods;

    Transport* m_transport;
    OnBusCalled* m_onBusCalled;
};

}

#endif // INTERBUS_BUS_H
