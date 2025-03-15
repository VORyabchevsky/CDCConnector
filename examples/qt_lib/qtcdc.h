#ifndef QTCDC_H
#define QTCDC_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QThread>


#include "CDCConnector/cdcc.h"


/*!
    \brief Класс для работы с USB CDC в Qt
    \author Владислав Рябчевский
    \version 1.0
    \date 12.10.2024

    Класс для работы с USB CDC в ситуациях, когда нет драйвра для устройства. Проверено на Qt5 для Аврора ОС версии 5 на aarch64.
    Для работы требуется библиотека https://github.com/VORyabchevsky/CDCConnector
*/


class QtCDC : public QObject
{
    Q_OBJECT

public:
    explicit QtCDC(QObject *parent = nullptr);

    int init(); ///< Инициализация работы устройства. Возвращает код ошибки и сигнал exceptionSignal.

    Q_INVOKABLE int sendMsg(QString message); ///< Отправка сообщений устройству

    Q_PROPERTY (QString name READ getName);
    QString getName(){return variant.devName;}

    Q_PROPERTY (QString description READ getDesc);
    QString getDesc(){return QString::fromStdString(variant.desc);}

    Q_PROPERTY (QString vidXpid READ vidXpid);
    QString vidXpid();

    Q_PROPERTY (bool appUseTerminal READ getUseTerminal WRITE setUseTerminal);
    bool getUseTerminal(){return useTerminal;}
    void setUseTerminal(bool areUsing){useTerminal=areUsing;}

    Q_PROPERTY (bool appUseTimestamp READ getUseTimestamp WRITE setUseTimestamp);
    bool getUseTimestamp(){return useTimestamp;}
    void setUseTimestamp(bool areUsing){useTimestamp=areUsing;}

    Q_PROPERTY (bool appUseEcho READ getUseEcho WRITE setUseEcho);
    bool getUseEcho(){return useEcho;}
    void setUseEcho(bool areUsing){useEcho=areUsing;}

    Q_PROPERTY (qint32 appBaudrate READ getBaudrate WRITE setBaudrate);
    qint32 getBaudrate(){return actualBaud;}
    void setBaudrate(uint32_t baudrate);

signals:
    void exceptionSignal(int status);
    void messageAvailable(QString msg);

public slots:
    void slotTimerAlarm(){readMsg();}

private:
    CDCDEV variant;
    CDCConnector dev;
    uint32_t actualBaud = 9600;
    const int bufferSize = 128;
    int buffPosition = 0;
    bool isConnected = false;

    void initTimer();
    int readMsg();

    //параметры работы приложения. TODO: вынести отдельно

    bool useTerminal = true;
    bool useTimestamp = true;
    bool useEcho = true;
};

#endif // QTCDC_H
