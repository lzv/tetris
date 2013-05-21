#include <QtGui>
#include "board.h"

board::board (QWidget * parent)
	: QFrame(parent), lines_removed(0), isStarted(false), isPaused(false), isWaitingAfterLine(false)
{
	clearData();
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setFocusPolicy(Qt::StrongFocus);
	setMinimumSize(BOARD_WIDTH * 20, BOARD_HEIGHT * 20);
}

void board::paintEvent (QPaintEvent * event)
{
	QFrame::paintEvent(event);

	QPainter painter(this);
	QRect rect = contentsRect();

	if (isPaused) {
		painter.drawText(rect, Qt::AlignCenter, "Пауза");
		return;
	}

	// Вертикальные полосы.
	painter.setPen(QColor("#dddddd"));
	int x;
	for (index_type i = 1; i < BOARD_WIDTH; ++i) {
		x = rect.left() + squaresWidth(i);
		painter.drawLine(x, rect.top(), x, rect.bottom());
	}

    if (isStarted) drawFigure(painter, curPiece);

	lines_type mask;
	for (index_type r = 0; r < BOARD_HEIGHT; ++r) {
		mask = 1;
		for (index_type c = 0; c < BOARD_WIDTH; ++c) {
			if (lines[r] & mask) drawBoardCell(painter, piece::DRAW_COLOR, c, r);
			mask <<= 1;
		}
	}
}

void board::drawFigure (QPainter & painter, const piece & figure) {
    for (index_type row = 0; row < piece::FIELD_SIZE; ++row)
        for (index_type col = 0; col < piece::FIELD_SIZE; ++col)
            if (figure.check_point(row, col)) drawBoardCell(painter, piece::DRAW_COLOR, curCol + col, curRow + row);
}

void board::drawBoardCell (QPainter & painter, const QColor & color, int x, int y)
{
    QRect rect = contentsRect();
    int w = squaresWidth(1);
    int h = squaresHeight(1);
    x = rect.left() + squaresWidth(BOARD_WIDTH - x - 1);
    y = rect.top() + squaresHeight(y);
    
    painter.fillRect(x + 1, y + 1, w - 2, h - 2, color);
    painter.setPen(color.light());
    painter.drawLine(x, y + h - 2, x, y);
    painter.drawLine(x, y, x + w - 1, y);
    painter.setPen(color.dark());
    painter.drawLine(x, y + h - 1, x + w - 1, y + h - 1);
    painter.drawLine(x + w - 1, y + h - 1, x + w - 1, y + 1);
}

void board::start ()
{
	isStarted = true;
	isPaused = false;
	isWaitingAfterLine = false;
	lines_removed = 0;
	clearData();
	nextPiece.setRandomFigure();
	emit linesRemovedChanged(lines_removed);
	timer.start(timeoutTime(), this);
	newPiece();
}

void board::pause () {
	if (!isStarted) return;
	isPaused = !isPaused;
	isPaused ? timer.stop() : timer.start(timeoutTime(), this);
	update();
}

void board::timerEvent (QTimerEvent * event)
{
	if (event->timerId() == timer.timerId()) {
		if (checkAllowPosition(curPiece, curRow + 1, curCol)) {
			++curRow;
			update();
		} else {
			mergeCurPiece();
			removeFullLines();
			newPiece();
		}
	} else {
		QFrame::timerEvent(event);
	}
}

void board::keyPressEvent(QKeyEvent * event)
{
	if (!isStarted || isPaused) {
		QFrame::keyPressEvent(event);
		return;
	}

	piece tempPiece;
	switch (event->key()) {
		case Qt::Key_Left:
			if (checkAllowPosition(curPiece, curRow, curCol + 1)) {
				++curCol;
				update();
			}
			break;
		case Qt::Key_Right:
			if (checkAllowPosition(curPiece, curRow, curCol - 1)) {
				--curCol;
				update();
			}
			break;
		case Qt::Key_Up:
			tempPiece = curPiece;
			tempPiece.rotateRight();
			if (checkAllowPosition(tempPiece, curRow, curCol)) {
				curPiece = tempPiece;
				update();
			}
			break;
		case Qt::Key_Down:
			tempPiece = curPiece;
			tempPiece.rotateLeft();
			if (checkAllowPosition(tempPiece, curRow, curCol)) {
				curPiece = tempPiece;
				update();
			}
			break;
		case Qt::Key_Space:
			while (checkAllowPosition(curPiece, curRow + 1, curCol)) ++curRow;
			update();
			break;
		default:
			QFrame::keyPressEvent(event);
	}
}

void board::newPiece () {
	curPiece = nextPiece;
	nextPiece.setRandomFigure();
	emit nextPieceChanged(& nextPiece);
	curRow = 0;
	curCol = BOARD_WIDTH / 2 - piece::FIELD_SIZE / 2;
	if (checkAllowPosition(curPiece, curRow, curCol)) {
		update();
	} else {
		isStarted = false;
		timer.stop();
	}
}

void board::removeFullLines () {
	lines_type mask = -1;
	mask <<= BOARD_WIDTH;
	mask = ~mask;
	index_type i, j;
	for (i = BOARD_HEIGHT - 1; i > 0; --i)
		if (lines[i] == mask) {
			for (j = i - 1; j < i; --j) lines[j + 1] = lines[j];
			lines[0] = 0;
			++lines_removed;
			++i;
		}
	emit linesRemovedChanged(lines_removed);
}

bool board::checkAllowPosition (const piece & p, index_type row, index_type col) {
	if (row + p.getDrows() > BOARD_HEIGHT or col + p.getDcols() > BOARD_WIDTH) return false;
	lines_type mask;
	for (piece::index_type i = 0; i < piece::FIELD_SIZE; ++i) {
		mask = p.getLine(i);
		mask <<= col;
		if (mask & lines[row + i]) return false;
	}
	return true;
}

void board::mergeCurPiece () {
	lines_type mask;
	for (piece::index_type i = 0; i < piece::FIELD_SIZE; ++i) {
		mask = curPiece.getLine(i);
		mask <<= curCol;
		lines[curRow + i] |= mask;
	}
}

void board::clearData () {
	for (index_type i = 0; i < BOARD_HEIGHT; ++i) lines[i] = 0;
}

int board::squaresWidth(index_type count) {
    return count * (static_cast<qreal>(contentsRect().width()) / BOARD_WIDTH);
}

int board::squaresHeight(index_type count) {
    return count * (static_cast<qreal>(contentsRect().height()) / BOARD_HEIGHT);
}
