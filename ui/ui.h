// ui.h : main header file for the ui application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CSolverApp:
// See ui.cpp for the implementation of this class
//

class CSolverApp : public CWinApp
{
public:
	CSolverApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSolverApp theApp;