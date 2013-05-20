#include "piece.h"
#include <QtCore/qmath.h>
#include <QVector>
#include <QPoint>
#include <QPainter>
#include <QtDebug>

const QColor piece::DRAW_COLOR("#CC6666");

piece::piece () {
	clear();
}

piece::piece (lines_type figure []) {
	setFigure(figure);
}

piece::piece (const piece & val) {
	for (index_type i = 0; i < FIELD_SIZE; ++i) lines[i] = val.lines[i];
}

void piece::setFigure (lines_type figure []) {
	for (index_type i = 0; i < FIELD_SIZE; ++i) lines[i] = figure[i];
	check_empty_space();
}

void piece::clear () {
	for (index_type i = 0; i < FIELD_SIZE; ++i) lines[i] = 0;
}

piece::index_type piece::getDcols () const {
	for (index_type i = 0; i < FIELD_SIZE; ++i)
		if (check_col_is_zero(i)) return i;
	return FIELD_SIZE;
}

piece::index_type piece::getDrows () const {
	for (index_type i = 0; i < FIELD_SIZE; ++i)
		if (lines[i] == 0) return i;
	return FIELD_SIZE;
}

piece::lines_type piece::getLine(index_type ind) const {
	return ind >= FIELD_SIZE ? 0 : lines[ind];
}

bool piece::check_point(index_type row, index_type col) {
	if (row >= FIELD_SIZE or col >= FIELD_SIZE) return false;
	lines_type mask = 1;
	mask <<= col;
	return lines[row] & mask;
}

bool piece::check_point(const QPoint & p) {
	return check_point(p.x(), p.y());
}

void piece::setTrueRandomFigure () {
	quint64 max = qPow(2, FIELD_SIZE);
	do {
		for (index_type i = 0; i < FIELD_SIZE; ++i) lines[i] = qrand() % max;
		check_empty_space();
	} while (lines[0] == 0);
}

// Очищаем доску и добавляем FIELD_SIZE по одному. После каждого добавления берем места на поле по бокам от уже добавленных, и следующее добавление идет только в них. Таким образом, получается цельная фигура, и отсутствуют отдельные от нее пикселы.
// Можно увеличить количество вариантов фигур, сделав случайным количество пикселов, из которых состоит фигура.
void piece::setRandomFigure () {
	clear();
	index_type point_row = qrand() % FIELD_SIZE;
	index_type point_col = qrand() % FIELD_SIZE;
	lines[point_row] = static_cast<lines_type>(1) << point_col;
	QVector<QPoint> points;
	if (point_row + 1 < FIELD_SIZE) points << QPoint(point_row + 1, point_col);
	if (point_col + 1 < FIELD_SIZE) points << QPoint(point_row, point_col + 1);
	if (point_row > 0) points << QPoint(point_row - 1, point_col);
	if (point_col > 0) points << QPoint(point_row, point_col - 1);
	int replaced_point;
	for (index_type i = 1; i < FIELD_SIZE; ++i) {
		replaced_point = qrand() % points.size();
		point_row = points[replaced_point].x();
		point_col = points[replaced_point].y();
		lines[point_row] |= static_cast<lines_type>(1) << point_col;
		points.remove(replaced_point);
		QPoint p1(point_row + 1, point_col);
		if (point_row + 1 < FIELD_SIZE and !check_point(p1) and !points.contains(p1)) points << p1;
		QPoint p2(point_row, point_col + 1);
		if (point_col + 1 < FIELD_SIZE and !check_point(p2) and !points.contains(p2)) points << p2;
		QPoint p3(point_row - 1, point_col);
		if (point_row > 0 and !check_point(p3) and !points.contains(p3)) points << p3;
		QPoint p4(point_row, point_col - 1);
		if (point_col > 0 and !check_point(p4) and !points.contains(p4)) points << p4;
	}
	check_empty_space();
}

