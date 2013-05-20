#ifndef PIECE_H
#define PIECE_H

#include <QtGlobal>
#include <QColor>

class QPoint;
class QPainter;

// Координаты в поле кусочка (строка и столбец) отсчитываются от правого верхнего угла - вниз и влево. Первая координата 0.

class piece {

	public:
		typedef quint8 lines_type; // Тип для битовых масок поля кусочка. Обязательно беззнаковый.
		typedef quint8 index_type; // Для индексов в массивах. Обязательно беззнаковый.

		// Размены поля кусочка, по сути его максимально возможные ширина и высота. Для quint8 максимум 8 (lines_type).
		static const index_type FIELD_SIZE = 4;
		static const QColor DRAW_COLOR;

		piece ();
		piece (lines_type []);
		piece (const piece &);
		void setFigure (lines_type []);
		void clear ();								// Очистка всех данных объекта.
		void rotateLeft ();							// Поворот против часовой.
		void rotateRight ();						// Поворот по часовой.
		index_type getDcols () const;				// Ширина кусочка.
		index_type getDrows () const;				// Высота кусочка.
		lines_type getLine (index_type) const;		// Получить линию поля кусочка.
		bool check_point (index_type, index_type);	// Проверить, есть ли пиксел в указанной ячейке поля.
		bool check_point (const QPoint &);			// Аналогично, x - строка, y - столбец.
		void setTrueRandomFigure ();				// Получение случайного кусочка из всех возможных.
		void setRandomFigure ();					// Получение случайной фигуры без "оторванных" пикселов.
		void drawFigure (QPainter &, int, int, int, int);	// Рисуем весь кусочек. Координаты правого верхнего угла.
		void drawSquare (QPainter &, const QColor &, int, int, int, int);	// Рисуем пиксел кусочка. Координаты так же.

	private:
		void check_empty_space ();					// Сдвигает кусочек вверх-вправо поля кусочка.
		bool check_col_is_zero (index_type) const;	// Проверка, пуст ли указанный столбец поля кусочка.

		lines_type lines[FIELD_SIZE]; // Битовая маска кусочка.

		void debug_showFigure ();	// Для отладки. Вывод фигуры в двоичном виде в консоли отладки.
};

#endif // PIECE_H
