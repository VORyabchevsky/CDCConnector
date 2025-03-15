#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define CH340 cdcVariants[0]
/*!
 * Структура с полями необходимыми для описания подлючаемого CDC устройства
 */
struct CDCDEV
{
    uint16_t vid;                     ///< Vendor ID
    uint16_t pid;                     ///< Product ID
    char devName[16] = "";            ///< Имя устройства (необязательный параметр)
    std::string desc = "";            ///< Описание устройства (необязательный параметр)
    uint8_t bulkReadEndpoint = 0x82;  ///< Адрес эндпоинта для чтения данных, используется для readBytes
    uint8_t bulkWriteEndpoint = 0x02; ///< Адрес эндпоинта для записи данных
    uint8_t setupVarian = 0;          ///< Вариант настройки интерфейса. 0 подходит для profilic, 1 для ch340
};

inline CDCDEV cdcVariants[2] = {
    {0x1a86, 0x7523, "CH340", "Generic CH34x device", 0x82, 0x02, 1},
    {0x067b, 0x2303, "PL2303", "Prolific Technology, Inc. USB-Serial Controller", 0x83, 0x02}};
