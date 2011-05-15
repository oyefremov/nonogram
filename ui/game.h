#pragma once
#include <vector>

class game_field;
class game_row
{
public:
	class const_iterator
	{
	};
	class iterator : public const_iterator
	{
	};
	game_row(game_field& f, size_t i);
	size_t size() const;
	char operator[](size_t i) const;
	char& operator[](size_t i);
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
};

class game_col
{
	game_field& m_field;
	size_t m_col_index;
public:
	class const_iterator
	{
	};
	class iterator : public const_iterator
	{
	};
	game_col(game_field& f, size_t i) : m_field(f), m_col_index(i) {}
	size_t size() const;
	char operator[](size_t i) const;
	char& operator[](size_t i);
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
};

class game_rows : public std::vector<game_row>
{
public:
	game_rows(){}
	void reset(game_field& f, size_t size);
};

class game_cols : public std::vector<game_col>
{
public:
	game_cols(){}
	void reset(game_field& f, size_t size);
};

class game_field
{
	size_t m_rows_count;
	size_t m_cols_count;
	std::vector<char> m_field;
	std::vector<char> m_solution;

	game_cols m_columns;
	game_rows m_rows;

public:
	game_field() {reset(0, 0);}
	void reset(size_t rows, size_t cols);

	const game_rows& rows() const {return m_rows;}
	game_rows& rows() {return m_rows;}
	const game_cols& columns() const {return m_columns;}
	game_cols& columns() {return m_columns;}
	const std::vector<char>& all_cells() const {return m_field;}	
	std::vector<char>& all_cells() {return m_field;}	

	char cell(size_t col, size_t row) const;
	char& cell(size_t col, size_t row);
};

