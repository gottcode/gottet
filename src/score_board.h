/***********************************************************************
 *
 * Copyright (C) 2008 Graeme Gott <graeme@gottcode.org>
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

#ifndef SCORE_BOARD_H
#define SCORE_BOARD_H

#include <QDialog>
class QTreeWidget;
class QTreeWidgetItem;

class ScoreBoard : public QDialog
{
	Q_OBJECT
public:
	ScoreBoard(QWidget* parent = 0);

	int highScorePosition(int score) const;

public slots:
	void addHighScore(int level, int lines, int score);

private:
	QTreeWidgetItem* createItem(int level, int lines, int score) const;

	QTreeWidget* m_scores;
};

#endif // SCORE_BOARD_H
