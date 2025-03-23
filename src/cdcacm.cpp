#include "CDCConnector/cdcacm.h"

int CDCC_ACM::connect()
{
    int error = 0;

    // m_husb = libusb_open_device_with_vid_pid(m_ctx, m_device.vid, m_device.pid);
    m_husb = libusb_open_device_with_vid_pid(m_ctx, 0x2341, 0x8036);
    if (!m_husb)
    {
        return LIBUSB_ERROR_NO_DEVICE;
    }
    error = libusb_claim_interface(m_husb, 0);
    IS_ERROR;

    return 0; // setBaudrate(m_baudrate);
}

int CDCC_ACM::setBaudrate(uint32_t baud)
{
    return 0;
}