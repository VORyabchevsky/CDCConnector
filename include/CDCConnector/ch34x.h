#ifndef CDCC_CH34X_H
#define CDCC_CH34X_H

#include "cdcc.h" // Подключаем базовый класс

class CDCC_CH34x : public CDCConnector
{
public:
    // Конструктор
    CDCC_CH34x();
    int connect();
    /*!
     *  Конвертор скоростей из uint32 в значения регистров для CH340 (и подобных). Содержит магическое число CH341_BAUDBASE_FACTOR = 1532620800.
     *  Взято из https://android.googlesource.com/kernel/msm/+/android-msm-hammerhead-3.4-kk-r1/drivers/usb/serial/ch341.c
     *
     * \return 0 в случае успеха и -EINVAL если указанная скорость вне диапазона или код ошибки для libusb_error_name()
     */
    int setBaudrate(uint32_t baud);
};

#endif // CDCC_CH34X_H