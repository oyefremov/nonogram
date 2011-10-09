#pragma once

#include <vector>
#include <algorithm>

class sum{
	int m_sum;
public:
	sum(const std::vector<size_t>& v) : m_sum(0) {
		(*this)(v);
	}
	sum() : m_sum(0) {}
	operator int() {return m_sum;}
	void operator() (int v) 
	{
		m_sum += v;
	}
	void operator() (const std::vector<size_t>& v) 
	{
		m_sum += std::for_each(v.begin(), v.end(), sum());
	}
};


inline bool compare_by_size(const std::vector<size_t>& a, const std::vector<size_t>& b)
{
	return a.size() < b.size();
}

inline size_t maxBlocks(const std::vector< std::vector<size_t> >& blocks)
{
	return std::max_element(blocks.begin(), blocks.end(), compare_by_size) -> size();
}
