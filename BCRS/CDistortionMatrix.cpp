// CDistortionMatrix.cpp: 实现文件
//

#include "pch.h"
#include "BCRS.h"
#include "CDistortionMatrix.h"
#include "afxdialogex.h"


// CDistortionMatrix 对话框

IMPLEMENT_DYNAMIC(CDistortionMatrix, CDialogEx)

CDistortionMatrix::CDistortionMatrix(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DISTORTION_MATRIX, pParent)
{

}

CDistortionMatrix::~CDistortionMatrix()
{
}

void CDistortionMatrix::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDistortionMatrix, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDistortionMatrix 消息处理程序


BOOL CDistortionMatrix::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 读取ini并显示
	CString info;
	VCTSTR vsKeyName = {
		TEXT("CAM1_K1"), TEXT("CAM1_K2"), TEXT("CAM1_P1"), TEXT("CAM1_P2"),
		TEXT("CAM2_K1"), TEXT("CAM2_K2"), TEXT("CAM2_P1"), TEXT("CAM2_P2") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		ReadIni(TEXT("DISTORTION"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
		GetDlgItem(IDC_EDT_DIS_CAM1_K1 + i)->SetWindowTextW(info);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDistortionMatrix::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 保存ini
	CString info;
	VCTSTR vsKeyName = {
		TEXT("CAM1_K1"), TEXT("CAM1_K2"), TEXT("CAM1_P1"), TEXT("CAM1_P2"),
		TEXT("CAM2_K1"), TEXT("CAM2_K2"), TEXT("CAM2_P1"), TEXT("CAM2_P2") };
	for (int i = 0;i < vsKeyName.size();i++)
	{
		GetDlgItem(IDC_EDT_DIS_CAM1_K1 + i)->GetWindowTextW(info);
		WriteIni(TEXT("DISTORTION"), vsKeyName.at(i), info, PATH_SYSTEM_CONFIG);
	}
	CDialogEx::OnClose();
}
