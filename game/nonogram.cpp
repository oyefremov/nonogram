#include "nonogram.h"
#include <algorithm>
#include <stdlib.h>

void nonogram::generateRandomField(int probability, int w, int h)
{
	std::string s(w * h, ' ');
	std::generate(s.begin(), s.end(), [=](){
		return rand() % 100 < probability ? filled_char : empty_char;
	});
	// TODO: ensure game is solvable
	// TODO: ensure we have no empty lines and no lines filled by 100%
	// TODO: ensure we don't have too many numbers per line
	setRealField(s, w, h);
}

void nonogram::setRealField(std::string s, int w, int h)
{
	m_width = w;
	m_height = h;
	m_realField = std::move(s);
	m_openField = std::string(w * h, unknown_char);
	m_cols.clear();
	m_cols.resize(w);
	m_rows.clear();
	m_rows.resize(h);
	calculateNumbers();
}

template <class T> 
void calc_nums(T& line)
{
	int black = 0;
	for (size_t i = 0; i < line.size(); ++i)
	{
		if (line[i] == nonogram::filled_char)
			++black;
		else if (black > 0)
			line.nums().push_back(black), black = 0;
	}
	if (black > 0)
		line.nums().push_back(black);
}

void nonogram::calculateNumbers()
{
	for (size_t i = 0; i < m_width; ++i)
		calc_nums(cols(i));
	for (size_t i = 0; i < m_height; ++i)
		calc_nums(rows(i));
}

bool nonogram::gess(const std::string& s)
{
	if (s.size() != m_realField.size()) return false;

	auto res = std::mismatch(s.begin(), s.end(), m_realField.begin(), [](char a, char b){
		return a == unknown_char || (a & char_mask) == b;
	});
	if (res.first != s.end()) return false;

	std::transform(s.begin(), s.end(), m_openField.begin(), m_openField.begin(), [](char a, char b){
		return a != unknown_char && b == unknown_char ? a : b;
	});
	return true;
}
