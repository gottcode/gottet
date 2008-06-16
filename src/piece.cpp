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

#include "piece.h"

#include "board.h"

/*****************************************************************************/

static const Cell types[][4] = {
	{ Cell(0,0), Cell(0,1), Cell(0,2), Cell(0,3) },
	{ Cell(0,0), Cell(0,1), Cell(1,1), Cell(1,2) },
	{ Cell(1,0), Cell(0,1), Cell(1,1), Cell(0,2) },
	{ Cell(0,0), Cell(0,1), Cell(1,1), Cell(0,2) },
	{ Cell(0,0), Cell(1,0), Cell(1,1), Cell(1,2) },
	{ Cell(0,0), Cell(1,0), Cell(0,1), Cell(0,2) },
	{ Cell(0,0), Cell(1,0), Cell(0,1), Cell(1,1) }
};

/*****************************************************************************/

Piece::Piece(int type, Board* board)
:	m_type(type),
	m_pivot(4,1),
	m_valid(false),
	m_board(board)
{
	Q_ASSERT(type > 0 && type < 8);

	Cell position[4];
	cells(position, type);
	for (int i = 0; i < 4; ++i)
		position[i].x += 4;

	if (updatePosition(position))
		m_valid = true;
}

/*****************************************************************************/

bool Piece::rotate()
{
	Cell rotated[4];
	for (int i = 0; i < 4; ++i) {
		int x = static_cast<int>(m_cells[i].x) - m_pivot.x;
		int y = static_cast<int>(m_cells[i].y) - m_pivot.y;
		rotated[i].x = y + m_pivot.x;
		rotated[i].y = -x + m_pivot.y;
		if (rotated[i].x > 9 || rotated[i].x < 0 || rotated[i].y > 19 || rotated[i].y < 0)
			return false;
	}

	return updatePosition(rotated);
}

/*****************************************************************************/

void Piece::drop()
{
	for (int i = 0; i < 20; ++i)
		moveDown();
}

/*****************************************************************************/

void Piece::cells(Cell* cells, int type)
{
	Q_ASSERT(cells != 0);
	Q_ASSERT(type > 0 && type < 8);

	const Cell* values = types[type - 1];
	for (int i = 0; i < 4; ++i)
		cells[i] = values[i];
}

/*****************************************************************************/

bool Piece::move(int x, int y)
{
	// Move cells
	Cell moved[4];
	for (int i = 0; i < 4; ++i) {
		moved[i].x = m_cells[i].x + x;
		moved[i].y = m_cells[i].y + y;
		if (moved[i].x > 9 || moved[i].x < 0 || moved[i].y > 19 || moved[i].y < 0)
			return false;
	}

	bool success = updatePosition(moved);
	if (success) {
		m_pivot.x += x;
		m_pivot.y += y;
	}
	return success;
}

/*****************************************************************************/

bool Piece::updatePosition(const Cell* cells)
{
	// Check for collision of cells
	const Cell* cell = 0;
	bool solid = false;
	for (int i = 0; i < 4; ++i) {
		cell = &cells[i];
		solid = m_board->cell(cell->x, cell->y);
		if (solid) {
			for (int j = 0; j < 4; ++j) {
				if (*cell == m_cells[j])
					solid = false;
			}
		}
		if (solid)
			return false;
	}

	// Move cells
	for (int i = 0; i < 4; ++i)
		m_board->removeCell(m_cells[i].x, m_cells[i].y);
	for (int i = 0; i < 4; ++i)
		m_board->addCell(cells[i].x, cells[i].y, m_type);
	for (int i = 0; i < 4; ++i)
		m_cells[i] = cells[i];

	return true;
}

/*****************************************************************************/
