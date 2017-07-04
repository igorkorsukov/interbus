#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>

#include <interbus>
#include <goping>

void myMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
{
    static QTextStream ts(stdout);
    ts << msg << "\n";
    ts.flush();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    QCoreApplication a(argc, argv);


    InterBus::GoPing ping;
    ping.ping();

    InterBus::Bus *bus = InterBus::Bus::instance();



    return a.exec();
}
