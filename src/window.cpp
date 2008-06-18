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

#include "window.h"

#include "board.h"
#include "score_board.h"

#include <ctime>

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QVBoxLayout>

#if defined(QTOPIA_PHONE)
#include <QSoftMenuBar>
#else
#include <QMenuBar>
#endif

/*****************************************************************************/

Window::Window(QWidget *parent, Qt::WindowFlags wf)
:	QMainWindow(parent, wf)
{
	setWindowTitle(tr("Gottet"));
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	setWindowIcon(QIcon(":/gottet.png"));
#endif

	QWidget* contents = new QWidget(this);
	setCentralWidget(contents);

	// Create preview
	m_preview = new QLabel(contents);
#if !defined(QTOPIA_PHONE)
	m_preview->setFixedSize(80, 100);
#else
	m_preview->setFixedSize(30, 40);
#endif
	m_preview->setAutoFillBackground(true);
	{
		QPalette palette = m_preview->palette();
		palette.setColor(m_preview->backgroundRole(), Qt::black);
		m_preview->setPalette(palette);
	}

	// Create level display
	m_level = new QLabel("0", contents);
	m_level->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_level->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_level->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create lines display
	m_lines = new QLabel("0", contents);
	m_lines->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_lines->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_lines->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create score display
	m_score = new QLabel("0", contents);
	m_score->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_score->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_score->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	// Create scoreboard
	m_score_board = new ScoreBoard(this);

	// Create board
	srand(time(0));
	m_board = new Board(contents);
	connect(m_board, SIGNAL(pauseAvailable(bool)), this, SLOT(pauseAvailable(bool)));
	connect(m_board, SIGNAL(nextPieceAvailable(QPixmap)), m_preview, SLOT(setPixmap(QPixmap)));
	connect(m_board, SIGNAL(levelUpdated(int)), m_level, SLOT(setNum(int)));
	connect(m_board, SIGNAL(linesRemovedUpdated(int)), m_lines, SLOT(setNum(int)));
	connect(m_board, SIGNAL(scoreUpdated(int)), this, SLOT(scoreUpdated(int)));
	connect(m_board, SIGNAL(gameOver(int, int, int)), m_score_board, SLOT(addHighScore(int, int, int)));
	connect(m_board, SIGNAL(gameOver(int, int, int)), this, SLOT(gameOver()));

	// Create menus
#if defined(QTOPIA_PHONE)
	QMenu* menu = QSoftMenuBar::menuFor(this);
	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addSeparator();
	menu->addAction(tr("&Scores"), m_score_board, SLOT(show()));
	menu->addSeparator();
	m_resume_action = menu->addAction(tr("&Resume"), m_board, SLOT(resumeGame()));
	m_resume_action->setVisible(false);
	m_pause_action = menu->addAction(tr("&Pause"), m_board, SLOT(pauseGame()));
	m_pause_action->setEnabled(false);
	QAction* action = menu->addAction(tr("&New Game"), m_board, SLOT(newGame()));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(newGame()));
#else
	QMenu* menu = menuBar()->addMenu(tr("&Game"));
	QAction* action = menu->addAction(tr("&New"), m_board, SLOT(newGame()), tr("Ctrl+N"));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(newGame()));
	m_pause_action = menu->addAction(tr("&Pause"), m_board, SLOT(pauseGame()), tr("P"));
	m_pause_action->setEnabled(false);
	m_resume_action = menu->addAction(tr("&Resume"), m_board, SLOT(resumeGame()), tr("P"));
	m_resume_action->setVisible(false);
	menu->addSeparator();
 	menu->addAction(tr("&Scores"), m_score_board, SLOT(show()));
	menu->addSeparator();
	menu->addAction(tr("&Quit"), qApp, SLOT(quit()), tr("Ctrl+Q"));
	menu = menuBar()->addMenu(tr("&Help"));
	menu->addAction(tr("&About"), this, SLOT(about()));
	menu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
#endif

	// Layout window
	QVBoxLayout* sidebar = new QVBoxLayout;
	sidebar->setMargin(0);
	sidebar->setSpacing(0);
	sidebar->addWidget(new QLabel(tr("Next Piece"), contents), 0, Qt::AlignCenter);
	sidebar->addWidget(m_preview, 0, Qt::AlignCenter);
	sidebar->addSpacing(24);
	sidebar->addWidget(new QLabel(tr("Level"), contents), 0, Qt::AlignCenter);
	sidebar->addWidget(m_level);
	sidebar->addSpacing(24);
	sidebar->addWidget(new QLabel(tr("Removed Lines"), contents), 0, Qt::AlignCenter);
	sidebar->addWidget(m_lines);
	sidebar->addSpacing(24);
	sidebar->addWidget(new QLabel(tr("Score"), contents), 0, Qt::AlignCenter);
	sidebar->addWidget(m_score);
	sidebar->addStretch();

	QHBoxLayout* layout = new QHBoxLayout(contents);
	layout->setMargin(12);
	layout->setSpacing(12);
	layout->addWidget(m_board, 1);
	layout->addLayout(sidebar);

	// Restore window
	restoreGeometry(QSettings().value("Geometry").toByteArray());
}

/*****************************************************************************/

void Window::closeEvent(QCloseEvent* event)
{
	QSettings().setValue("Geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}

/*****************************************************************************/

void Window::pauseAvailable(bool available)
{
	static QPixmap preview;

	if (available) {
		m_pause_action->setVisible(true);
		m_resume_action->setVisible(false);
		m_preview->setPixmap(preview);
	} else {
		m_resume_action->setVisible(true);
		m_pause_action->setVisible(false);
		preview = *m_preview->pixmap();
		m_preview->clear();
	}
}

/*****************************************************************************/

void Window::scoreUpdated(int score)
{
	m_score->setText(QString("%L1").arg(score));

	int position = m_score_board->highScorePosition(score);
	QPalette palette = m_score->palette();
	if (position == 0) {
		palette.setColor(m_preview->foregroundRole(), Qt::red);
	} else if (position < 10) {
		palette.setColor(m_preview->foregroundRole(), Qt::blue);
	}
	m_score->setPalette(palette);
}

/*****************************************************************************/

void Window::newGame()
{
	m_pause_action->setEnabled(true);

	QPalette palette = m_score->palette();
	palette.setColor(m_preview->foregroundRole(), Qt::black);
	m_score->setPalette(palette);
}

/*****************************************************************************/

void Window::gameOver()
{
	m_pause_action->setEnabled(false);
}

/*****************************************************************************/

void Window::about()
{
	QMessageBox::about(this, tr("About Gottet"), tr("<center><big><b>Gottet 1.0.3</b></big><br/>A tetris clone I made for my wife<br/><small>Copyright &copy; 2008 Graeme Gott</small></center>"));
}

/*****************************************************************************/
