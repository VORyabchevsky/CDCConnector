#include <iostream>
#include <vector>
#include "../cdcc.h"

int main()
{
    int error = 0;
    int len = 0;
    unsigned char buf[65];
    unsigned char testMessage[20] = "Hello World!!\n\r";
    std::vector<CDCDEV> list;

    int x = CDCConnector::lsUSB(&list);
    std::cout << "found: " << x << std::endl;
    std::cout << std::hex << list[0].vid << " " << std::hex << list[0].pid << std::endl;

    CDCDEV variant;

    int res = CDCConnector::firstCDC(&variant);
    if (res != 1)
    {
        std::cout << "Device not found";
        return 1;
    }

    CDCConnector dev(variant);
    dev.setBaudrate(9600);
    res = dev.connect();
    std::cout << libusb_error_name(res) << std::endl;

    while (1)
    {
        error = dev.sendBytes(testMessage, sizeof(testMessage) / sizeof(testMessage[0])); ///< Отправка пакета в UART
        if (error != 0)
        {
            std::cout << error << std::endl;
            std::cout << libusb_error_name(error) << std::endl;
        }
        len = dev.readBytes(buf, 64, 100); ///< Чтение ответного сообщения из UART
        if (len > 0)
        {
            buf[len] = 0;
            std::cout << "Received: \"" << buf << "\"\r\n";
        }
        sleep(1);
    }

    return 0;
}
