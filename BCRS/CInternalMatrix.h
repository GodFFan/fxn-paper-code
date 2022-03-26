#pragma once


// CInternalMatrix 对话框

class CInternalMatrix : public CDialogEx
{
	DECLARE_DYNAMIC(CInternalMatrix)

public:
	CInternalMatrix(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CInternalMatrix();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTERNAL_MATRIX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
