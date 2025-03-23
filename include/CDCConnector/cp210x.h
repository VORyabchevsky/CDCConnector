#ifndef CDCC_CP210X_H
#define CDCC_CP210X_H

#include "cdcc.h" // Подключаем базовый класс

class CDCC_CP210x : public CDCConnector
{
public:
    // На основе https://github.com/torvalds/linux/blob/master/drivers/usb/serial/cp210x.c
    CDCC_CP210x(CDCDEV variant);
    int connect();

    /* Из документации:
     * CP2101 поддерживает следующие скорости интерфейса:
     *
     *	300, 600, 1200, 1800, 2400, 4800, 7200, 9600, 14400, 19200, 28800,
     *	38400, 56000, 57600, 115200, 128000, 230400, 460800, 921600
     *
     * CP2102 и  CP2103 к вышеуказанным также поддерживают:
     *
     *	4000, 16000, 51200, 64000, 76800, 153600, 250000, 256000, 500000,
     *	576000
     *
     * Дополнительная информация о поддерживаемых скоростях доступна по ссылкЕ:
     * https://www.freecalypso.org/pub/GSM/Pirelli/chips/silabs_an205.pdf
     */
    int setBaudrate(uint32_t baud);
};

#endif // CDCC_CP210X_H