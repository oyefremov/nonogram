// SolverDoc.cpp : implementation of the CSolverDoc class
//

#include "stdafx.h"
#include "ui.h"

#include "SolverDoc.h"
#include <algorithm>
#include <sstream>
#include <fstream>

#include <socket\socket.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSolverDoc

IMPLEMENT_DYNCREATE(CSolverDoc, CDocument)

BEGIN_MESSAGE_MAP(CSolverDoc, CDocument)
	ON_COMMAND(ID_MULTIPLAYER_CONNECT, &CSolverDoc::OnMultiplayerConnect)
END_MESSAGE_MAP()


// CSolverDoc construction/destruction

CSolverDoc::CSolverDoc()
{
	// TODO: add one-time construction code here

}

CSolverDoc::~CSolverDoc()
{
}

BOOL CSolverDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	generate_new(20, 65, 124);
//	newField(20, 20);

	return TRUE;
}

void CSolverDoc::newField(int rows, int cols)
{
	// clear, because SDI will reuse same document 
	m_rows_blocks.clear();
	m_cols_blocks.clear();
	m_rows_blocks_flags.clear();
	m_cols_blocks_flags.clear();

	m_field = m_gess = std::string(rows * cols, unknown_cell);

	m_rows = rows;
	m_cols = cols;

	m_rows_blocks.resize(rows);
	m_cols_blocks.resize(cols);
	m_rows_blocks_flags.resize(rows);
	m_cols_blocks_flags.resize(cols);

	m_max_col_blocks = maxBlocks(m_cols_blocks);
	m_max_row_blocks = maxBlocks(m_rows_blocks);
}


bool compare_by_size(const std::vector<size_t>& a, const std::vector<size_t>& b)
{
	return a.size() < b.size();
}

size_t CSolverDoc::maxBlocks(const std::vector< std::vector<size_t> >& blocks)
{
	return std::max_element(blocks.begin(), blocks.end(), compare_by_size) -> size();
}

std::vector<size_t> parse(const std::string& s) 
{
	std::stringstream line(s);
	std::vector<size_t> res;
	size_t i;
	while (line >> i)
		res.push_back(i);
	if (res.size() == 1 && res[0] == 0) 
		res.clear();
	return res;
}

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


// CSolverDoc diagnostics

#ifdef _DEBUG
void CSolverDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSolverDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSolverDoc commands

BOOL CSolverDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	std::ifstream file(lpszPathName);

	if (!file)
		return FALSE;

	std::string line;
	std::vector< std::vector<size_t> > nums;
	while (std::getline(file, line))
	{
		if (line == "// board") break;
		if (line.empty() || line.size() >= 2 &&
			line[0] == '/' && line[1] == '/') continue;
		
		nums.push_back(parse(line));
	}

	if (!init_with_nums(nums)) return false;

	for (size_t i=0; i<rows(); ++i)
	{
		if (!std::getline(file, line)) return false;
		m_field += line;
	}

	if (std::getline(file, line) && line == "// solution")
	{
		std::string solution;
		for (size_t i=0; i<rows(); ++i)
		{
			if (!std::getline(file, line)) return false;
			solution += line;
		}
		m_nonogram.setRealField(std::move(solution), m_cols, m_rows);
		m_nonogram.gess(m_field);
	}
	return TRUE;
}

bool CSolverDoc::init_with_nums(const std::vector< std::vector<size_t> >& nums)
{
	int all = std::for_each(nums.begin(), nums.end(), sum());
	if (all <= 0) return false;
	int half = all / 2;
	if (half * 2 != all) return false;
	int s = 0;
	int i;
	for (i = 0; s < half; ++i)
		s += std::for_each(nums[i].begin(), nums[i].end(), sum());
	if (s != half) return false;
	newField(i, nums.size() - i);

	m_cols_blocks.assign(nums.begin(), nums.begin() + i);
	m_rows_blocks.assign(nums.begin() + i, nums.end());

	for (size_t i = 0; i < m_cols_blocks.size(); ++i)
		m_cols_blocks_flags[i].resize(m_cols_blocks[i].size(), false);
	for (size_t i = 0; i < m_rows_blocks.size(); ++i)
		m_rows_blocks_flags[i].resize(m_rows_blocks[i].size(), false);

	m_max_col_blocks = maxBlocks(m_cols_blocks);
	m_max_row_blocks = maxBlocks(m_rows_blocks);

	return true;
}


void CSolverDoc::saveBlocks(std::ostream& file, const std::vector< std::vector<size_t> >& blocks)
{
	for (size_t i=0; i<blocks.size(); ++i)
	{
		const std::vector<size_t>& block = blocks[i];
		if (block.size() == 0)
			file << 0;
		for (size_t j=0; j<block.size(); ++j)
		{
			if (j) file << " ";
			file << block[j];
		}
		file << std::endl;
	}
}


