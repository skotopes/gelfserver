#-------------------------------------------------
#
# Project created by QtCreator 2016-07-15T19:29:06
#
#-------------------------------------------------

QT       += core gui widgets network


TARGET = GELFServer
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    gelfserver.cpp \
    gelfmessagemodel.cpp

HEADERS  += \
    mainwindow.h \
    gelfserver.h \
    gelfmessagemodel.h

FORMS    += mainwindow.ui

LIBS    += -lz
