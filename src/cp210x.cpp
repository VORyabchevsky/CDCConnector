#include "CDCConnector/cp210x.h"

#define CP210X_SET_BAUDRATE 0x1E       ///< Регистр хранения скорости интерфейса, 32 бит
#define REQTYPE_HOST_TO_INTERFACE 0x41 //< 0 01 00001 хост-устройству, LIBUSB_REQUEST_TYPE_CLASS, LIBUSB_RECIPIENT_INTERFACE

#define DIV_ROUND_CLOSEST(x, divisor) \
    (((x) + ((divisor) / 2)) / (divisor))

// CDCC_CP210x::CDCC_CP210x()
// {
//     libusb_init(&m_ctx);
//     libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 1); // уровень вывода ошибок libusb: warning [darwin_transfer_status] transfer error: timed out, https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html#ga2d6144203f0fc6d373677f6e2e89d2d2
//     m_husb = 0;
//     std::cout << "CDCC_CP210x created!" << std::endl;
// }

int CDCC_CP210x::connect()
{
    int error = 0;

    m_husb = libusb_open_device_with_vid_pid(m_ctx, m_device.vid, m_device.pid);
    if (!m_husb)
    {
        return LIBUSB_ERROR_NO_DEVICE;
    }
    error = libusb_claim_interface(m_husb, 0);
    IS_ERROR;

    error = libusb_control_transfer(
        m_husb, REQTYPE_HOST_TO_INTERFACE, 0x00, 0x01, 0x00, nullptr, 0, 100);
    IS_ERROR;

    return setBaudrate(m_baudrate);
}

int CDCC_CP210x::setBaudrate(uint32_t baud)
{
    std::cout << "HERE!" << std::endl;
    int error = 0;
    unsigned int prescale = 1;
    unsigned int div;

    m_baudrate = baud;

    if (m_husb != 0)
    {
        if (baud <= 365) ///< Не знаю, когда может понадобиться такая скорость
            prescale = 4;

        div = DIV_ROUND_CLOSEST(48000000, 2 * prescale * baud);
        baud = 48000000 / (2 * prescale * div);

        error = libusb_control_transfer(
            m_husb, REQTYPE_HOST_TO_INTERFACE, CP210X_SET_BAUDRATE,
            0, 0, // wValue и wIndex не используются
            (unsigned char *)&baud, sizeof(baud), 100);

        IS_ERROR;
        return 0;
    }
}