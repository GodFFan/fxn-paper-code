#pragma once


// CDisplayRange 对话框

class CDisplayRange : public CDialogEx
{
	DECLARE_DYNAMIC(CDisplayRange)

public:
	CDisplayRange(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDisplayRange();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAY_RANGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
