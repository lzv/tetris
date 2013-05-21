QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	DEFINES += HAVE_QT5
}

TARGET = tetris
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    piece.cpp \
    board.cpp

HEADERS  += mainwindow.h \
    piece.h \
    board.h
