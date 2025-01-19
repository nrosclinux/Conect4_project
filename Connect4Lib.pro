QT += gui sql core

TEMPLATE = lib
DEFINES += CONNECT4LIB_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connect4.cpp\
    connect4dao.cpp \
    player.cpp \
    round.cpp

HEADERS += \
    Connect4Lib_global.h \
    connect4.h \
    connect4dao.h \
    player.h \
    round.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
