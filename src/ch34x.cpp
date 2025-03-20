#include "CDCConnector/ch34x.h"

CDCC_CH34x::CDCC_CH34x()
{
    std::cout << "CDCC_CH34x created!" << std::endl;
}

int CDCC_CH34x::connect()
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
    error = libusb_control_transfer(m_husb, CTRL_OUT, 0xa1, 0x501f, 0xd90a, NULL, 0, 1000);
    IS_ERROR;
    return setBaudrate(m_baudrate);

    return error;
}

int CDCC_CH34x::setBaudrate(uint32_t baud)
{
    uint16_t reg_a, reg_b;
    uint32_t factor;
    int8_t divisor = 3;
    int error = 0;

    m_baudrate = baud;

    if (m_husb != 0)
    {

        factor = (1532620800 / m_baudrate);

        while ((factor > 0xfff0) && divisor)
        {
            factor >>= 3;
            divisor--;
        }

        if (factor > 0xfff0)
            return -EINVAL;

        factor = 0x10000 - factor;
        reg_a = (factor & 0xff00) | divisor;
        reg_b = factor & 0xff;

        libusb_release_interface(m_husb, 0);

        error = libusb_control_transfer(m_husb, CTRL_OUT, 0x9a, 0x1312, reg_a, NULL, 0, 1000);
        if (error < 0)
        {
            error = libusb_control_transfer(m_husb, CTRL_OUT, 0x9a, 0x0f2c, reg_b, NULL, 0, 1000);
        }
    }
    return error;
}