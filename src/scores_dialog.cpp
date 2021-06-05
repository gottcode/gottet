/*
	SPDX-FileCopyrightText: 2008-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "scores_dialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QTime>

#if defined(Q_OS_UNIX)
#include <pwd.h>
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <lmcons.h>
#include <windows.h>
#endif

#include <cmath>

//-----------------------------------------------------------------------------

int ScoresDialog::m_max = -1;
int ScoresDialog::m_min = -1;

//-----------------------------------------------------------------------------

ScoresDialog::ScoresDialog(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("High Scores"));

	QSettings settings;

	// Load default name
	m_default_name = settings.value("Scores/DefaultName").toString();
	if (m_default_name.isEmpty()) {
#if defined(Q_OS_UNIX)
		passwd* pws = getpwuid(geteuid());
		if (pws) {
			m_default_name = QString::fromLocal8Bit(pws->pw_gecos).section(',', 0, 0);
			if (m_default_name.isEmpty()) {
				m_default_name = QString::fromLocal8Bit(pws->pw_name);
			}
		}
#elif defined(Q_OS_WIN)
		TCHAR buffer[UNLEN + 1];
		DWORD count = UNLEN + 1;
		if (GetUserName(buffer, &count)) {
			m_default_name = QString::fromWCharArray(buffer, count);
		}
#endif
	}

	m_username = new QLineEdit(this);
	m_username->hide();
	connect(m_username, &QLineEdit::editingFinished, this, &ScoresDialog::editingFinished);

	// Create score widgets
	m_scores_layout = new QGridLayout(this);
	m_scores_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_scores_layout->setHorizontalSpacing(18);
	m_scores_layout->setVerticalSpacing(6);
	m_scores_layout->setColumnStretch(NameColumn, 1);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Rank") + "</b>", this), 1, RankColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Name") + "</b>", this), 1, NameColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Level") + "</b>", this), 1, LevelColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Lines") + "</b>", this), 1, LinesColumn, Qt::AlignCenter);
	m_scores_layout->addWidget(new QLabel("<b>" + tr("Score") + "</b>", this), 1, ScoreColumn, Qt::AlignCenter);

	QFrame* divider = new QFrame(this);
	divider->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	m_scores_layout->addWidget(divider, 2, 0, 1, TotalColumns);

	QVector<Qt::Alignment> alignments(TotalColumns, Qt::AlignTrailing);
	alignments[NameColumn] = Qt::AlignLeading;
	for (int r = 0; r < 10; ++r) {
		m_score_labels[r][0] = new QLabel(tr("#%1").arg(r + 1), this);
		m_scores_layout->addWidget(m_score_labels[r][0], r + 3, 0, alignments[RankColumn] | Qt::AlignVCenter);
		for (int c = RankColumn + 1; c < TotalColumns; ++c) {
			m_score_labels[r][c] = new QLabel("-", this);
			m_scores_layout->addWidget(m_score_labels[r][c], r + 3, c, alignments[c] | Qt::AlignVCenter);
		}
	}

	// Populate scores widgets with values
	load(settings);

	// Lay out dialog
	m_buttons = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	m_buttons->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons));
	m_buttons->button(QDialogButtonBox::Close)->setDefault(true);
	m_buttons->button(QDialogButtonBox::Close)->setFocus();
	connect(m_buttons, &QDialogButtonBox::rejected, this, &ScoresDialog::reject);
	m_scores_layout->addWidget(m_buttons, 13, 0, 1, TotalColumns);
}

//-----------------------------------------------------------------------------

bool ScoresDialog::addScore(int level, int lines, int score)
{
	// Add score
	if (!addScore(m_default_name, level, lines, score)) {
		return false;
	}

	// Inform player of success
	QLabel* label = new QLabel(this);
	label->setAlignment(Qt::AlignCenter);
	if (m_row == 0) {
		label->setText(QString("<big>🎉</big> %1<br>%2").arg(tr("Congratulations!"), tr("You beat your top score!")));
	} else {
		label->setText(QString("<big>🙌</big> %1<br>%2").arg(tr("Well done!"), tr("You have a new high score!")));
	}
	m_scores_layout->addWidget(label, 0, 0, 1, TotalColumns);

	// Add score to display
	updateItems();

	// Show lineedit
	m_scores_layout->addWidget(m_username, m_row + 3, 1);
	m_score_labels[m_row][1]->hide();
	m_username->setText(m_scores[m_row].name);
	m_username->show();
	m_username->setFocus();

	m_buttons->button(QDialogButtonBox::Close)->setDefault(false);

	return true;
}

//-----------------------------------------------------------------------------

int ScoresDialog::isHighScore(int score)
{
	if (m_max == -1) {
		m_max = 1;
		ScoresDialog();
	}

	if (score >= m_max) {
		return 2;
	} else if (score >= m_min) {
		return 1;
	} else {
		return 0;
	}
}

//-----------------------------------------------------------------------------

void ScoresDialog::migrate()
{
	QSettings settings;
	if (settings.contains("Scores_Gottet")) {
		return;
	}

	const QStringList scores = QSettings().value("Scores").toString().split(";", Qt::SkipEmptyParts);
	settings.remove("Scores");
	settings.remove("ScoresSize");

	if (scores.isEmpty()) {
		return;
	}

	settings.beginWriteArray("Scores_Gottet");
	int index = 0;
	for (const QString& score : scores) {
		const QStringList values = score.split(",", Qt::SkipEmptyParts);
		if (values.size() != 3) {
			continue;
		}

		settings.setArrayIndex(index);
		settings.setValue("Level", values[0].toInt());
		settings.setValue("Lines", values[1].toInt());
		settings.setValue("Score", values[2].toInt());
		++index;
	}
	settings.endArray();
}

//-----------------------------------------------------------------------------

void ScoresDialog::hideEvent(QHideEvent* event)
{
	editingFinished();
	QDialog::hideEvent(event);
}

//-----------------------------------------------------------------------------

void ScoresDialog::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		event->ignore();
		return;
	}
	QDialog::keyPressEvent(event);
}

//-----------------------------------------------------------------------------

void ScoresDialog::editingFinished()
{
	if (!m_username->isVisible()) {
		return;
	}

	Q_ASSERT(m_row != -1);

	// Set player name
	m_scores[m_row].name = m_username->text();
	m_score_labels[m_row][1]->setText("<b>" + m_scores[m_row].name + "</b>");

	// Hide lineedit
	m_username->hide();
	m_scores_layout->removeWidget(m_username);
	m_score_labels[m_row][1]->show();

	m_buttons->button(QDialogButtonBox::Close)->setDefault(true);
	m_buttons->button(QDialogButtonBox::Close)->setFocus();

	// Save scores
	QSettings settings;
	settings.setValue("Scores/DefaultName", m_username->text());
	settings.beginWriteArray("Scores_Gottet");
	for (int r = 0, size = m_scores.size(); r < size; ++r) {
		const Score& score = m_scores[r];
		settings.setArrayIndex(r);
		settings.setValue("Name", score.name);
		settings.setValue("Level", score.level);
		settings.setValue("Lines", score.lines);
		settings.setValue("Score", score.score);
	}
	settings.endArray();
}

//-----------------------------------------------------------------------------

bool ScoresDialog::addScore(const QString& name, int level, int lines, int score)
{
	m_row = -1;
	if (score == 0) {
		return false;
	}

	m_row = 0;
	for (const Score& s : qAsConst(m_scores)) {
		if (score >= s.score) {
			break;
		}
		++m_row;
	}
	if (m_row == 10) {
		m_row = -1;
		return false;
	}

	Score s = { name, level, lines, score };
	m_scores.insert(m_row, s);
	if (m_scores.size() == 11) {
		m_scores.removeLast();
	}

	m_max = m_scores.first().score;
	m_min = (m_scores.size() == 10) ? m_scores.last().score : 1;

	return true;
}

//-----------------------------------------------------------------------------

void ScoresDialog::load(QSettings& settings)
{
	const int size = std::min(settings.beginReadArray("Scores_Gottet"), 10);
	for (int r = 0; r < size; ++r) {
		settings.setArrayIndex(r);
		const QString name = settings.value("Name", m_default_name).toString();
		const int level = settings.value("Level").toInt();
		const int lines = settings.value("Lines").toInt();
		const int score = settings.value("Score").toInt();
		addScore(name, level, lines, score);
	}
	settings.endArray();

	m_row = -1;
	updateItems();
}

//-----------------------------------------------------------------------------

void ScoresDialog::updateItems()
{
	const int size = m_scores.size();

	// Add scores
	for (int r = 0; r < size; ++r) {
		const Score& score = m_scores[r];
		m_score_labels[r][NameColumn]->setText(score.name);
		m_score_labels[r][LevelColumn]->setNum(score.level);
		m_score_labels[r][LinesColumn]->setNum(score.lines);
		m_score_labels[r][ScoreColumn]->setText(QString("%L1").arg(score.score));
	}

	// Fill remainder of scores with dashes
	for (int r = size; r < 10; ++r) {
		for (int c = RankColumn + 1; c < TotalColumns; ++c) {
			m_score_labels[r][c]->setText("-");
		}
	}

	// Use bold text for new score
	if (m_row != -1) {
		for (int c = 0; c < TotalColumns; ++c) {
			m_score_labels[m_row][c]->setText("<b>" + m_score_labels[m_row][c]->text() + "</b>");
		}
	}
}

//-----------------------------------------------------------------------------
