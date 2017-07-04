#ifndef INTERBUS_LOG_H
#define INTERBUS_LOG_H

#include <QDebug>

#define LOGI() qInfo()
#define LOGW() qWarning()
#define LOGE() qCritical()
#define LOGD() qDebug()

#endif // INTERBUS_LOG_H
