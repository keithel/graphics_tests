TEMPLATE     = app

QT          += qml quick

SOURCES     += main.cpp
RESOURCES   += clocks.qrc

OTHER_FILES  += \
                clocks.qml \
                content/Clock.qml \
                content/*.png

target.path = /home/ares
INSTALLS += target
