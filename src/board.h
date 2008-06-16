/***********************************************************************
 *
 * Copyright (C) 2007-2008 Graeme Gott <graeme@gottcode.org>
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
class QTimeLine;
class Piece;

class Board : public QWidget
{
	Q_OBJECT
public:
	Board(QWidget* parent = 0);

	bool cell(int x, int y) const;
	void addCell(int x, int y, int type);
	void removeCell(int x, int y);
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

protected:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void paintEvent(QPaintEvent*);
	virtual void focusOutEvent(QFocusEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void shiftPiece();
	void flashLines(int frame);
	void removeLines();

private:
	void createPiece();
	void landPiece();
	void renderText(QPainter& painter, const QString& message) const;
	QPixmap renderPiece(int type) const;

	QPixmap m_images[7];
	int m_cells[10][20];
	int m_full_lines[4];
	int m_removed_lines;
	int m_level;
	int m_score;
	Piece* m_piece;
	int m_next_piece;
	QTimeLine* m_shift_timer;
	QTimeLine* m_flash_timer;
	bool m_done;
	bool m_paused;
};

#endif // BOARD_H
