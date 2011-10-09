#pragma once

#include <vector>

template<class T>
void auto_nums(const T& line, std::vector<bool>& flags)
{
	if (std::find(flags.begin(), flags.end(), false) == flags.end())
		return;

	// check whole line
	int filled = 0;
	for (size_t i=0; i<line.size(); ++i)
		if (line[i] == filled_cell) ++filled;

	if (filled == 0) return;

	int nums_sum = sum(line.nums());	
	if (filled == nums_sum)
	{
		std::fill(flags.begin(), flags.end(), true);
		return;
	}

	// check left-to-right
	size_t left_pos = 0;
	size_t left_num = 0;
	size_t block = 0;
	for (; left_pos<line.size(); ++left_pos)
	{
		if (line[left_pos] == filled_cell) ++block;
		else if (line[left_pos] == empty_cell)
		{
			if (block == 0) continue;
			if (line.nums()[left_num] != block)
				return; // error case, must be equal
			else flags[left_num++] = true;
			block = 0;
		}
		else break;
	}
	if (left_pos >= line.size()) return; // an error

	// check right-to-left
	size_t right_pos = line.size() - 1;
	size_t right_num = line.nums().size() - 1;
	block = 0;
	for (; right_pos<line.size(); --right_pos)
	{
		if (line[right_pos] == filled_cell) ++block;
		else if (line[right_pos] == empty_cell)
		{
			if (block == 0) continue;
			if (line.nums()[right_num] != block)
				return; // error case, must be equal
			else flags[right_num--] = true;
			block = 0;
		}
		else break;
	}

	if (++left_pos >= --right_pos) return;
	if (right_pos > line.size()) return;

	// check for largest
	std::vector<size_t> mid_nums(line.nums().begin() + left_num, line.nums().begin() + right_num + 1);
	std::sort(mid_nums.rbegin(), mid_nums.rend());
	for (size_t i=1; i<mid_nums.size(); ++i)
	{
		if (mid_nums[i - 1] == mid_nums[i])
		{
			mid_nums.resize(i - 1);
			break;
		}
	}
	if (mid_nums.size() > 0)
	{
		size_t block = 0;
		bool empty = false;
		std::vector<size_t> mid_nums_2;
		for (size_t i = left_pos; i < right_pos; ++i)
		{
			if (line[i] == filled_cell) ++block;
			else if (line[i] == empty_cell) 
			{
				if (empty && block > 0)
					mid_nums_2.push_back(block);
				block = 0;
				empty = true;
			}
			else  
			{
				block = 0;
				empty = false;
			}
		}
		std::sort(mid_nums_2.rbegin(), mid_nums_2.rend());
	}
}
