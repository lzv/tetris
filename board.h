#ifndef BOARD_H
#define BOARD_H

#include <QBasicTimer>
#include <QFrame>
#include <QPointer>
#include "piece.h"

class QLabel;

class board : public QFrame
{
	Q_OBJECT

	public:
		typedef quint32 lines_type; // Тип для битовых масок поля. Обязательно беззнаковый.
		typedef quint8 index_type; // Для индексов в массивах. Обязательно беззнаковый.

		static const index_type BOARD_WIDTH = 12; // Ширина поля в ячейках. Для quint32 максимум 32 (lines_type).
		static const index_type BOARD_HEIGHT = 25; // Высота поля в ячейках.

		board (QWidget * = 0);

	public slots:
		void start();
		void pause();

	signals:
		void linesRemovedChanged (int);
		void nextPieceChanged (piece *);

	protected:
		void paintEvent (QPaintEvent *);
		void keyPressEvent(QKeyEvent *);
		void timerEvent(QTimerEvent *);

	private:
		lines_type lines[BOARD_HEIGHT];
		QBasicTimer timer;
		int lines_removed;
		bool isStarted;
		bool isPaused;
		bool isWaitingAfterLine;
		piece curPiece;
		piece nextPiece;
		int curRow; // Строка текущего кусочка, считая из правого верхнего угла.
		int curCol; // Колонка текущего кусочка, аналогично.

		int timeoutTime() {return 500;}		// Количество милисекунд между падениями кусочка.
		bool checkAllowPosition (const piece &, index_type row, index_type col); // Проверка, есть ли место для кусочка.
		void newPiece();					// Создание нового кусочка и помещение его на поле.
		void mergeCurPiece();				// Присоединяет текущий кусочек к полю.
		void removeFullLines ();			// Удаление заполненных строк.
		void clearData ();					// Очистка данных поля.
		int squaresWidth(index_type);		// Возвращает ширину указанного количества ячеек поля. Подход с получением ширины одного и умножением на количество здесь не используется, что бы избежать погрешности округления.
		int squaresHeight(index_type);		// Аналогично для высоты.
};

#endif // BOARD_H
