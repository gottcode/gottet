/***********************************************************************
 *
 * Copyright (C) 2007, 2008, 2010 Graeme Gott <graeme@gottcode.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "board.h"

#include "piece.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QTimeLine>

/*****************************************************************************/

static QColor colors[] = {
	QColor("#0057ae"),
	QColor("#006e29"),
	QColor("#9c0f0f"),
	QColor("#539ae3"),
	QColor("#8f6b32"),
	QColor("#ec7331"),
	QColor("#644a9b")
};

/*****************************************************************************/

Board::Board(QWidget* parent)
:	QWidget(parent),
	m_removed_lines(0),
	m_level(1),
	m_score(0),
	m_piece(0),
	m_next_piece((rand() % 7) + 1),
	m_piece_size(0),
	m_started(false),
	m_done(false),
	m_paused(false)
{
#if !defined(QTOPIA_PHONE)
	setMinimumSize(201, 401);
	setFocusPolicy(Qt::StrongFocus);
#endif
	setFocus();

	m_shift_timer = new QTimeLine(500, this);
	m_shift_timer->setUpdateInterval(5);
	m_shift_timer->setCurveShape(QTimeLine::LinearCurve);
	connect(m_shift_timer, SIGNAL(finished()), this, SLOT(shiftPiece()));

	m_flash_timer = new QTimeLine(480, this);
	m_flash_timer->setCurveShape(QTimeLine::LinearCurve);
	m_flash_timer->setFrameRange(0, 6);
	connect(m_flash_timer, SIGNAL(frameChanged(int)), this, SLOT(flashLines(int)));
	connect(m_flash_timer, SIGNAL(finished()), this, SLOT(removeLines()));

	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;

	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			m_cells[col][row] = 0;
		}
	}
}

/*****************************************************************************/

bool Board::cell(int x, int y) const
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);

	return m_cells[x][y] != 0;
}

/*****************************************************************************/

void Board::addCell(int x, int y, int type)
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);
	Q_ASSERT(type > 0 && type < 8);
	Q_ASSERT(m_cells[x][y] == 0);

	m_cells[x][y] = type;
}

/*****************************************************************************/

void Board::removeCell(int x, int y)
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);

	m_cells[x][y] = 0;
}

/*****************************************************************************/

bool Board::endGame()
{
	if (m_done || !m_started) {
		return true;
	}

	if (QMessageBox::question(this, tr("Question"), tr("End the current game?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		gameOver();
		return true;
	} else {
		return false;
	}
}

/*****************************************************************************/

void Board::findFullLines()
{
	// Empty list of full lines
	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;
	int pos = 0;

	// Find full lines
	bool full = false;
	for (int row = 0; row < 20; ++row) {
		full = true;
		for (int col = 0; col < 10; ++col) {
			if (m_cells[col][row] == 0)
				full = false;
		}
		if (full) {
			m_full_lines[pos] = row;
			++pos;
		}
	}
}

/*****************************************************************************/

void Board::newGame()
{
	if (!endGame()) {
		return;
	}

	m_flash_timer->stop();
	m_shift_timer->stop();
	delete m_piece;
	m_piece = 0;

	m_started = true;
	m_done = false;
	m_paused = false;
	m_removed_lines = 0;
	m_level = 1;
	m_score = 0;
	m_shift_timer->setDuration(500);
	m_next_piece = (rand() % 7) + 1;

	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;

	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			m_cells[col][row] = 0;
		}
	}

	emit pauseAvailable(true);
	emit levelUpdated(m_level);
	emit linesRemovedUpdated(m_removed_lines);
	emit scoreUpdated(m_score);
	emit gameStarted();

	setCursor(Qt::BlankCursor);
	createPiece();
}

/*****************************************************************************/

void Board::pauseGame()
{
	m_paused = true;
	if (m_shift_timer->state() == QTimeLine::Running)
		m_shift_timer->setPaused(true);
	if (m_flash_timer->state() == QTimeLine::Running)
		m_flash_timer->setPaused(true);

	update();

	unsetCursor();
	emit pauseAvailable(false);
}

/*****************************************************************************/

void Board::resumeGame()
{
	m_paused = false;
	if (m_shift_timer->state() == QTimeLine::Paused)
		m_shift_timer->setPaused(false);
	if (m_flash_timer->state() == QTimeLine::Paused)
		m_flash_timer->setPaused(false);

	update();

	setCursor(Qt::BlankCursor);
	emit pauseAvailable(true);
}

/*****************************************************************************/

void Board::keyPressEvent(QKeyEvent* event)
{
	if (!m_piece || m_paused)
		return;

	switch (event->key()) {
	case Qt::Key_Left:
		m_piece->moveLeft();
		break;
	case Qt::Key_Right:
		m_piece->moveRight();
		break;
	case Qt::Key_Up:
		m_piece->rotate();
		break;
	case Qt::Key_Down:
		m_piece->drop();
		landPiece();
		break;
	default:
		break;
	}

	update();
}

/*****************************************************************************/

void Board::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillRect(m_background, Qt::black);

	if (m_paused) {
		renderText(painter, tr("Paused"));
		return;
	}

	for (int col = 0; col < 10; ++col) {
		for (int row = 0; row < 20; ++row) {
			int cell = m_cells[col][row] - 1;
			if (cell >= 0) {
				painter.drawPixmap(col * m_piece_size + m_background.x(), row * m_piece_size + m_background.y(), m_images[cell]);
			}
		}
	}

	if (m_done) {
		renderText(painter, tr("Game Over"));
	}
}

