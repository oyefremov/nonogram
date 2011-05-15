// SolverView.h : interface of the CSolverView class
//


#pragma once


class CSolverView : public CView
{
private:
	void drawGrid(CDC* pDC, int size, int step, COLORREF color, int extraX = 0, int extraY = 0);
	void drawCell(CDC* pDC, int size, int x, int y);
	void drawNumbers(CDC* pDC);

	void toCellPoint(CPoint& pt) const;

	bool m_drawing;
	char m_color;
	static const int cell_size = 20;

protected: // create from serialization only
	CSolverView();
	DECLARE_DYNCREATE(CSolverView)

// Attributes
public:
	CSolverDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSolverView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void try_color(char newColor, CPoint point);
	void try_nums(CPoint point);

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in SolverView.cpp
inline CSolverDoc* CSolverView::GetDocument() const
   { return reinterpret_cast<CSolverDoc*>(m_pDocument); }
#endif

