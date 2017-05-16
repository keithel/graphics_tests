QT += widgets

HEADERS       = \
    rectangle.h \
    hello-tp.h
SOURCES       = \
                main.cpp \
    rectangle.cpp \
    hello-tp.c

QMAKE_PROJECT_NAME = simplest_widgets
LIBS += -Wl,-rpath-link,$$[QT_SYSROOT]/usr/lib/arm-linux-gnueabihf -llttng-ust -ldl

# install
target.path = /opt/$${TARGET}/bin
INSTALLS += target
