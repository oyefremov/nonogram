
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "nclient.h"
#include "ChildView.h"

#include <algorithm>

#include "autonum.h"
#include "solveemptyline.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
: m_drawing(false)
, m_color(empty_cell)
, m_cols(0)
, m_rows(0)
, m_max_row_blocks(0)
, m_max_col_blocks(0)
{
	generate_new(20, 60, 4);
	solve_empty_lines();
	m_field = m_nonogram.getOpenField();
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	int shift_x = max_row_blocks() * cell_size;
	int shift_y = max_col_blocks() * cell_size;

	dc.SetViewportOrg(shift_x, shift_y);

	// grid
	drawGrid(&dc, cell_size, 1, RGB(200, 200, 200), max_row_blocks(), max_col_blocks());
	drawGrid(&dc, cell_size, 5, RGB(0, 0, 0));

	// numbers
	drawNumbers(&dc);

	// field
	for (size_t y = 0; y < rows(); ++y)
		for (size_t x = 0; x < cols(); ++x)
			drawCell(&dc, cell_size, x, y);
}

void CChildView::drawGrid(CDC* pDC, int size, int step, COLORREF color, int extraX, int extraY)
{
	// grid
	CPen pen(PS_SOLID, 1, color);
	CPen *pNormalPen = pDC->SelectObject(&pen);

	for (int x = -extraX; x <= (int)cols(); x += step)
	{
		int ex = (x >=0 && x <= (int)cols()) ? extraY : 0; 
		pDC->MoveTo(x * size, -ex * size);
		pDC->LineTo(x * size, rows() * size);
	}
	for (int y = -extraY; y <= (int)cols(); y += step)
	{
		int ex = (y >=0 && y <= (int)rows()) ? extraX : 0; 
		pDC->MoveTo(-ex * size, y * size);
		pDC->LineTo(cols() * size, y * size);
	}
	pDC->SelectObject(pNormalPen);

}

void CChildView::drawCell(CDC* pDC, int size, int x, int y)
{
	char c = cell(x, y);
	char g = gessCell(x, y);

	bool dot = false;
	COLORREF bgColor, dotColor;

	switch (c)
	{
	case empty_cell:
		bgColor = RGB(255, 255, 255);
		dot = true;
		dotColor = RGB(0, 0, 255);
		break;
	case filled_cell:
		bgColor = RGB(128, 128, 128);
		break;
	case unknown_cell:
		switch (g)
		{
		case unknown_cell:
			bgColor = RGB(255, 255, 255);
			break;
		case empty_cell:
			bgColor = RGB(230, 230, 255);
			dot = true;
			dotColor = RGB(0, 0, 255);
			break;
		case filled_cell:
			bgColor = RGB(128, 128, 200);
			break;
		}
		break;
	}
	pDC->FillSolidRect(x * size + 1, y * size + 1, size - 1, size - 1, bgColor);
	if (dot)
		pDC->FillSolidRect(x * size + size / 2 - 1, y * size + size / 2 - 1, 3, 3, dotColor);
}

void CChildView::drawNumbers(CDC* pDC)
{
	CString s;
	int shift_x = 5;
	int shift_y = 3;
	COLORREF normal = RGB(0, 0, 0);
	COLORREF flagged = RGB(255, 220, 220);
	for (size_t col = 0; col < cols(); ++col)
	{
		const std::vector<size_t>& blocks = col_blocks(col);
		const std::vector<bool>& flags = col_blocks_flags(col);
		int x_pos = col * cell_size + shift_x;
		for (int i=0; i<(int)blocks.size(); ++i)
		{
			s.Format(L"%d", blocks[i]);
			pDC->SetTextColor(flags[i] ? flagged : normal);
			pDC->TextOut(x_pos, (i - (int)blocks.size()) * cell_size + shift_y, s); 
		}
	}
	for (size_t row = 0; row < rows(); ++row)
	{
		const std::vector<size_t>& blocks = row_blocks(row);
		const std::vector<bool>& flags = row_blocks_flags(row);
		int y_pos = row * cell_size + shift_y;
		for (int i=0; i<(int)blocks.size(); ++i)
		{
			s.Format(L"%d", blocks[i]);
			pDC->SetTextColor(flags[i] ? flagged : normal);
			pDC->TextOut((i - (int)blocks.size()) * cell_size + shift_x, y_pos, s); 
		}
	}
}


