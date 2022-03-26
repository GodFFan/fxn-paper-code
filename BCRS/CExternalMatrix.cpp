// CExternalMatrix.cpp: 实现文件
//

#include "pch.h"
#include "BCRS.h"
#include "CExternalMatrix.h"
#include "afxdialogex.h"


// CExternalMatrix 对话框

IMPLEMENT_DYNAMIC(CExternalMatrix, CDialogEx)

CExternalMatrix::CExternalMatrix(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXTERNAL_MATRIX, pParent)
{

}

CExternalMatrix::~CExternalMatrix()
{
}

void CExternalMatrix::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExternalMatrix, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CExternalMatrix 消息处理程序


BOOL CExternalMatrix::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	// 读取ini并显示
	CString info;
	VCTSTR vsKeyName = {
		TEXT("ROT_11"), TEXT("ROT_12"), TEXT("ROT_13"),
		TEXT("ROT_21"), TEXT("ROT_22"), TEXT("ROT_23"),
		TEXT("ROT_31"), TEXT("ROT_32"), TEXT("ROT_33"),
		TEXT("TRAN_1"), TEXT("TRAN_2"), TEXT("TRAN_3") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		ReadIni(TEXT("EXTERNAL"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
		GetDlgItem(IDC_EDT_EXT_ROT_11 + i)->SetWindowTextW(info);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CExternalMatrix::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 保存ini
	CString info;
	VCTSTR vsKeyName = {
		TEXT("ROT_11"), TEXT("ROT_12"), TEXT("ROT_13"),
		TEXT("ROT_21"), TEXT("ROT_22"), TEXT("ROT_23"),
		TEXT("ROT_31"), TEXT("ROT_32"), TEXT("ROT_33"),
		TEXT("TRAN_1"), TEXT("TRAN_2"), TEXT("TRAN_3") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		GetDlgItem(IDC_EDT_EXT_ROT_11 + i)->GetWindowTextW(info);
		WriteIni(TEXT("EXTERNAL"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
	}
	CDialogEx::OnClose();
}
