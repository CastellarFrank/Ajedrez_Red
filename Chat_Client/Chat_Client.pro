# -------------------------------------------------
# Project created by QtCreator 2009-11-30T13:55:49
# -------------------------------------------------
QT += network \
    webkit
TARGET = Chat_Client
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    clientconnection.cpp \
    gamearea.cpp \
    tablero.cpp
HEADERS += mainwindow.h \
    clientconnection.h \
    gamearea.h \
    tablero.h
FORMS += mainwindow.ui \
    tablero.ui
RESOURCES += recursos.qrc
