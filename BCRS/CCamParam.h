#pragma once


// CCamParam 对话框

class CCamParam : public CDialogEx
{
	DECLARE_DYNAMIC(CCamParam)

public:
	CCamParam(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCamParam();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAM_PARAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
};
