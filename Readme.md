# CDCConnector

Библиотека для подключения к CDC (микросхемам CH340б PL2303 и др.), когда нет подходящего драйвера.

## Релизы

Актуальную версию собранной библиотеки можете скачать со страницы релизов [github](https://github.com/VORyabchevsky/CDCConnector/releases/) или [gitflic](https://gitflic.ru/project/voryabchevsky/cdcconnector/release).

## Совместимые чипы

| Название чипа | VID (Vendor ID) | PID (Product ID) | Поддерживаемый функционал                   |
| ------------- | --------------- | ---------------- | ------------------------------------------- |
| CH340         | `0x1A86`        | `0x7523`         | Прием и передача данных, настройка скорости |
| PL2303        | `0x067b`        | `0x2303`         | Прием и передача данных, настройка скорости |
| CP2102        | `0x10c4`        | `0xea60`         | Прием и передача данных, настройка скорости |

## Сборка проекта

Потребуется следующее ПО:

- gcc
- make
- libusb-1.0
- doxygen (не обязательно)

Если все установлено, то скачиваем репозиторий и дополнительно библиотеку с заголовками:

```
git clone https://gitflic.ru/project/voryabchevsky/cdcconnector.git cdcc
cd cdcc
git clone https://github.com/libusb/libusb.git tmp
mv tmp/libusb . && rm -rf tmp
```

Для сборки всего проекта достаточно ввести: `make all`. В папке `build` автоматически создастся библиотеки и файл `example` для текущей ОС.

Для получения документации необходимо вызвать `doxygen Doxyfile` или `make docs`. Читаемый файл будет находиться по пути `doc/html/index.html`

- `make libcdcc.so` - создает файл динамической библиотеки
- `make libcdcc.a` - создает файл статической библиотеки

### Аврора ОС

Сборка осуществляется через PSDK. Для этого необходимо настроить его в соответсвии со [статьей](https://habr.com/ru/articles/886192/). Далее доустановить пакеты (для каждого таргета):

```
export TARGET_ARMV7HL=AuroraOS-5.1.3.85-MB2-armv7hl
export TARGET_AARCH64=AuroraOS-5.1.3.85-MB2-aarch64

#по необходимости - установить таргеты
sdk-chroot sdk-assistant target create $TARGET_ARMV7HL Aurora_OS-5.1.3.85-MB2-Aurora_SDK_Target-armv7hl.tar.7z
sdk-chroot sdk-assistant target create $TARGET_AARCH64 Aurora_OS-5.1.3.85-MB2-Aurora_SDK_Target-aarch64.tar.7z

sdk-chroot sb2 -R -t $TARGET_ARMV7HL zypper in doxygen libusb1-devel
sdk-chroot sb2 -R -t $TARGET_AARCH64 zypper in doxygen libusb1-devel
```

## TODO

Текущая версия - минимальный прототип для обмена сообщениями. В планах:

- [ ] функции для работы с сигналами rts, dtr и др.
- [ ] проверить и докрутить работу с другими микросхемами (cp210x, ft232)
- [ ] ci, (_возможно перенесу на gitverse_)

## Примеры

- [EasyCOM](https://gitflic.ru/project/voryabchevsky/easycom)
- examples/console_example.cpp - пример отправки сообщения "Hello world" и вывода всех входящих пакетов данных;
- examples/echo/echo.ino - загрузите в любую плату \*duino _(с конвертором ch340 или pl2303)_ этот пример для отправки эхом всех сообщений от console_example.cpp;
- examples/qt_class - пример вызова библиотеки в Qt.

## CHANGELOG

### 2.0 (23/03/2025)

- добавлены отдельные классы для каждой микросхемы;
- добавлена фабрика для выбора нужного чипа по vid+pid;
- добавлена работа с чипом cp2102.

### 1.1 (15/03/2025)

- добавлена функция изменения сокрости обмена;
- добавлена и проверена сборка статической библиотеки;
- добавлен пример для Qt;
- мелкие исправления.
