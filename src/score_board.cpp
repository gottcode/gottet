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

#include "score_board.h"

#include <QHeaderView>
#include <QPushButton>
#include <QSettings>
#include <QTreeWidget>
#include <QVBoxLayout>

/*****************************************************************************/

ScoreBoard::ScoreBoard(QWidget* parent)
:	QDialog(parent)
{
	setWindowTitle(tr("High Scores"));

	// Create tree view
	m_scores = new QTreeWidget(this);
	m_scores->setHeaderLabels(QStringList() << tr("Level") << tr("Lines") << tr("Score"));
	m_scores->header()->setResizeMode(QHeaderView::ResizeToContents);
	m_scores->setRootIsDecorated(false);

	// Load scores
	QStringList scores = QSettings().value("Scores").toString().split(";", QString::SkipEmptyParts);
	QStringList values;
	foreach (const QString& score, scores) {
		values = score.split(",", QString::SkipEmptyParts);
		if (values.count() != 3)
			continue;
		m_scores->addTopLevelItem( createItem(values[0].toInt(), values[1].toInt(), values[2].toInt()) );
	}

	// Create OK button
	QPushButton* ok_button = new QPushButton(tr("OK"), this);
	connect(ok_button, SIGNAL(clicked()), this, SLOT(accept()));

	// Layout window
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(24);
	layout->addWidget(m_scores);
	layout->addWidget(ok_button, 0, Qt::AlignRight);
}

/*****************************************************************************/

int ScoreBoard::highScorePosition(int score) const
{
	// Empty scores can't go on scoreboard
	if (score == 0)
		return 10;

	// Return position
	int position = 0;
	for (position = 0; position < m_scores->topLevelItemCount(); ++position) {
		if (score > m_scores->topLevelItem(position)->data(2, Qt::UserRole).toInt())
			break;
	}
	return position;
}

/*****************************************************************************/

void ScoreBoard::addHighScore(int level, int lines, int score)
{
	// Find position in high score list
	int position = highScorePosition(score);
	if (position >= 10)
		return;

	// Add score to list
	if (m_scores->topLevelItemCount() == 10)
		delete m_scores->takeTopLevelItem(9);
	QTreeWidgetItem* item = createItem(level, lines, score);
	m_scores->insertTopLevelItem(position, item);
	m_scores->clearSelection();
	item->setSelected(true);

	// Save scores
	QStringList scores;
	for (int i = 0; i < m_scores->topLevelItemCount(); ++i) {
		item = m_scores->topLevelItem(i);
		scores.append(QString("%1,%2,%3").arg(item->text(0)).arg(item->text(1)).arg(item->data(2, Qt::UserRole).toInt()));
	}
	QSettings().setValue("Scores", scores.join(";"));

	show();
}

/*****************************************************************************/

QTreeWidgetItem* ScoreBoard::createItem(int level, int lines, int score) const
{
	QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QString::number(level) << QString::number(lines) << QString("%L1").arg(score));
	item->setData(2, Qt::UserRole, score);
	item->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
	item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
	item->setTextAlignment(2, Qt::AlignRight | Qt::AlignVCenter);
	return item;
}

/*****************************************************************************/
