/*
	SPDX-FileCopyrightText: 2007-2008 Graeme Gott <graeme@gottcode.org>

	SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GOTTET_PIECE_H
#define GOTTET_PIECE_H

struct Cell
{
	explicit Cell(int x1 = -1, int y1 = -1)
		: x(x1)
		, y(y1)
	{
	}

	int x;
	int y;
};

class Board;

class Piece
{
public:
	Piece(int type, Board* board);

	bool isValid() const
	{
		return m_valid;
	}

	bool moveLeft()
	{
		return move(-1, 0);
	}

	bool moveRight()
	{
		return move(1, 0);
	}

	bool moveDown()
	{
		return move(0, 1);
	}

	bool rotate();
	void drop();

	const Cell* cells() const
	{
		return m_cells;
	}

	int type() const
	{
		return m_type;
	}

	static void cells(Cell* cells, int type);

private:
	bool move(int x, int y);
	bool updatePosition(const Cell* cells);

private:
	int m_type;
	Cell m_cells[4];
	Cell m_pivot;
	bool m_valid;
	Board* m_board;
};

#endif // GOTTET_PIECE_H
