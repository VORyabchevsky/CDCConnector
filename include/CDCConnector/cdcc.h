#ifndef CDCC
#define CDCC
#include <iostream>
#include <vector>
#include "variants.h"
#include <../libusb/libusb.h>  // Внешняя библиотека libusb

/*!
    \brief Класс для работы с USB CDC
    \author Владислав Рябчевский
    \version 1.0
    \date 15.08.2024
    \example examples/console_example.cpp

    Класс для работы с USB CDC в ситуациях, когда нет драйвра для устройства
*/
class CDCConnector
{
public:
    CDCConnector(CDCDEV variant);
    CDCConnector();
    ~CDCConnector();

    virtual void resetVariant(CDCDEV variant);
    virtual int connect();
    virtual void disconnect();
    virtual void setBaudrate(uint32_t baud);
    virtual int applyBaudrate();

    virtual int readBytes(unsigned char *buf, int size); ///< Перегрузка для readBytes
    virtual int readBytes(unsigned char *buf, int size, int timeout);
    virtual int sendBytes(unsigned char *buf, int size);

    static int lsUSB(std::vector<CDCDEV> *dev_list);
    static int lsCDC(std::vector<CDCDEV> *dev_list);
    static int firstCDC(CDCDEV *first);

protected:
    CDCDEV m_device;              ///< переменная, содержащая всю информацию о подключаемом устройстве
    libusb_device_handle *m_husb; ///< дескриптор USB устройства
    libusb_context *m_ctx = NULL; ///< контекст для libusb
    uint32_t m_baudrate = 115200; ///< скорость обмена информацией по UART

    int setBaudrateToPL();
    int setBaudrateToCH();
};

#endif
