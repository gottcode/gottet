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

#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
class QPixmap;
class QTimer;
class Piece;

#include <random>

class Board : public QWidget
{
	Q_OBJECT
public:
	Board(QWidget* parent = 0);

	bool cell(int x, int y) const;
	bool endGame();
	void findFullLines();

public slots:
	void newGame();
	void pauseGame();
	void resumeGame();

signals:
	void pauseAvailable(bool available);
	void nextPieceAvailable(QPixmap piece);
	void levelUpdated(int level);
	void linesRemovedUpdated(int lines);
	void scoreUpdated(int score);
	void gameOver(int level, int lines, int score);
	void gameStarted();
	void hideMessage();
	void showMessage(const QString& message);

protected:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void paintEvent(QPaintEvent*);
	virtual void focusOutEvent(QFocusEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void shiftPiece();
	void flashLines();
	void removeLines();

private:
	void gameOver();
	void addCell(int x, int y, int type);
	void removeCell(int x, int y);
	void createPiece();
	void landPiece();
	int nextPiece();
	QPixmap renderPiece(int type) const;

private:
	QPixmap m_images[7];
	int m_cells[10][20];
	int m_full_lines[4];
	int m_removed_lines;
	int m_level;
	int m_score;
	Piece* m_piece;
	int m_next_piece;
	QTimer* m_shift_timer;
	QTimer* m_flash_timer;
	int m_flash_frame;
	int m_piece_size;
	QRect m_background;
	bool m_started;
	bool m_done;
	bool m_paused;

	std::mt19937 m_random_generator;
	std::uniform_int_distribution<int> m_random_distribution;
};

inline bool Board::cell(int x, int y) const
{
	Q_ASSERT(x >= 0 && x < 10);
	Q_ASSERT(y >= 0 && y < 20);
	return m_cells[x][y] != 0;
}

inline int Board::nextPiece()
{
	return m_random_distribution(m_random_generator);
}

#endif // BOARD_H
