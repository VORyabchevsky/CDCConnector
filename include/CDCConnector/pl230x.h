#ifndef CDCC_PL230X_H
#define CDCC_PL230X_H

#include "cdcc.h" // Подключаем базовый класс

class CDCC_PL230x : public CDCConnector
{
public:
    // Конструктор
    CDCC_PL230x();
    int connect();
    /*!
     *  Конвертор скоростей из uint32 в массив байт
     *
     * \return 0 в случае успеха и -EINVAL если указанная скорость вне диапазона или код ошибки для libusb_error_name()
     */
    int setBaudrate(uint32_t baud);
};

#endif