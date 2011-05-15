#include "stdafx.h"
#include "game.h"
#if 0
char game_field::cell(size_t col, size_t row) const
{
	return m_field[row * m_cols_count + col];
}

char& game_field::cell(size_t col, size_t row)
{
	return m_field[row * m_cols_count + col];
}

void game_cols::reset(game_field& f, size_t size)
{
	clear();
	reserve(size);
	for (size_t i = 0; i < size; ++i)
		push_back(game_col(f, i));
}

void game_rows::reset(game_field& f, size_t size)
{
	clear();
	reserve(size);
	for (size_t i = 0; i < size; ++i)
		push_back(game_row(f, i));
}

size_t game_col::size() const
{
	return m_field.rows().size();
}

char game_col::operator[](size_t i) const
{
	return m_field.cell(m_col_index, i);
}

char& game_col::operator[](size_t i)
{
	return m_field.cell(m_col_index, i);
}

#endif