void piece::rotateRight () {
	lines_type new_figure[FIELD_SIZE], mask;
	index_type i, j;
	for (i = FIELD_SIZE - 1; i < FIELD_SIZE; --i) { // Помним, что индекс - беззнаковый тип.
		new_figure[i] = 0;
		mask = 1;
		mask <<= FIELD_SIZE - i - 1;
		for (j = FIELD_SIZE - 1; j < FIELD_SIZE; --j) {
			if (lines[j] & mask) new_figure[i] += 1;
			if (j > 0) new_figure[i] <<= 1;
		}
	}
	for (i = 0; i < FIELD_SIZE; ++i) lines[i] = new_figure[i];
	check_empty_space();
}

/*
  00001011
  00000110
  00001010
  00001111
  */
void piece::rotateLeft () {
	lines_type new_figure[FIELD_SIZE], mask;
	index_type i, j;
	for (i = 0; i < FIELD_SIZE; ++i) {
		new_figure[i] = 0;
		mask = 1;
		mask <<= i;
		for (j = 0; j < FIELD_SIZE; ++j) {
			if (lines[j] & mask) new_figure[i] += 1;
			if (j < FIELD_SIZE - 1) new_figure[i] <<= 1;
		}
	}
	for (i = 0; i < FIELD_SIZE; ++i) lines[i] = new_figure[i];
	check_empty_space();
}

void piece::check_empty_space () {
	// Передвигаем кусочек в верх поля.
	index_type i, depth = 0;
	for (i = 0; i < FIELD_SIZE; ++i)
		if (lines[i] == 0) ++depth;
		else break;
	if (depth > 0 and depth < FIELD_SIZE) {
		for (i = 0; i < FIELD_SIZE - depth; ++i) lines[i] = lines[i + depth];
		for (i = FIELD_SIZE - depth; i < FIELD_SIZE; ++i) lines[i] = 0;
	}
	// Теперь передвигаем его вправо.
	depth = 0;
	lines_type mask;
	bool flag = false;
	for (i = 0; i < FIELD_SIZE; ++i) {
		mask = 1;
		mask <<= i;
		for (index_type j = 0; j < FIELD_SIZE; ++j) flag = flag or (lines[j] & mask);
		if (flag) break;
		else ++depth;
	}
	if (depth > 0 and depth < FIELD_SIZE)
		for (i = 0; i < FIELD_SIZE; ++i) lines[i] >>= depth;
}

bool piece::check_col_is_zero (index_type ind) const {
	if (ind >= FIELD_SIZE) {
		return true;
	} else {
		lines_type mask = 1;
		mask <<= ind;
		bool result = true;
		for (index_type i = 0; i < FIELD_SIZE; ++i)
			if (lines[i] & mask) {
				result = false;
				break;
			}
		return result;
	}
}

void piece::drawFigure (QPainter & painter, int x, int y, int pw, int ph) {
	for (index_type row = 0; row < FIELD_SIZE; ++row)
		for (index_type col = 0; col < FIELD_SIZE; ++col)
			if (check_point(row, col)) drawSquare(painter, DRAW_COLOR, x - pw * col, y + ph * row, pw, ph);
}

void piece::drawSquare (QPainter & painter, const QColor & color, int x, int y, int w, int h) {
	painter.fillRect(x - w + 2, y + 1, w - 2, h - 2, color);
	painter.setPen(color.light());
	painter.drawLine(x - w + 1, y + h - 1, x - w + 1, y);
	painter.drawLine(x - w + 1, y, x, y);
	painter.setPen(color.dark());
	painter.drawLine(x - w + 2, y + h - 1, x, y + h - 1);
	painter.drawLine(x, y + h - 1, x, y + 1);
}

void piece::debug_showFigure () {
	for (index_type i = 0; i < FIELD_SIZE; ++i) qDebug() << QString::number(lines[i], 2);
	qDebug() << "-----";
}
