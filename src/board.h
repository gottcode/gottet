/*
	SPDX-FileCopyrightText: 2007-2010 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GOTTET_BOARD_H
#define GOTTET_BOARD_H

class Piece;

#include <QRandomGenerator>
#include <QWidget>
class QPixmap;
class QTimer;

class Board : public QWidget
{
	Q_OBJECT

public:
	explicit Board(QWidget* parent = nullptr);

	bool cell(int x, int y) const
	{
		Q_ASSERT(x >= 0 && x < 10);
		Q_ASSERT(y >= 0 && y < 20);
		return m_cells[x][y] != 0;
	}

	bool endGame();
	void findFullLines();

public Q_SLOTS:
	void newGame();
	void pauseGame();
	void resumeGame();

Q_SIGNALS:
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
	void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent*) override;
	void focusOutEvent(QFocusEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private Q_SLOTS:
	void shiftPiece();
	void flashLines();
	void removeLines();

private:
	void gameOver();
	void addCell(int x, int y, int type);
	void removeCell(int x, int y);
	void createPiece();
	void landPiece();

	int nextPiece()
	{
		return m_random_generator.bounded(1, 8);
	}

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

	QRandomGenerator m_random_generator;
};

#endif // GOTTET_BOARD_H
