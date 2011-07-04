#pragma once
#pragma comment(lib, "game.lib")

#include <string>
#include <vector>

class nonogram_col;
class nonogram_row;

class nonogram
{
	class row
	{
		nonogram& n;
		size_t r;
	public:
		row(nonogram& _n, size_t _i) : n(_n), r(_i) {}
		size_t size() const {return n.m_width;}
		char operator[](size_t c) const {return n.m_realField[n.getIndex(c, r)];}
		char& operator[](size_t c) {return n.m_realField[n.getIndex(c, r)];}
		const std::vector<size_t>& nums() const {return n.m_rows[r];}
		std::vector<size_t>& nums() {return n.m_rows[r];}
	};

	class col
	{
		nonogram& n;
		size_t c;
	public:
		col(nonogram& _n, size_t _i) : n(_n), c(_i) {}
		size_t size() const {return n.m_height;}
		char operator[](size_t r) const {return n.m_realField[n.getIndex(c, r)];}
		char& operator[](size_t r) {return n.m_realField[n.getIndex(c, r)];}
		const std::vector<size_t>& nums() const {return n.m_cols[c];}
		std::vector<size_t>& nums() {return n.m_cols[c];}
	};

	std::string m_realField;
	std::string m_openField;
	std::vector<std::vector<size_t>> m_cols;
	std::vector<std::vector<size_t>> m_rows;
	size_t m_width;
	size_t m_height;

	void calculateNumbers();
	size_t getIndex(int col, int row) const {return col + row * m_width;}

public:
	static const char filled_char = 'A' - 1; // @
	static const char empty_char = 'a' - 1;  // `
	static const char char_mask = 'a' - 1;  // `
	static const char unknown_char = ' ';

	void setRealField(std::string s, int w, int h);
	void generateRandomField(int probability, int w, int h);

	col cols(size_t i) {return col(*this, i);}
	row rows(size_t i) {return row(*this, i);}

	bool check(const std::string& s) const;
	bool gess(const std::string& s);

	const std::string& getRealField() const {return m_realField;}	
	const std::string& getOpenField() const {return m_openField;}	

	std::vector<std::vector<size_t>> getNums() const;
};
