#-------------------------------------------------
#
# Project created by QtCreator 2014-02-18T18:53:02
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): cache()

QT       += core xml xmlpatterns

QT       -= gui

TARGET = large-file-test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
} else {
    DESTDIR = build/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

SOURCES += main.cpp \
    xmldomratereader.cpp \
    xqueryratereader.cpp \
    easyxmlstreamreader.cpp

HEADERS += \
    xmldomratereader.h \
    xqueryratereader.h \
    easyxmlstreamreader.h
