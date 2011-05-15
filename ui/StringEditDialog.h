#pragma once

#include <string>
#include "afxwin.h"

// CStringEditDialog dialog

class CStringEditDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CStringEditDialog)
	
	std::string m_str;
	const wchar_t* m_prompt;
	const wchar_t* m_caption;

public:
	CStringEditDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStringEditDialog();

	static bool edit(std::string& str, const wchar_t* prompt, const wchar_t* caption);

// Dialog Data
	enum { IDD = IDD_EDIT_STRING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CEdit m_string;
	CStatic m_promptLabel;
public:
	virtual BOOL OnInitDialog();
};