/*****************************************************************************/

void Board::focusOutEvent(QFocusEvent*)
{
	if (m_piece && !m_done && !m_paused)
		pauseGame();
}

/*****************************************************************************/

void Board::resizeEvent(QResizeEvent* event)
{
	m_piece_size = qMin(event->size().width() / 10, event->size().height() / 20);
	int w = m_piece_size * 10 + 1;
	int h = m_piece_size * 20 + 1;
	m_background = QRect((width() - w) / 2, (height() - h) / 2, w, h);

	QPainter painter;

	for (int i = 0; i < 7; ++i) {
		QPixmap pixmap(m_piece_size + 1, m_piece_size + 1);
		pixmap.fill(QColor(0, 0, 0, 0));

		painter.begin(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(colors[i]);
		painter.setPen(QPen(colors[i].lighter(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter.drawRoundRect(1, 1, m_piece_size - 1, m_piece_size - 1);
		painter.end();

		m_images[i] = pixmap;
	}
}

/*****************************************************************************/

void Board::shiftPiece()
{
	Q_ASSERT(m_piece != 0);

	if (m_piece->moveDown()) {
		update();
		m_shift_timer->start();
	} else {
		landPiece();
	}
}

/*****************************************************************************/

void Board::flashLines(int frame)
{
	if (frame < 6) {
		bool flash = m_cells[0][m_full_lines[0]] > 0;
		int amount = flash ? -7 : 7;

		for (int i = 0; i < 4; ++i) {
			int row = m_full_lines[i];
			if (row == -1)
				break;

			for (int col = 0; col < 10; ++col) {
				m_cells[col][row] += amount;
			}
		}

		update();
	}
}

/*****************************************************************************/

void Board::removeLines()
{
	int score = 14 * m_level;

	// Loop through full lines
	for (int i = 0; i < 4; ++i) {
		int row = m_full_lines[i];
		if (row == -1)
			break;

		// Remove line
		for (int col = 0; col < 10; ++col) {
			removeCell(col, row);
		}
		++m_removed_lines;
		score *= 3;

		// Shift board down
		for (; row > 0; --row) {
			for (int col = 0; col < 10; ++col) {
				m_cells[col][row] = m_cells[col][row - 1];
			}
		}
	}

	// Remove top line
	if (m_full_lines[0] != -1) {
		for (int col = 0; col < 10; ++col) {
			removeCell(col, 0);
		}
	}

	m_level = (m_removed_lines / 10) + 1;
	m_shift_timer->setDuration(10000 / (m_removed_lines + 20));
	m_score += score;
	emit levelUpdated(m_level);
	emit linesRemovedUpdated(m_removed_lines);
	emit scoreUpdated(m_score);

	// Empty list of full lines
	for (int i = 0; i < 4; ++i)
		m_full_lines[i] = -1;

	// Add new piece
	createPiece();
}

/*****************************************************************************/

void Board::gameOver()
{
	delete m_piece;
	m_piece = 0;
	m_done = true;
	unsetCursor();
	emit gameOver(m_level, m_removed_lines, m_score);
}

/*****************************************************************************/

void Board::createPiece()
{
	Q_ASSERT(m_piece == 0);

	m_piece = new Piece(m_next_piece, this);
	if (m_piece->isValid()) {
		m_next_piece = (rand() % 7) + 1;
		emit nextPieceAvailable(renderPiece(m_next_piece));
		m_shift_timer->start();
	} else {
		gameOver();
	}
	update();
}

/*****************************************************************************/

void Board::landPiece()
{
	m_shift_timer->stop();

	delete m_piece;
	m_piece = 0;

	findFullLines();
	if (m_full_lines[0] != -1) {
		m_flash_timer->start();
	} else {
		createPiece();
	}
}

/*****************************************************************************/

void Board::renderText(QPainter& painter, const QString& message) const
{
	painter.setFont(QFont("Sans", 24));
	QRect rect = painter.fontMetrics().boundingRect(message);
	int x1 = (width() - rect.width() - rect.height()) >> 1;
	int y1 = (height() >> 1) - rect.height();

	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(255, 255, 255, 200));
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawRoundRect(x1, y1, rect.width() + rect.height(), rect.height() * 2, 10);

	painter.setPen(Qt::black);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.drawText((rect.height() >> 1) + x1, ((rect.height() * 5) >> 2) + y1, message);
}

/*****************************************************************************/

QPixmap Board::renderPiece(int type) const
{
	Q_ASSERT(type > 0 && type < 8);

	Cell piece[4];
	Piece::cells(piece, type);

	QPixmap result(80, 100);
	result.fill(Qt::black);
	{
		QPainter painter(&result);
		painter.setRenderHint(QPainter::Antialiasing, true);
		if (type == 1) {
			painter.translate(30, 10);
		} else if (type < 7) {
			painter.translate(20, 20);
		} else {
			painter.translate(20, 30);
		}

		for (int i = 0; i < 4; ++i) {
			painter.setBrush(colors[type - 1]);
			painter.setPen(QPen(colors[type - 1].lighter(), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
			painter.drawRoundRect(piece[i].x * 20, piece[i].y * 20, 19, 19);
		}
	}

#if defined(QTOPIA_PHONE)
	result = result.scaled(30,50, Qt::KeepAspectRatio);
#endif

	return result;
}

/*****************************************************************************/
