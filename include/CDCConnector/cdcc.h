#ifndef CDCC
#define CDCC
#include "variants.h"
#include <iostream>
#include <vector>
#include <../libusb/libusb.h> // Внешняя библиотека libusb

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
    \brief Класс для работы с USB CDC
    \author Владислав Рябчевский
    \version 1.0
    \date 20.03.2024
    \example examples/console_example.cpp

    Класс для работы с USB CDC в ситуациях, когда нет драйвра для устройства
*/
class CDCConnector
{
public:
    /*!
     * Конструктор класса. На вход ожидает  CDCDEV с параметрами выбранной микросхемы
     */
    CDCConnector(CDCDEV variant);
    CDCConnector();
    /*!
     * Деструктор класа. Выполняет отключение микросхемы и высвобождение ресурсов
     */
    ~CDCConnector();

    /*!
     * Функция подготовки микросхемы к работе. Выполняет открытие интерфейса, подготовку и передачу скорости UART (baudrate) для обмена данными
     *
     * \result возвращает код ошибки (для libusb_error_name()) или 0 в случае успешного выполнения
     */
    virtual int connect();
    /*!
     *  Отключение и высвобождение интерфейса. Вызывается деструктором.
     */
    virtual void disconnect();
    /*!
     * Функция для записи необходимй скорости. Применяется во время connect
     *
     * \result возвращает код ошибки (для libusb_error_name()) или 0 в случае успешного выполнения
     */
    virtual int setBaudrate(uint32_t baud);

    /*!
     *  Чтение данных из последовательного интерфейса. Обращается за данными к bulkReadEndpoint
     *
     *  \param[in] buf массив для записи результата
     *  \param[in] size рколличество ожидаемых символов = азмер массива для данных
     *  \param[in] timeout мс, время ожидания данных от интерфейса. Если ничего не пришло за это вермя то вернет LIBUSB_ERROR_TIMEOUT
     *  \return код ошибки или колличество полученных данных
     */
    virtual int readBytes(unsigned char *buf, int size, int timeout);
    virtual int readBytes(unsigned char *buf, int size); ///< Перегрузка для readBytes
    /*!
     *  Отправка данных в последовательный интерфейс. Отправляет в bulkWriteEndpoint
     *
     *  \param[in] buf массив с сообщением
     *  \param[in] size размер сообщения
     *  \return код ошибки или 0 если все ок
     */
    virtual int sendBytes(unsigned char *buf, int size);

    /*!
     *  Заполнение списока доступных USB устройств
     *
     * \param[in] dev_list ссылка на вектор для записи списка обнаруженных устройств
     * \return колличество найденых устройств или -1 если не удалось инициализировать работу
     */
    static int lsUSB(std::vector<CDCDEV> *dev_list);
    /*!
     *  Заполнение списка доступных USB CDC устройств
     *
     * \param[in] dev_list ссылка на вектор для записи списка обнаруженных устройств
     * \return колличество найденых устройств или -1 если не удалось инициализировать работу
     */
    static int lsCDC(std::vector<CDCDEV> *dev_list);
    /*!
     *  Заполнение списка доступных USB CDC устройств
     *
     * \param[in] first ссылка CDCDEV переменную для сохранения
     * \return 1 если найдено устройство, 0 если нету или код ошибки
     */
    static int firstCDC(CDCDEV *first);
    static CDCConnector *createDevice(uint16_t vid, uint16_t pid);

protected:
    CDCDEV m_device;              ///< переменная, содержащая всю информацию о подключаемом устройстве
    libusb_device_handle *m_husb; ///< дескриптор USB устройства
    libusb_context *m_ctx = NULL; ///< контекст для libusb
    uint32_t m_baudrate = 115200; ///< скорость обмена информацией по UART
};

#endif
