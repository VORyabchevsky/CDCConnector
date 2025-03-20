#include <iostream>
#include <vector>
#include "CDCConnector/cdcc.h"

CDCConnector::CDCConnector(CDCDEV variant)
{
    m_device = variant;
    libusb_init(&m_ctx);
    libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 1); // уровень вывода ошибок libusb: warning [darwin_transfer_status] transfer error: timed out, https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html#ga2d6144203f0fc6d373677f6e2e89d2d2
    m_husb = 0;
}

CDCConnector::CDCConnector()
{
    libusb_init(&m_ctx);
    libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 1); // уровень вывода ошибок libusb: warning [darwin_transfer_status] transfer error: timed out, https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html#ga2d6144203f0fc6d373677f6e2e89d2d2
    m_husb = 0;
}

void CDCConnector::resetVariant(CDCDEV variant)
{
    m_device = variant;
}

CDCConnector::~CDCConnector()
{
    disconnect();
    libusb_exit(m_ctx);
}

/*!
 *  Конвертор скоростей из uint32 в массив байт
 *
 * \return 0 в случае успеха и -EINVAL если указанная скорость вне диапазона или код ошибки для libusb_error_name()
 */
int CDCConnector::setBaudrateToPL()
{
    uint8_t error = 0;
    uint32_t baud = m_baudrate;
    uint8_t encoding[8] = {0};

    if (!(110 < baud && baud < 1000000))
    {

        std::cout << "ERROR!" << std::endl;
        return -EINVAL;
    }
    for (uint8_t pos = 0; pos < 4; pos++)
    {
        encoding[pos] = baud & 0xFF;
        baud = baud >> 8;
    }
    std::cout << "STILL OK!" << std::endl;
    encoding[7] = 0x07;
    error = libusb_control_transfer(m_husb, 0x21, 0x20, 0, 0, encoding, sizeof(encoding), 100);
    IS_ERROR;
    return 0;
}

int CDCConnector::setBaudrate(uint32_t baud)
{
    m_baudrate = baud;
    return 0;
}

int CDCConnector::applyBaudrate()
{
    int error = 0;
    if (m_husb != 0)
    {
        libusb_release_interface(m_husb, 0);

        if (m_device.setupVarian == 0)
        { // Profilic, SiliconLabs (CP), FT232
            return setBaudrateToPL();
        }
        return error;
    }
    return 0;
}

int CDCConnector::connect()
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
    if (m_device.setupVarian == 0)
    { // Profilic, SiliconLabs (CP), FT232
        error = libusb_control_transfer(m_husb, 0x21, 0x22, ACM_CTRL_DTR | ACM_CTRL_RTS, 0, NULL, 0, 0);
        error = libusb_control_transfer(m_husb, 0x40, 0x01, 0x02, 0x44, NULL, 0, 0);
        IS_ERROR;
        return setBaudrateToPL();
    }
    return error;
}

void CDCConnector::disconnect()
{
    if (m_husb != 0)
    {
        libusb_release_interface(m_husb, 0);
        if (m_husb)
        {
            libusb_close(m_husb);
        }
        libusb_exit(NULL);
        m_husb = 0;
    }
}

int CDCConnector::readBytes(unsigned char *buf, int size, int timeout = 100)
{
    // struct timespec  tv1;
    int actual_length = 0;
    int rc = libusb_bulk_transfer(m_husb, m_device.bulkReadEndpoint, buf, size, &actual_length, timeout);
    if (rc < 0)
    {
        return rc;
    }

    return actual_length;
}

int CDCConnector::readBytes(unsigned char *buf, int size)
{
    return readBytes(buf, size, DEFAULT_TIMEOUT);
}

int CDCConnector::sendBytes(unsigned char *buf, int size)
{
    int transferred;
    unsigned char local_buf[8] = {};
    int error = 0, i = 0;
    if (size < 1)
        return -EINVAL;
    if (size < 9)
        return libusb_bulk_transfer(m_husb, m_device.bulkWriteEndpoint, buf, size, &transferred, DEFAULT_TIMEOUT);
    else
    {
        for (i = 0; i < size; i++)
        {
            if (i && (i % 8 == 0))
            {
                error = libusb_bulk_transfer(m_husb, m_device.bulkWriteEndpoint, local_buf, 8, &transferred, DEFAULT_TIMEOUT);
                IS_ERROR;
            }
            local_buf[i % 8] = buf[i];
        }
        return libusb_bulk_transfer(m_husb, m_device.bulkWriteEndpoint, local_buf, i % 8, &transferred, DEFAULT_TIMEOUT);
    }
    return error;
}

int CDCConnector::lsUSB(std::vector<CDCDEV> *dev_list)
{

    libusb_context *context = NULL;
    libusb_device **list = NULL;
    int rc = 0;
    size_t count = 0;

    rc = libusb_init(&context);
    if (rc != 0)
        return -1; // usb not initialized

    count = libusb_get_device_list(context, &list);
    if (count < 1)
        return 0; // devices not found

    for (size_t idx = 0; idx < count; ++idx)
    {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};
        rc = libusb_get_device_descriptor(device, &desc);

        if (rc != 0)
            continue;
        // Костыль для Аврора SDK <5:
        CDCDEV found_dev;
        found_dev.vid = desc.idVendor;
        found_dev.pid = desc.idProduct;
        (*dev_list).push_back(found_dev);
    }

    libusb_free_device_list(list, 1);
    libusb_exit(context);

    return (*dev_list).size();
}

int CDCConnector::lsCDC(std::vector<CDCDEV> *dev_list)
{

    libusb_context *context = NULL;
    libusb_device **list = NULL;
    int rc = 0;
    size_t count = 0;

    rc = libusb_init(&context);
    if (rc != 0)
        return -1; // usb not initialized

    count = libusb_get_device_list(context, &list);
    if (count < 1)
        return 0; // devices not found

    for (size_t idx = 0; idx < count; ++idx)
    {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};
        rc = libusb_get_device_descriptor(device, &desc);

        if (rc != 0)
            continue; // skip if error
        for (CDCDEV variant : cdcVariants)
        {
            if (variant.vid == desc.idVendor && variant.pid == desc.idProduct)
            {
                (*dev_list).push_back(variant);
            }
        }
    }

    libusb_free_device_list(list, 1);
    libusb_exit(context);

    return (*dev_list).size();
}

int CDCConnector::firstCDC(CDCDEV *first)
{
    std::vector<CDCDEV> list;
    int result = lsCDC(&list);
    if (result > 0)
    {
        (*first) = list[0];
        return 1;
    }
    return result;
}
