// CCamParam.cpp: 实现文件
//

#include "pch.h"
#include "BCRS.h"
#include "CCamParam.h"
#include "afxdialogex.h"
#include <vector>

// CCamParam 对话框

IMPLEMENT_DYNAMIC(CCamParam, CDialogEx)

CCamParam::CCamParam(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAM_PARAM, pParent)
{

}

CCamParam::~CCamParam()
{
}

void CCamParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCamParam, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCamParam 消息处理程序


BOOL CCamParam::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 读取ini并显示
	CString info;
	VCTSTR vsKeyName = {TEXT("EXPOSURE"), TEXT("GAIN"), TEXT("FPS")};
	for (int i = 0;i < vsKeyName.size();i++)
	{
		ReadIni(TEXT("CONFIG"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
		GetDlgItem(IDC_EDT_EXPOSURE + i)->SetWindowTextW(info);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCamParam::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	// 保存ini
	CString info;
	VCTSTR vsKeyName = { TEXT("EXPOSURE"), TEXT("GAIN"), TEXT("FPS") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		GetDlgItem(IDC_EDT_EXPOSURE + i)->GetWindowTextW(info);
		WriteIni(TEXT("CONFIG"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
	}

	CDialogEx::OnClose();
}
