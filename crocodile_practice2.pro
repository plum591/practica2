QT += widgets network

CONFIG += c++17

SOURCES += \
    drawingcanvas.cpp \
    gameengine.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    player.cpp \
    playermanager.cpp \
    worddictionary.cpp

HEADERS += \
    difficulty.h \
    drawingcanvas.h \
    gameengine.h \
    mainwindow.h \
    networkmanager.h \
    player.h \
    playermanager.h \
    worddictionary.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
