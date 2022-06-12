/*
	SPDX-FileCopyrightText: 2008-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GOTTET_WINDOW_H
#define GOTTET_WINDOW_H

class Board;

#include <QMainWindow>
class QAction;
class QLabel;

class Window : public QMainWindow
{
	Q_OBJECT

public:
	explicit Window(QWidget *parent = nullptr);

protected:
	void closeEvent(QCloseEvent* event) override;

private Q_SLOTS:
	void pauseAvailable(bool available);
	void togglePaused(bool paused);
	void scoreUpdated(int score);
	void showScores();
	void newGame();
	void gameOver(int level, int lines, int score);
	void about();
	void setLocale();

private:
	Board* m_board;
	QAction* m_pause_action;
	QLabel* m_preview;
	QLabel* m_level;
	QLabel* m_lines;
	QLabel* m_score;
};

#endif // GOTTET_WINDOW_H
