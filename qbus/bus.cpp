#include "bus.h"
#include "log.h"
#include "interfaces.h"
#include "transport.h"

namespace InterBus {

struct OnBusCalled: public Transport::Handler
{
    Bus *bus;
    OnBusCalled(Bus *b) : bus(b) {}
    Transport::Ret onCalled(const QString &method, const QJsonArray &args) {
        Transport::Ret ret;
        bus->onCalled(method, args, &ret.result, &ret.errorString);
        return ret;
    }
};

Bus::Bus()
    : m_transport(nullptr), m_onBusCalled(nullptr)
{

    m_transport = new Transport(InterBus::Port_Bus);

    m_onBusCalled = new OnBusCalled(this);
    m_transport->listen(m_onBusCalled);
}

Bus::~Bus()
{
    delete m_transport;
    delete m_onBusCalled;
}

Transport* Bus::transport() const
{
    return m_transport;
}

void Bus::bindMethod(const QString &method, IMethod *fn)
{
    if (m_methods.contains(method)) {
        LOGE() << "gobus: already bind method:" << method;
        return;
    }
    m_methods[method] = fn;
}

void Bus::unbindMethod(const QString &method)
{
    if (!m_methods.contains(method)) {
        LOGW() << "gobus: not found unbind method:" << method;
        return;
    }

    IMethod* fn = m_methods.take(method);
    delete fn;
}

void Bus::unbindObject(const QString &name)
{
    QStringList keys;
    QHash<QString, IMethod* >::ConstIterator it = m_methods.constBegin(), end = m_methods.constEnd();
    for (it = m_methods.constBegin(); it != end; ++it) {
        if (it.key().startsWith(name)) {
            keys << it.key();
        }
    }

    foreach (const QString &key, keys) {
        IMethod* fn = m_methods.take(key);
        delete fn;
    }
}

Bus::Ret Bus::callMethod(const QString &method, const QJsonValue &arg1, const QJsonValue &arg2)
{
    QJsonArray args;
    if (!arg1.isNull()) args << arg1;
    if (!arg2.isNull()) args << arg2;

    Transport::Ret tRet = transport()->call(method, args);
    Bus::Ret ret;
    ret.errorString = tRet.errorString;
    ret.result = tRet.result;
    return ret;
}

void Bus::onCalled(const QString &method, const QJsonArray &args, QJsonValue *res, QString *err)
{
    IMethod *m = m_methods.value(method, nullptr);
    if (!m) {
        LOGW() << "gobus: not found method:" << method;
        return;
    }

    *res = m->call(args, err);
}

Bus::IMethod::~IMethod(){}

}
