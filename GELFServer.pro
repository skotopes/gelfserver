#-------------------------------------------------
#
# Project created by QtCreator 2016-07-15T19:29:06
#
#-------------------------------------------------

QT       += core gui widgets network svg


TARGET = GELFServer
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    gelfserver.cpp \
    gelfmessagemodel.cpp \
    qjsonmodel.cpp \
    qjsonitem.cpp

HEADERS  += \
    mainwindow.h \
    gelfserver.h \
    gelfmessagemodel.h \
    qjsonmodel.h \
    qjsonitem.h

FORMS    += mainwindow.ui

LIBS    += -lz

RESOURCES += \
    resources.qrc

mac {
    ICON = GELFServer.icns
}
