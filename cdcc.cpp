#include <iostream>
#include <vector>
#include "cdcc.h"

#define ACM_CTRL_DTR 0x01
#define ACM_CTRL_RTS 0x02

#define CTRL_IN (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT (LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)

#define DEFAULT_TIMEOUT 100 ///< мС

#define IS_ERROR      \
    if (error < 0)    \
    {                 \
        return error; \
    }

/*!
 * Конструктор класса. На вход ожидает  CDCDEV с параметрами выбранной микросхемы
 */
CDCConnector::CDCConnector(CDCDEV variant)
{
    m_device = variant;
    libusb_init(&m_ctx);
    libusb_set_debug(m_ctx, 1); // уровень вывода ошибок libusb: warning [darwin_transfer_status] transfer error: timed out, https://libusb.sourceforge.io/api-1.0/group__libusb__lib.html#ga2d6144203f0fc6d373677f6e2e89d2d2
    m_husb = 0;
}

/*!
 * Деструктор класа. Выполняет отключение микросхемы и высвобождение ресурсов
 */
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

/*!
 *  Конвертор скоростей из uint32 в значения регистров для CH340 (и подобных). Содержит магическое число CH341_BAUDBASE_FACTOR = 1532620800.
 *  Взято из https://android.googlesource.com/kernel/msm/+/android-msm-hammerhead-3.4-kk-r1/drivers/usb/serial/ch341.c
 *
 * \return 0 в случае успеха и -EINVAL если указанная скорость вне диапазона или код ошибки для libusb_error_name()
 */
int CDCConnector::setBaudrateToCH()
{
    uint16_t reg_a, reg_b;
    uint32_t factor;
    int8_t divisor;
    int error = 0;

    factor = (1532620800 / m_baudrate);
    divisor = 3;

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

    error = libusb_control_transfer(m_husb, CTRL_OUT, 0x9a, 0x1312, reg_a, NULL, 0, 1000);
    if (error < 0)
    {
        error = libusb_control_transfer(m_husb, CTRL_OUT, 0x9a, 0x0f2c, reg_b, NULL, 0, 1000);
    }

    return error;
}

/*!
 * Функция для записи необходимй скорости. Применяется во время connect
 */
void CDCConnector::setBaudrate(uint32_t baud)
{
    m_baudrate = baud;
}

/*!
 * Функция подготовки микросхемы к работе. Выполняет открытие интерфейса, подготовку и передачу скорости UART (baudrate) для обмена данными
 *
 * \result возвращает код ошибки (для libusb_error_name()) или 0 в случае успешного выполнения
 */
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
    else if (m_device.setupVarian == 1)
    { // CH34x
        error = libusb_control_transfer(m_husb, CTRL_OUT, 0xa1, 0x501f, 0xd90a, NULL, 0, 1000);
        IS_ERROR;
        return setBaudrateToCH();
    }

    return error;
}

/*!
 *  Отключение и высвобождение интерфейса. Вызывается деструктором. Необходимо вызывать перед изменением скорости инерфейса или когда не используется.
 */
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

/*!
 *  Чтение данных из последовательного интерфейса. Обращается за данными к bulkReadEndpoint
 *
 *  \param[in] buf массив для записи результата
 *  \param[in] size рколличество ожидаемых символов = азмер массива для данных
 *  \param[in] timeout мс, время ожидания данных от интерфейса. Если ничего не пришло за это вермя то вернет LIBUSB_ERROR_TIMEOUT
 *  \return код ошибки или колличество полученных данных
 */

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

/*!
 *  Отправка данных в последовательный интерфейс. Отправляет в bulkWriteEndpoint
 *
 *  \param[in] buf массив с сообщением
 *  \param[in] size размер сообщения
 *  \return код ошибки или 0 если все ок
 */
int CDCConnector::sendBytes(unsigned char *buf, int size)
{
    int transferred;
    unsigned char local_buf[8] = {};
    int error = 0, i = 0;
    if (size < 1)
        return -EINVAL;
    if (size < 8)
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

/*!
 *  Заполнение списока доступных USB устройств
 *
 * \param[in] dev_list ссылка на вектор для записи списка обнаруженных устройств
 * \return колличество найденых устройств или -1 если не удалось инициализировать работу
 */
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
            continue; // skip if error
        (*dev_list).push_back({.vid = desc.idVendor, .pid = desc.idProduct, "found dev"});
    }

    libusb_free_device_list(list, 1);
    libusb_exit(context);

    return (*dev_list).size();
}

/*!
 *  Заполнение списка доступных USB CDC устройств
 *
 * \param[in] dev_list ссылка на вектор для записи списка обнаруженных устройств
 * \return колличество найденых устройств или -1 если не удалось инициализировать работу
 */
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

/*!
 *  Заполнение списка доступных USB CDC устройств
 *
 * \param[in] first ссылка CDCDEV переменную для сохранения
 * \return 1 если найдено устройство, 0 если нету или код ошибки
 */
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