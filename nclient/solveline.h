#pragma once

#include <string>
#include <game/nonogram.h>
#include "utils.h"

template<class T>
bool solve_line(const T& line, std::string& solution)
{
	const int line_size = line.size();
	solution.assign(line.begin(), line.end());

	return false;
}
