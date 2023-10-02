TEMPLATE = app
TARGET = PocketComputerManager
LIBS += -ld3d12 -ldxgi
QT += widgets core gui charts
SOURCES += main.cpp \
    myos.cpp
HEADERS += myos.h
FORMS += myos.ui