void CChildView::generate_new(int size, int prob, int seed)
{
	srand(seed);
	m_nonogram.generateRandomField(prob, size, size);
	init_with_nums(m_nonogram.getNums());
}



bool CChildView::init_with_nums(const std::vector< std::vector<size_t> >& nums)
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



void CChildView::newField(int rows, int cols)
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


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	try_color(filled_cell, point);

	try_nums(point);

	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_drawing = false;

	CWnd::OnLButtonUp(nFlags, point);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	try_color(empty_cell, point);

	CWnd::OnRButtonDown(nFlags, point);
}


void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_drawing = false;

	CWnd::OnRButtonUp(nFlags, point);
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_drawing)
	{
		toCellPoint(point);
		if (isField(point))
		{
			char openCell = cell(point);
			char gess = gessCell(point);
			if (openCell == unknown_cell && gess != m_color)
			{
				setGessCell(point, m_color);
				Invalidate(false);
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_SPACE:
		if (ensure())
		{
			commit();
			auto_togle_row_num();
			Invalidate(false);
		}
		else AfxMessageBox(L"Invalid gess");
		break;
	case VK_BACK:
		rollback();	
		Invalidate(false);
		break;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
// CSolverView message handlers
void CChildView::toCellPoint(CPoint& pt) const
{
	pt.x = pt.x / cell_size - max_row_blocks();
	pt.y = pt.y / cell_size - max_col_blocks();
}

void CChildView::try_nums(CPoint point)
{
	toCellPoint(point);
	if (isColumnNums(point))
	{
		togle_column_num(point);
		Invalidate(false);
	}
	else if (isRowNums(point))
	{
		togle_row_num(point);
		Invalidate(false);
	}
}

void CChildView::try_color(char newColor, CPoint point)
{
	if (m_drawing)
	{
		rollback();
		m_drawing = false;
		ReleaseCapture();
		Invalidate(false);
		return;
	}

	toCellPoint(point);
	if (isField(point) && !m_drawing)
	{
		if (cell(point) != unknown_cell) return;
		char gess_color = gessCell(point);

		if (gess_color != unknown_cell)
			newColor = unknown_cell;

		m_color = newColor;
		m_drawing = true;
		SetCapture();
		setGessCell(point, m_color);
		Invalidate(false);
	}
}
void CChildView::rollback()
{
	std::fill(m_gess.begin(), m_gess.end(), unknown_cell);
}

void CChildView::auto_togle_row_num()
{
	for (size_t i = 0; i < cols(); ++i)
		auto_nums(m_nonogram.open_cols(i), m_cols_blocks_flags[i]);

	for (size_t i = 0; i < rows(); ++i)
		auto_nums(m_nonogram.open_rows(i), m_rows_blocks_flags[i]);
}

void CChildView::solve_empty_lines()
{
	std::string solution;
	for (size_t i = 0; i < cols(); ++i)
	{
		auto line = m_nonogram.open_cols(i);
		if (solve_empty_line(line, solution))
			commit_line(line, solution);
	}

	for (size_t i = 0; i < rows(); ++i)
	{
		auto line = m_nonogram.open_rows(i);
		if (solve_empty_line(line, solution))
			commit_line(line, solution);
	}
}


void CChildView::commit()
{
	m_nonogram.gess(m_gess);
	m_field = m_nonogram.getOpenField();
}
bool CChildView::ensure()
{
	return m_nonogram.check(m_gess);
}

void CChildView::togle_column_num(CPoint p)
{
	size_t col = p.x;
	int i = m_cols_blocks[col].size() + p.y;
	if (i >= 0)
		m_cols_blocks_flags[col][i] = !m_cols_blocks_flags[col][i];
}
void CChildView::togle_row_num(CPoint p)
{
	size_t row = p.y;
	int i = m_rows_blocks[row].size() + p.x;
	if (i >= 0)
		m_rows_blocks_flags[row][i] = !m_rows_blocks_flags[row][i];
}
