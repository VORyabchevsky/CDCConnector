# CDCConnector

Библиотека для подключения к CDC (микросхемам CH340б PL2303 и др.), когда нет подходящего драйвера.

## Релизы

Актуальную версию собранной библиотеки можете скачать со [страницы релизов](https://github.com/VORyabchevsky/CDCConnector/releases/).

## Сборка проекта

Потребуется следующее ПО:

- gcc
- make
- libusb-1.0
- doxygen (не обязательно)

Если все установлено, то скачиваем репозиторий и дополнительный файл заголовков libusb.h:

```
git clone git@github.com:VORyabchevsky/experimental_1.git lib
curl -O curl -o lib/libusb.h https://raw.githubusercontent.com/tenderlove/libusb/refs/heads/master/libusb/libusb.h
```

Для сборки всего проекта достаточно ввести: `make all`. В папке `build` автоматически создастся динамическая библиотека и файл `example` для текущей ОС.

Для получения документации необходимо вызвать `doxygen Doxyfile` или `make docs`. Читаемый файл будет находиться по пути `doc/html/index.html`

Если необходимо создать только файл динамической библиотеки, достаточно вызвать `make libcdcc.so`. Для АврораОС использовал команды:

```
export $AURORA_TAG=AuroraOS-5.0.0.60-base-armv7hl.default
sfdk engine exec sb2 -t $AURORA_TAG make libcdcc.so
```

_примечание: sfdk добавлено в PATH. AURORA_TAG указать в соответсвии с используемой платформой_

## TODO

Текущая версия - минимальный прототип для обмена сообщениями. В планах:

- [ ] функции для работы с сигналами rts, dtr и др.
- [ ] qt класс для работы
- [ ] добавить пример для aurora sdk
- [ ] проверить и докрутить работу с другими микросхемами (cp210x, ft232)

## Примеры

В папке examples сохранены примеры работы с библиотекой:

- console_example.cpp - пример отправки сообщения "Hello world" и вывода всех входящих пакетов данных;
- echo.ino - загрузите в любую плату \*duino _(с конвертором ch340 или pl2303)_ этот пример для отправки эхом всех сообщений от console_example.cpp.
