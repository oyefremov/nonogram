
// nclient.h : main header file for the nclient application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CNclientApp:
// See nclient.cpp for the implementation of this class
//

class CNclientApp : public CWinApp
{
public:
	CNclientApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CNclientApp theApp;
