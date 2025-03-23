#include "CDCConnector/pl230x.h"

CDCC_PL230x::CDCC_PL230x(CDCDEV variant)
{
    m_device = variant;
    libusb_init(&m_ctx);
    libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 1); // уровень вывода ошибок libusb: warning [darwin_transfer_status] transfer error: timed out, https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html#ga2d6144203f0fc6d373677f6e2e89d2d2
    m_husb = 0;
}

int CDCC_PL230x::connect()
{
    int error = 0;
    // Подготовка интерфейса
    m_husb = libusb_open_device_with_vid_pid(m_ctx, m_device.vid, m_device.pid);
    if (!m_husb)
    {
        return LIBUSB_ERROR_NO_DEVICE;
    }

    error = libusb_claim_interface(m_husb, 0);
    IS_ERROR;

    // Настройка микросхемы и скоростей
    error = libusb_control_transfer(m_husb, 0x21, 0x22, ACM_CTRL_DTR | ACM_CTRL_RTS, 0, NULL, 0, 0);
    error = libusb_control_transfer(m_husb, 0x40, 0x01, 0x02, 0x44, NULL, 0, 0);
    IS_ERROR;
    return setBaudrate(m_baudrate);
    return error;
}

int CDCC_PL230x::setBaudrate(uint32_t baud)
{
    int error = 0;
    m_baudrate = baud;
    if (m_husb != 0)
    {
        uint8_t error = 0;
        uint32_t baud = m_baudrate;
        uint8_t encoding[8] = {0};

        if (!(110 < baud && baud < 1000000))
            return -EINVAL;
        for (uint8_t pos = 0; pos < 4; pos++)
        {
            encoding[pos] = baud & 0xFF;
            baud = baud >> 8;
        }
        encoding[7] = 0x07;
        error = libusb_control_transfer(m_husb, 0x21, 0x20, 0, 0, encoding, sizeof(encoding), 100);
        IS_ERROR;
        return 0;
    }
    return error;
}