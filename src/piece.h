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

#ifndef PIECE_H
#define PIECE_H

struct Cell
{
	Cell(int x1 = 0, int y1 = 0)
	:	x(x1),
		y(y1)
	{
	}

	bool operator==(const Cell& cell) const
	{
		return x == cell.x && y == cell.y;
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

	static void cells(Cell* cells, int type);

private:
	bool move(int x, int y);
	bool updatePosition(const Cell* cells);

	int m_type;
	Cell m_cells[4];
	Cell m_pivot;
	bool m_valid;
	Board* m_board;
};

#endif // PIECE_H
