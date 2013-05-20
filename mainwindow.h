#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QLCDNumber;
class QLabel;
class QPushButton;
class board;
class piece;

class MainWindow : public QWidget {
	Q_OBJECT

	public:
		MainWindow ();

	public slots:
		void drawNextPiece (piece *);

	private:
		board * mainBoard;			// Игровое поле, в котором будут падать кусочки.
		QLabel * nextPieceLabel;	// Виджет, в котором будет показана следующий кусочек.
		QLCDNumber * linesLcd;		// Дисплей с количеством собранных строк.
		QPushButton * startButton;	// Кнопка начала игры.
		QPushButton * quitButton;	// Кнопка выхода из программы.
		QPushButton * pauseButton;	// Кнопка паузы.

		QLabel * createLabel (const QString &); // Простое создание виджета с текстом.
};

#endif // MAINWINDOW_H
