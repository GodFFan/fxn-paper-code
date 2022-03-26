// CInternalMatrix.cpp: 实现文件
//

#include "pch.h"
#include "BCRS.h"
#include "CInternalMatrix.h"
#include "afxdialogex.h"


// CInternalMatrix 对话框

IMPLEMENT_DYNAMIC(CInternalMatrix, CDialogEx)

CInternalMatrix::CInternalMatrix(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INTERNAL_MATRIX, pParent)
{

}

CInternalMatrix::~CInternalMatrix()
{
}

void CInternalMatrix::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInternalMatrix, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CInternalMatrix 消息处理程序


BOOL CInternalMatrix::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 读取ini并显示
	CString info;
	VCTSTR vsKeyName = {
		TEXT("CAM1_11"), TEXT("CAM1_12"), TEXT("CAM1_13"),
		TEXT("CAM1_21"), TEXT("CAM1_22"), TEXT("CAM1_23"),
		TEXT("CAM1_31"), TEXT("CAM1_32"), TEXT("CAM1_33"),
		TEXT("CAM2_11"), TEXT("CAM2_12"), TEXT("CAM2_13"),
		TEXT("CAM2_21"), TEXT("CAM2_22"), TEXT("CAM2_23"),
		TEXT("CAM2_31"), TEXT("CAM2_32"), TEXT("CAM2_33") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		ReadIni(TEXT("INTERNAL"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
		GetDlgItem(IDC_EDT_INT_CAM1_11 + i)->SetWindowTextW(info);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CInternalMatrix::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 保存ini
	CString info;
	VCTSTR vsKeyName = {
		TEXT("CAM1_11"), TEXT("CAM1_12"), TEXT("CAM1_13"),
		TEXT("CAM1_21"), TEXT("CAM1_22"), TEXT("CAM1_23"),
		TEXT("CAM1_31"), TEXT("CAM1_32"), TEXT("CAM1_33"),
		TEXT("CAM2_11"), TEXT("CAM2_12"), TEXT("CAM2_13"),
		TEXT("CAM2_21"), TEXT("CAM2_22"), TEXT("CAM2_23"),
		TEXT("CAM2_31"), TEXT("CAM2_32"), TEXT("CAM2_33") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		GetDlgItem(IDC_EDT_INT_CAM1_11 + i)->GetWindowTextW(info);
		WriteIni(TEXT("INTERNAL"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
	}
	CDialogEx::OnClose();
}
