﻿#pragma once


// CExternalMatrix 对话框

class CExternalMatrix : public CDialogEx
{
	DECLARE_DYNAMIC(CExternalMatrix)

public:
	CExternalMatrix(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CExternalMatrix();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXTERNAL_MATRIX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
