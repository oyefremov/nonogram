#pragma once

#include <string>
#include <game/nonogram.h>
#include "utils.h"


template<class T>
void commit_line(T& line, const std::string& solution)
{
	std::copy(solution.begin(), solution.end(), line.begin());
	//for (size_t j=0; j<line.size(); ++j)
	//	line[j] = solution[j];
}


template<class T>
bool solve_empty_line(const T& line, std::string& solution)
{
	const int line_size = line.size();
	solution.assign(line.begin(), line.end());

	const int nums_sum = sum(line.nums());	

	if (nums_sum == 0) // handle empty line
	{
		std::fill_n(solution.begin(), line_size, nonogram::empty_char);
		return true;
	}

	int value = nums_sum + line.nums().size() - 1;
	int penalty = line_size - value;

	bool res = false;
	auto pos = solution.begin();
	std::for_each(line.nums().begin(), line.nums().end(), [&](int num)
	{
		if (num > penalty)
		{
			// place current num
			std::fill(pos + penalty, pos + num, nonogram::filled_char);
			res = true;
		}
		pos += num;
		if (pos != solution.end())
		{
			if (penalty == 0)
				*pos = nonogram::empty_char;
			++pos;
		}
	});
	
	return res;
}
