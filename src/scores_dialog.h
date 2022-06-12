/*
	SPDX-FileCopyrightText: 2008-2021 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GOTTET_SCORES_DIALOG_H
#define GOTTET_SCORES_DIALOG_H

#include <QDialog>
class QDialogButtonBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QSettings;

/**
 * @brief The ScoresDialog class displays the list of high scores.
 */
class ScoresDialog : public QDialog
{
	Q_OBJECT

	/** The columns of scores. */
	enum Columns
	{
		RankColumn,
		NameColumn,
		LevelColumn,
		LinesColumn,
		ScoreColumn,
		TotalColumns
	};

	/**
	 * @brief The ScoresDialog::Score struct descibres a high score.
	 */
	struct Score
	{
		QString name; /**< the player's name */
		int level; /**< what level the player was on */
		int lines; /**< how many total lines the player cleared */
		int score; /**< the value of the score */
	};

public:
	/**
	 * Constructs a scores dialog.
	 * @param parent the QWidget that manages the dialog
	 */
	explicit ScoresDialog(QWidget* parent = nullptr);

	/**
	 * Attempts to add a score.
	 * @param level what level the player was on
	 * @param lines how many total lines the player cleared
	 * @param score the value of the score
	 * @return whether the score was added
	 */
	bool addScore(int level, int lines, int score);

	/**
	 * Checks if a score is a high score.
	 * @param score the value of the score
	 * @return whether the score is a high score, and if it is the highest
	 */
	static int isHighScore(int score);

	/**
	 * Converts the stored scores to the new format.
	 */
	static void migrate();

protected:
	/**
	 * Override hideEvent to add score if the player has not already pressed enter.
	 * @param event details of the hide event
	 */
	void hideEvent(QHideEvent* event) override;

	/**
	 * Override keyPressEvent to ignore return key. Keeps dialog from closing when the player
	 * presses return after entering their name.
	 * @param event details of the key press event
	 */
	void keyPressEvent(QKeyEvent* event) override;

private Q_SLOTS:
	/**
	 * Enters the score and saves list of scores once the player has finished entering their name.
	 */
	void editingFinished();

private:
	/**
	 * Adds a score to the high score board.
	 * @param name the player's name
	 * @param level what level the player was on
	 * @param lines how many total lines the player cleared
	 * @param score the value of the score
	 * @return @c true if the score was added
	 */
	bool addScore(const QString& name, int level, int lines, int score);

	/**
	 * Loads the scores from the settings.
	 * @param settings where to load the scores from
	 */
	void load(QSettings& settings);

	/**
	 * Sets the text of the high scores. Adds the dashed lines for empty scores.
	 */
	void updateItems();

private:
	QDialogButtonBox* m_buttons; /**< buttons to control dialog */
	QLineEdit* m_username; /**< widget for the player to enter their name */

	QList<Score> m_scores; /**< the high score data */
	QLabel* m_score_labels[10][TotalColumns]; /**< the grid[row][column] of labels to display the scores */
	QGridLayout* m_scores_layout; /**< the layout for the dialog */
	int m_row; /**< location of most recently added score */

	QString m_default_name; /**< the default name */

	static int m_max; /**< the largest high score */
	static int m_min; /**< the smallest high score */
};

#endif // GOTTET_SCORES_DIALOG_H
