requires(qtHaveModule(httpserver))

TEMPLATE = app

QT = httpserver
QT += sql

SOURCES += \
    common/common.cpp \
    event/eventapi.cpp \
    main.cpp \
    user/userapi.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/httpserver/simple
INSTALLS += target

RESOURCES += \
    assets.qrc

CONFIG += cmdline
CONFIG -= app_bundle

HEADERS += \
    common/common.h \
    event/eventapi.h \
    user/userapi.h
