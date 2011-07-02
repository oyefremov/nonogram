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
	m_field.clear();

	std::string empty_line(cols, ' ');
	m_field.resize(rows, empty_line);
	m_rows_blocks.resize(rows);
	m_cols_blocks.resize(cols);
	m_rows_blocks_flags.resize(rows);
	m_cols_blocks_flags.resize(cols);

	m_max_col_blocks = maxBlocks(m_cols_blocks);
	m_max_row_blocks = maxBlocks(m_rows_blocks);
}


bool compare_by_size(const std::vector<int>& a, const std::vector<int>& b)
{
	return a.size() < b.size();
}

size_t CSolverDoc::maxBlocks(const std::vector< std::vector<int> >& blocks)
{
	return std::max_element(blocks.begin(), blocks.end(), compare_by_size) -> size();
}

std::vector<int> parse(const wchar_t* s) 
{
	std::wstringstream line(s);
	std::vector<int> res;
	int i;
	while (line >> i)
		res.push_back(i);
	if (res.size() == 1 && res[0] == 0) 
		res.clear();
	return res;
}

class sum{
	int m_sum;
public:
	sum(const std::vector<int>& v) : m_sum(0) {
		(*this)(v);
	}
	sum() : m_sum(0) {}
	operator int() {return m_sum;}
	void operator() (int v) 
	{
		m_sum += v;
	}
	void operator() (const std::vector<int>& v) 
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
	CStdioFile file;

	if (!file.Open(lpszPathName, CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;

	CString line;
	std::vector< std::vector<int> > nums;
	while (file.ReadString(line))
	{
		if (line == "// board") break;
		if (line.IsEmpty() || line.GetLength() >= 2 &&
			line[0] == '/' && line[1] == '/') continue;
		
		nums.push_back(parse(line));
	}

	if (!init_with_nums(nums)) return false;

	for (size_t i=0; i<rows(); ++i)
	{
		if (!file.ReadString(line)) return false;
		std::copy(line.GetString(), line.GetString() + cols(), m_field[i].begin());
	}

	if (file.ReadString(line) && line == "// solution")
	{
		m_solution.clear();
		m_solution.resize(rows(), std::string(cols(), ' '));
		for (size_t i=0; i<rows(); ++i)
		{
			if (!file.ReadString(line)) return false;
			std::copy(line.GetString(), line.GetString() + cols(), m_solution[i].begin());
		}
	}
	return TRUE;
}

bool CSolverDoc::init_with_nums(const std::vector< std::vector<int> >& nums)
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


void CSolverDoc::saveBlocks(std::ostream& file, const std::vector< std::vector<int> >& blocks)
{
	for (size_t i=0; i<blocks.size(); ++i)
	{
		const std::vector<int>& block = blocks[i];
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
	for (size_t i = 0; i < m_field.size(); ++i)
		file << m_field[i] << std::endl;

	// solution
	file << "// solution" << std::endl;
	for (size_t i = 0; i < m_field.size(); ++i)
		file << m_solution[i] << std::endl;

	return TRUE;
}


void CSolverDoc::replace(char oldColor, char newColor)
{
	for (size_t i = 0; i<m_field.size(); ++i)
		std::replace(m_field[i].begin(), m_field[i].end(), oldColor, newColor);
}

bool CSolverDoc::ensure()
{
	if (m_solution.empty()) return true;

	for (size_t y = 0; y<m_field.size(); ++y)
		for (size_t x = 0; x<m_field[0].size(); ++x)
			if (m_field[y][x] == tmp_empty_cell && m_solution[y][x] != empty_cell ||
				m_field[y][x] == tmp_filled_cell && m_solution[y][x] != filled_cell) return false;
	return true;
}

void CSolverDoc::commit()
{
	replace(tmp_empty_cell, empty_cell);	
	replace(tmp_filled_cell, filled_cell);	
}

void CSolverDoc::rollback()
{
	replace(tmp_empty_cell, unknown_cell);	
	replace(tmp_filled_cell, unknown_cell);	
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
std::vector<int> encode_line(const T& line)
{
	std::vector<int> res;

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
	m_solution.clear();
	m_solution.resize(size, std::string(size, empty_cell));
	srand(seed);
	for (int y = 0; y < size; ++y)
		for (int x = 0; x < size; ++x)
			if (rand() % 100 < prob) m_solution[y][x] = filled_cell;

	std::vector< std::vector<int> > nums;
	for (size_t col = 0; col < m_solution[0].size(); ++col)
		nums.push_back( encode_line(column(m_solution, col)) );
	for (size_t row = 0; row < m_solution.size(); ++row)
		nums.push_back( encode_line(m_solution[row]) );

	init_with_nums(nums);
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
void auto_nums(const T& line, const std::vector<int>& nums, std::vector<bool>& flags)
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
	std::vector<int> mid_nums(nums.begin() + left_num, nums.begin() + right_num + 1);
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
		std::vector<int> mid_nums_2;
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
	for (size_t i = 0; i < cols(); ++i)
		auto_nums(column(m_field, i), m_cols_blocks[i], m_cols_blocks_flags[i]);

	for (size_t i = 0; i < rows(); ++i)
		auto_nums(m_field[i], m_rows_blocks[i], m_rows_blocks_flags[i]);
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
