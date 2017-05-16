QT += widgets

load(configure)
qtCompileTest(lttng)

HEADERS = \
    rectangle.h
SOURCES = \
    main.cpp \
    rectangle.cpp

#packagesExist(lttng-ust) {
CONFIG(config_lttng) {
    DEFINES += WITH_LTTNG
    message("Linux LTTNG tracing enabled")
    HEADERS += hello-tp.h
    SOURCES += hello-tp.c
    LIBS += -Wl,-rpath-link,$$[QT_SYSROOT]/usr/lib/arm-linux-gnueabihf -llttng-ust -ldl
}

QMAKE_PROJECT_NAME = simplest_widgets

# install
target.path = /opt/$${TARGET}/bin
INSTALLS += target
