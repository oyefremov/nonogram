
// ChildView.h : interface of the CChildView class
//

#include <string>
#include <vector>
#include <ostream>
#include <game/nonogram.h>

#pragma once

const char unknown_cell = nonogram::unknown_char;
const char filled_cell = nonogram::filled_char;
const char empty_cell = nonogram::empty_char;

// CChildView window

class CChildView : public CWnd
{
	private:
	void drawGrid(CDC* pDC, int size, int step, COLORREF color, int extraX = 0, int extraY = 0);
	void drawCell(CDC* pDC, int size, int x, int y);
	void drawNumbers(CDC* pDC);

	void toCellPoint(CPoint& pt) const;

	bool m_drawing;
	char m_color;
	static const int cell_size = 20;


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

	static void saveBlocks(std::ostream& file, const std::vector< std::vector<size_t> >& blocks);

	std::string m_player_name;

	nonogram m_nonogram;
	size_t idx(int x, int y) const {return x + y * m_cols;}

private:
	void try_color(char newColor, CPoint point);
	void try_nums(CPoint point);

// Construction
public:
	CChildView();

// Attributes
public:

// Operations
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

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

