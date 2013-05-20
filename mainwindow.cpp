#include <QtGui>
#include "mainwindow.h"
#include "board.h"
#include "piece.h"

MainWindow::MainWindow ()
{
	mainBoard = new board;

	nextPieceLabel = new QLabel;
	nextPieceLabel->setFrameStyle(QFrame::Box | QFrame::Raised);
	nextPieceLabel->setAlignment(Qt::AlignCenter);

	startButton = new QPushButton("Начать игру");
	startButton->setFocusPolicy(Qt::NoFocus);
	quitButton = new QPushButton("Выход");
	quitButton->setFocusPolicy(Qt::NoFocus);
	pauseButton = new QPushButton("Пауза");
	pauseButton->setFocusPolicy(Qt::NoFocus);

	linesLcd = new QLCDNumber(5);
	linesLcd->setSegmentStyle(QLCDNumber::Filled);

	connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(startButton, SIGNAL(clicked()), mainBoard, SLOT(start()));
	connect(pauseButton, SIGNAL(clicked()), mainBoard, SLOT(pause()));
	connect(mainBoard, SIGNAL(linesRemovedChanged(int)), linesLcd, SLOT(display(int)));
	connect(mainBoard, SIGNAL(nextPieceChanged(piece *)), this, SLOT(drawNextPiece(piece *)));

	QGridLayout * layout = new QGridLayout;
	layout->addWidget(createLabel("Следующая фигура"), 0, 0);
	layout->addWidget(nextPieceLabel, 1, 0, 2, 1);
	layout->addWidget(startButton, 5, 0);
	layout->addWidget(mainBoard, 0, 1, 6, 1);
	layout->addWidget(createLabel("Линий заполнено"), 0, 2);
	layout->addWidget(linesLcd, 1, 2);
	layout->addWidget(quitButton, 4, 2);
	layout->addWidget(pauseButton, 5, 2);
	setLayout(layout);

	setWindowTitle("Супер тетрис");
	resize(700, 370);
}

QLabel * MainWindow::createLabel (const QString & text) {
	QLabel * lbl = new QLabel(text);
	lbl->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
	return lbl;
}

void MainWindow::drawNextPiece (piece * np) {
	QRect rect = nextPieceLabel->contentsRect();
	int rect_min_size = qMin(rect.height(), rect.width());
	double pix_size = static_cast<double>(rect_min_size) / piece::FIELD_SIZE;
	QPixmap pixmap(pix_size * np->getDcols(), pix_size * np->getDrows());
	QPainter painter(& pixmap);
	painter.fillRect(pixmap.rect(), nextPieceLabel->palette().background());
	np->drawFigure(painter, pix_size * np->getDcols(), 0, pix_size, pix_size);
	nextPieceLabel->setPixmap(pixmap);
}
