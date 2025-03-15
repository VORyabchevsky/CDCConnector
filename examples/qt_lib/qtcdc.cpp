#include "qtcdc.h"



int QtCDC::sendMsg(QString message){
    int error = 0;
    QByteArray ba = message.toLocal8Bit();
    unsigned char *res = (unsigned char *)strdup(ba.constData());
    error = dev.sendBytes(res, ba.size());
    return error;
}

void QtCDC::setBaudrate(uint32_t baudrate){
    if ((baudrate!=actualBaud) && isConnected){
        actualBaud=baudrate;
        dev.setBaudrate(actualBaud);
        dev.applyBaudrate();
    }
}

QString QtCDC::vidXpid(){
    return QString("VID 0x%1, PID 0x%2")
            .arg(variant.vid, 4, 16, QLatin1Char( '0' ))
            .arg(variant.pid, 4, 16, QLatin1Char( '0' ));
}

QtCDC::QtCDC(QObject *parent){
    qDebug()<<"CDC imported";
};

int QtCDC::init(){
    std::vector<CDCDEV> list;

    int res = CDCConnector::lsUSB(&list);
    qDebug() << "found: " << res;
    if (res < 1)
    {
        qDebug() << "USB Devices not found";
        emit exceptionSignal(-1); // -1 не найдено ни одного usb устройства
        return -1;
    }
    else
    {
        for (CDCDEV dev : list)
        {
            qDebug() << QString("VID 0x%1, PID 0x%2")
                        .arg(dev.vid, 4, 16, QLatin1Char( '0' ))
                        .arg(dev.pid, 4, 16, QLatin1Char( '0' ));
        }
    }
    res = CDCConnector::firstCDC(&variant);
    if (res!=1)
    {
        qDebug() << "USB CDC Device not found";
        emit exceptionSignal(-2); // -2 не найдено cdc
        return -2;
    }
    dev.resetVariant(variant);
    dev.setBaudrate(actualBaud);
    res = dev.connect();
    if (res!=0)
    {
        qDebug() << "Access denied!";
        emit exceptionSignal(-3); // -3 нет доступа
        return -3;
    }
    initTimer();
    emit exceptionSignal(0); // успешно
    isConnected = true;
    return 0;
}


void QtCDC::initTimer(){
    QThread* thread = new QThread(this);
    thread->start();
    QTimer *timer = new QTimer(0);
    timer->setInterval(100);
    timer->moveToThread(thread);

    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()), Qt::DirectConnection);
    connect(thread, SIGNAL(started()), timer, SLOT(start()));
}


int QtCDC::readMsg(){
    int error = 0;
    unsigned char buf[65];
    QString myString = "";

    error = dev.readBytes(buf, 64, 100); ///< Чтение ответного сообщения из UART

    if (error > 0){
        for (int x = 0; x < error - 1; x++){
            myString = myString + QString(char(buf[x]));
        }
        emit messageAvailable(myString);
    }
    return 0;
}


