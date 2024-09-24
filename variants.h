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

CDCDEV cdcVariants[2] = {
    {.vid = 0x1a86, .pid = 0x7523, .devName = "CH340", .desc = "Generic CH34x device", .bulkReadEndpoint = 0x82, .bulkWriteEndpoint = 0x02, .setupVarian = 1},
    {.vid = 0x067b, .pid = 0x2303, .devName = "PL2303", .desc = "Prolific Technology, Inc. USB-Serial Controller", .bulkReadEndpoint = 0x83, .bulkWriteEndpoint = 0x02}};