BOOL CSolverDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	std::ofstream file(lpszPathName);

	// numbers
	saveBlocks(file, m_cols_blocks);
	saveBlocks(file, m_rows_blocks);

	// board
	file << "// board" << std::endl;
	file << m_field << std::endl;

	// solution
	file << "// solution" << std::endl;
	file << m_nonogram.getRealField() << std::endl;

	return TRUE;
}

bool CSolverDoc::ensure()
{
	return m_nonogram.check(m_gess);
}

void CSolverDoc::commit()
{
	m_nonogram.gess(m_gess);
	m_field = m_nonogram.getOpenField();
}

void CSolverDoc::rollback()
{
	std::fill(m_gess.begin(), m_gess.end(), unknown_cell);
}

class column{
	const std::vector<std::string>& board;
	int col;
public:
	column(const std::vector<std::string>& _board, int _col)
		: board(_board), col(_col) {}
	size_t size() const {return board.size();}
	char operator[] (size_t i) const {return board[i][col];}
};

template <class T>
std::vector<size_t> encode_line(const T& line)
{
	std::vector<size_t> res;

	int block = 0;
	for (size_t i=0; i<line.size(); ++i)
	{
		switch (line[i])
		{
		case '.':
			if (block != 0)
			{
				res.push_back(block);
				block = 0;
			}
			break;
		case '#':
			block += 1;
			break;
		}
	}
	if (block != 0)
		res.push_back(block);

	if (res.empty())
		res.push_back(0);
		
	return res;
}


void CSolverDoc::generate_new(int size, int prob, int seed)
{
	srand(seed);
	m_nonogram.generateRandomField(prob, size, size);
	init_with_nums(m_nonogram.getNums());
}

void CSolverDoc::togle_column_num(CPoint p)
{
	size_t col = p.x;
	int i = m_cols_blocks[col].size() + p.y;
	if (i >= 0)
		m_cols_blocks_flags[col][i] = !m_cols_blocks_flags[col][i];
}
void CSolverDoc::togle_row_num(CPoint p)
{
	size_t row = p.y;
	int i = m_rows_blocks[row].size() + p.x;
	if (i >= 0)
		m_rows_blocks_flags[row][i] = !m_rows_blocks_flags[row][i];
}

template<class T>
void auto_nums(const T& line, const std::vector<size_t>& nums, std::vector<bool>& flags)
{
	if (std::find(flags.begin(), flags.end(), false) == flags.end())
		return;

	// check whole line
	int filled = 0;
	for (size_t i=0; i<line.size(); ++i)
		if (line[i] == filled_cell) ++filled;

	if (filled == 0) return;

	int nums_sum = sum(nums);	
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
			if (nums[left_num] != block)
				return; // error case, must be equal
			else flags[left_num++] = true;
			block = 0;
		}
		else break;
	}

	// check right-to-left
	size_t right_pos = line.size() - 1;
	size_t right_num = nums.size() - 1;
	block = 0;
	for (; right_pos<line.size(); --right_pos)
	{
		if (line[right_pos] == filled_cell) ++block;
		else if (line[right_pos] == empty_cell)
		{
			if (block == 0) continue;
			if (nums[right_num] != block)
				return; // error case, must be equal
			else flags[right_num--] = true;
			block = 0;
		}
		else break;
	}

	if (++left_pos >= --right_pos) return;
	if (right_pos > line.size()) return;

	// check for largest
	std::vector<size_t> mid_nums(nums.begin() + left_num, nums.begin() + right_num + 1);
	std::sort(mid_nums.rbegin(), mid_nums.rend());
	for (int i=0; i<(signed)mid_nums.size() - 1; ++i)
	{
		if (mid_nums[i] == mid_nums[i + 1])
		{
			mid_nums.resize(i);
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

void CSolverDoc::auto_togle_row_num()
{
	// TODO: implement
	//for (size_t i = 0; i < cols(); ++i)
	//	auto_nums(column(m_field, i), m_cols_blocks[i], m_cols_blocks_flags[i]);

	//for (size_t i = 0; i < rows(); ++i)
	//	auto_nums(m_field[i], m_rows_blocks[i], m_rows_blocks_flags[i]);
}



void CSolverDoc::OnMultiplayerConnect()
{
	try
	{
		wsa::WSAInitialize();
		wsa::socket socket = wsa::connect("127.0.0.1", 12345);
		socket << "Sasha";
		std::string message;
		socket >> message;
		AfxMessageBox(message);

		socket << "list players";
		socket >> message;
		AfxMessageBox(message);
	}
	catch (std::exception& e)
	{
		AfxMessageBox(e.what());
	}
}

std::string get_string(const CString& s)
{
	return std::string(s.GetString(), s.GetString() + s.GetLength()); 
}

CString get_CString(const std::string& s)
{
	return std::wstring(s.begin(), s.end()).c_str(); 
}
