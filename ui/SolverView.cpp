// SolverView.cpp : implementation of the CSolverView class
//

#include "stdafx.h"
#include "ui.h"

#include "SolverDoc.h"
#include "SolverView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSolverView

IMPLEMENT_DYNCREATE(CSolverView, CView)

BEGIN_MESSAGE_MAP(CSolverView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CSolverView construction/destruction

CSolverView::CSolverView()
: m_drawing(false)
{
}

CSolverView::~CSolverView()
{
}

BOOL CSolverView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSolverView drawing

void CSolverView::drawGrid(CDC* pDC, int size, int step, COLORREF color, int extraX, int extraY)
{
	CSolverDoc* pDoc = GetDocument();

	// grid
	CPen pen(PS_SOLID, 1, color);
	CPen *pNormalPen = pDC->SelectObject(&pen);

	for (int x = -extraX; x <= (int)pDoc->cols(); x += step)
	{
		int ex = (x >=0 && x <= (int)pDoc->cols()) ? extraY : 0; 
		pDC->MoveTo(x * size, -ex * size);
		pDC->LineTo(x * size, (pDoc->rows()) * size);
	}
	for (int y = -extraY; y <= (int)pDoc->cols(); y += step)
	{
		int ex = (y >=0 && y <= (int)pDoc->rows()) ? extraX : 0; 
		pDC->MoveTo(-ex * size, y * size);
		pDC->LineTo((pDoc->cols()) * size, y * size);
	}
	pDC->SelectObject(pNormalPen);

}

void CSolverView::drawCell(CDC* pDC, int size, int x, int y)
{
	CSolverDoc* pDoc = GetDocument();

	char c = pDoc->cell(x, y);
	char g = pDoc->gessCell(x, y);

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
			//		pDC->FillSolidRect(x * size + 1, y * size + 1, size - 1, size - 1, RGB(255, 255, 255));
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

void CSolverView::drawNumbers(CDC* pDC)
{
	CString s;
	int shift_x = 5;
	int shift_y = 3;
	COLORREF normal = RGB(0, 0, 0);
	COLORREF flagged = RGB(255, 220, 220);
	for (size_t col = 0; col < GetDocument()->cols(); ++col)
	{
		const std::vector<size_t>& blocks = GetDocument()->col_blocks(col);
		const std::vector<bool>& flags = GetDocument()->col_blocks_flags(col);
		int x_pos = col * cell_size + shift_x;
		for (int i=0; i<(int)blocks.size(); ++i)
		{
			s.Format(L"%d", blocks[i]);
			pDC->SetTextColor(flags[i] ? flagged : normal);
			pDC->TextOut(x_pos, (i - (int)blocks.size()) * cell_size + shift_y, s); 
		}
	}
	for (size_t row = 0; row < GetDocument()->rows(); ++row)
	{
		const std::vector<size_t>& blocks = GetDocument()->row_blocks(row);
		const std::vector<bool>& flags = GetDocument()->row_blocks_flags(row);
		int y_pos = row * cell_size + shift_y;
		for (int i=0; i<(int)blocks.size(); ++i)
		{
			s.Format(L"%d", blocks[i]);
			pDC->SetTextColor(flags[i] ? flagged : normal);
			pDC->TextOut((i - (int)blocks.size()) * cell_size + shift_x, y_pos, s); 
		}
	}
}

void CSolverView::OnDraw(CDC* pDC)
{
	CSolverDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	int shift_x = pDoc->max_row_blocks() * cell_size;
	int shift_y = pDoc->max_col_blocks() * cell_size;

	pDC->SetViewportOrg(shift_x, shift_y);

	// grid
	drawGrid(pDC, cell_size, 1, RGB(200, 200, 200), pDoc->max_row_blocks(), pDoc->max_col_blocks());
	drawGrid(pDC, cell_size, 5, RGB(0, 0, 0));

	// numbers
	drawNumbers(pDC);

	// field
	for (size_t y = 0; y < pDoc->rows(); ++y)
		for (size_t x = 0; x < pDoc->cols(); ++x)
			drawCell(pDC, cell_size, x, y);
}


// CSolverView printing

BOOL CSolverView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSolverView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSolverView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CSolverView diagnostics

#ifdef _DEBUG
void CSolverView::AssertValid() const
{
	CView::AssertValid();
}

void CSolverView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSolverDoc* CSolverView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSolverDoc)));
	return (CSolverDoc*)m_pDocument;
}
#endif //_DEBUG


// CSolverView message handlers
void CSolverView::toCellPoint(CPoint& pt) const
{
	pt.x = pt.x / cell_size - GetDocument()->max_row_blocks();
	pt.y = pt.y / cell_size - GetDocument()->max_col_blocks();
}

void CSolverView::try_nums(CPoint point)
{
	toCellPoint(point);
	if (GetDocument()->isColumnNums(point))
	{
		GetDocument()->togle_column_num(point);
		Invalidate(false);
	}
	else if (GetDocument()->isRowNums(point))
	{
		GetDocument()->togle_row_num(point);
		Invalidate(false);
	}
}

void CSolverView::try_color(char newColor, CPoint point)
{
	if (m_drawing)
	{
		GetDocument()->rollback();
		m_drawing = false;
		ReleaseCapture();
		Invalidate(false);
		return;
	}

	toCellPoint(point);
	if (GetDocument()->isField(point) && !m_drawing)
	{
		if (GetDocument()->cell(point) != unknown_cell) return;
		char gess_color = GetDocument()->gessCell(point);

		if (gess_color != unknown_cell)
			newColor = unknown_cell;

		m_color = newColor;
		m_drawing = true;
		SetCapture();
		GetDocument()->setGessCell(point, m_color);
		Invalidate(false);
	}
}


void CSolverView::OnLButtonDown(UINT nFlags, CPoint point)
{
	try_color(filled_cell, point);

	try_nums(point);

	CView::OnLButtonDown(nFlags, point);
}

void CSolverView::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_drawing = false;

	CView::OnLButtonUp(nFlags, point);
}

void CSolverView::OnRButtonDown(UINT nFlags, CPoint point)
{
	try_color(empty_cell, point);
	CView::OnRButtonDown(nFlags, point);
}

void CSolverView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_drawing = false;

	CView::OnRButtonUp(nFlags, point);
}

void CSolverView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_drawing)
	{
		toCellPoint(point);
		if (GetDocument()->isField(point))
		{
			char c = GetDocument()->cell(point);
			if (c != empty_cell && c != filled_cell && c != m_color)
			{
				GetDocument()->setCell(point, m_color);
				Invalidate(false);
			}
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void CSolverView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_SPACE:
		if (GetDocument()->ensure())
		{
			GetDocument()->commit();
			GetDocument()->auto_togle_row_num();
			Invalidate(false);
		}
		else AfxMessageBox(L"Invalid gess");
		break;
	case VK_BACK:
		GetDocument()->rollback();	
		Invalidate(false);
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
