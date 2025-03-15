# CDCConnector

Библиотека для подключения к CDC (микросхемам CH340б PL2303 и др.), когда нет подходящего драйвера.

## Релизы

Актуальную версию собранной библиотеки можете скачать со страницы релизов [github](https://github.com/VORyabchevsky/CDCConnector/releases/) или [gitflic](https://gitflic.ru/project/voryabchevsky/cdcconnector/release).

## Сборка проекта

Потребуется следующее ПО:

- gcc
- make
- libusb-1.0
- doxygen (не обязательно)

Если все установлено, то скачиваем репозиторий и дополнительный файл заголовков libusb.h:

```
git clone https://gitflic.ru/project/voryabchevsky/cdcconnector.git lib
curl -O curl -o lib/libusb.h https://raw.githubusercontent.com/tenderlove/libusb/refs/heads/master/libusb/libusb.h
```

Для сборки всего проекта достаточно ввести: `make all`. В папке `build` автоматически создастся библиотеки и файл `example` для текущей ОС.

Для получения документации необходимо вызвать `doxygen Doxyfile` или `make docs`. Читаемый файл будет находиться по пути `doc/html/index.html`

- `make libcdcc.so` - создает файл динамической библиотеки
- `make libcdcc.a` - создает файл статической библиотеки

. Для АврораОС использовал команды:

```
export $AURORA_TAG=AuroraOS-5.1.3.85-MB2-aarch64.default
sfdk engine exec sb2 -t $AURORA_TAG make libcdcc.so
```

_примечание: sfdk добавлено в PATH. AURORA_TAG указать в соответсвии с используемой платформой_

## TODO

Текущая версия - минимальный прототип для обмена сообщениями. В планах:

- [ ] функции для работы с сигналами rts, dtr и др.
- [ ] проверить и докрутить работу с другими микросхемами (cp210x, ft232)

## Примеры

- [EasyCOM](https://gitflic.ru/project/voryabchevsky/easycom)
- examples/console_example.cpp - пример отправки сообщения "Hello world" и вывода всех входящих пакетов данных;
- examples/echo/echo.ino - загрузите в любую плату \*duino _(с конвертором ch340 или pl2303)_ этот пример для отправки эхом всех сообщений от console_example.cpp;
- examples/qt_class - пример вызова библиотеки в Qt.

## CHANGELOG

### 1.1 (15/03/2025)

- добавлена функция изменения сокрости обмена;
- добавлена и проверена сборка статической библиотеки
- добавлен пример для Qt;
- мелкие исправления.
