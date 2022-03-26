// CDisplayRange.cpp: 实现文件
//

#include "pch.h"
#include "BCRS.h"
#include "CDisplayRange.h"
#include "afxdialogex.h"


// CDisplayRange 对话框

IMPLEMENT_DYNAMIC(CDisplayRange, CDialogEx)

CDisplayRange::CDisplayRange(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DISPLAY_RANGE, pParent)
{

}

CDisplayRange::~CDisplayRange()
{
}

void CDisplayRange::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDisplayRange, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDisplayRange 消息处理程序


BOOL CDisplayRange::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 读取ini并显示
	CString info;
	VCTSTR vsKeyName = { TEXT("CAM1_L"), TEXT("CAM1_T"), TEXT("CAM1_W"), TEXT("CAM1_H"),
						 TEXT("CAM2_L"), TEXT("CAM2_T"), TEXT("CAM2_W"), TEXT("CAM2_H") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		ReadIni(TEXT("ROI"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
		GetDlgItem(IDC_EDT_ROI_CAM1_L + i)->SetWindowTextW(info);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDisplayRange::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 保存ini
	CString info;
	VCTSTR vsKeyName = { TEXT("CAM1_L"), TEXT("CAM1_T"), TEXT("CAM1_W"), TEXT("CAM1_H"),
						 TEXT("CAM2_L"), TEXT("CAM2_T"), TEXT("CAM2_W"), TEXT("CAM2_H") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		GetDlgItem(IDC_EDT_ROI_CAM1_L + i)->GetWindowTextW(info);
		WriteIni(TEXT("ROI"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
	}
	CDialogEx::OnClose();
}
