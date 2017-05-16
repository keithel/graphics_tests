HEADERS += tp.h
SOURCES += main.cpp
LIBS += -Wl,-rpath-link,$$[QT_SYSROOT]/usr/lib/arm-linux-gnueabihf -llttng-ust -ldl
