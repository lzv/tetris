#ifdef HAVE_QT5
	#include <QApplication>
#endif

#include <QtGui>
#include "mainwindow.h"
#include "piece.h"
#include "board.h"

int main(int argc, char *argv[])
{
	QTextCodec * codec = QTextCodec::codecForName("UTF-8");
#ifndef HAVE_QT5
//	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);
#endif
	QTextCodec::setCodecForLocale(codec);

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    
    return a.exec();
}
