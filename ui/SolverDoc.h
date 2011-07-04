// SolverDoc.h : interface of the CSolverDoc class
//


#pragma once
#include <vector>
#include <string>
#include <game/nonogram.h>

const char unknown_cell = nonogram::unknown_char;
const char filled_cell = nonogram::filled_char;
const char empty_cell = nonogram::empty_char;
//const char gess_filled_cell = '*';
//const char tmp_empty_cell = ',';

class CSolverDoc : public CDocument
{
//	std::vector<std::string> m_field;
//	std::vector<std::string> m_solution;
	std::string m_field;
	std::string m_gess;
	size_t m_cols;
	size_t m_rows;
	std::vector< std::vector<size_t> > m_rows_blocks;
	std::vector< std::vector<size_t> > m_cols_blocks;
	std::vector< std::vector<bool> > m_rows_blocks_flags;
	std::vector< std::vector<bool> > m_cols_blocks_flags;

	size_t m_max_row_blocks, m_max_col_blocks;

	void newField(int rows, int cols);
	static size_t maxBlocks(const std::vector< std::vector<size_t> >& blocks);

	static void saveBlocks(std::ostream& file, const std::vector< std::vector<size_t> >& blocks);

	std::string m_player_name;

	nonogram m_nonogram;
	size_t idx(int x, int y) const {return x + y * m_cols;}

protected: // create from serialization only
	CSolverDoc();
	DECLARE_DYNCREATE(CSolverDoc)

// Attributes
public:
	size_t cols() const {return m_cols;}
	size_t rows() const {return m_rows;}
	
	char cell(size_t x, size_t y) const {return m_field[idx(x, y)];}
	char cell(CPoint pt) const {return cell(pt.x, pt.y);}
	
	char gessCell(size_t x, size_t y) const {return m_gess[idx(x, y)];}
	char gessCell(CPoint pt) const {return gessCell(pt.x, pt.y);}
	
	void setCell(size_t x, size_t y, char value) {m_field[idx(x, y)] = value;}
	void setCell(CPoint pt, char value) {setCell(pt.x, pt.y, value);}
	
	void setGessCell(size_t x, size_t y, char value) {m_gess[idx(x, y)] = value;}
	void setGessCell(CPoint pt, char value) {setGessCell(pt.x, pt.y, value);}
	
	size_t max_col_blocks() const {return m_max_col_blocks;}
	size_t max_row_blocks() const {return m_max_row_blocks;}

	bool isField(CPoint pt) const {
		CRect r(0, 0, cols(), rows());
		return r.PtInRect(pt) == TRUE;
	}

	bool isColumnNums(CPoint pt) const {
		CRect r(0, -(signed)max_row_blocks(), cols(), 0);
		return r.PtInRect(pt) == TRUE;
	}

	bool isRowNums(CPoint pt) const {
		CRect r(-(signed)max_col_blocks(), 0, 0, rows());
		return r.PtInRect(pt) == TRUE;
	}

	void togle_column_num(CPoint p);
	void togle_row_num(CPoint p);
	void auto_togle_row_num();

	const std::vector<size_t>& col_blocks(int i) const {return m_cols_blocks[i];}
	const std::vector<size_t>& row_blocks(int i) const {return m_rows_blocks[i];}

	const std::vector<bool>& col_blocks_flags(int i) const {return m_cols_blocks_flags[i];}
	const std::vector<bool>& row_blocks_flags(int i) const {return m_rows_blocks_flags[i];}
	
	void commit();
	void rollback();
	bool ensure();
	
	bool init_with_nums(const std::vector< std::vector<size_t> >& nums);
	void generate_new(int size, int prob, int seed);

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

// Implementation
public:
	virtual ~CSolverDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMultiplayerConnect();
};

std::string get_string(const CString& s);
CString get_CString(const std::string& s);
inline void AfxMessageBox(const std::string& s) {AfxMessageBox(get_CString(s));}
