// StringEditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ui.h"
#include "StringEditDialog.h"
#include "afxdialogex.h"


// CStringEditDialog dialog

IMPLEMENT_DYNAMIC(CStringEditDialog, CDialogEx)

CStringEditDialog::CStringEditDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStringEditDialog::IDD, pParent)
{

}

CStringEditDialog::~CStringEditDialog()
{
}

void CStringEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT_EDIT, m_string);
	DDX_Control(pDX, IDC_PROMPT_LABEL, m_promptLabel);
}

bool CStringEditDialog::edit(std::string& str, const wchar_t* prompt, const wchar_t* caption)
{
	CStringEditDialog dlg;
	dlg.m_str = str;
	dlg.m_prompt = prompt;
	dlg.m_caption = caption;

	if (dlg.DoModal() == IDOK)
	{
		str = dlg.m_str;
		return true;
	}
	return false;
}


BEGIN_MESSAGE_MAP(CStringEditDialog, CDialogEx)
END_MESSAGE_MAP()


// CStringEditDialog message handlers


BOOL CStringEditDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_caption);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
