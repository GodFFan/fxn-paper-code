
// BCRSDlg.cpp: 实现文件
//
#include "pch.h"
#include "framework.h"
#include "BCRS.h"
#include "BCRSDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBCRSDlg 对话框



CBCRSDlg::CBCRSDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BCRS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_MvDevList = MV_CC_DEVICE_INFO_LIST{};
	m_SignalFlag = SignalFlag{ 0 };

	for (int i = 0; i <MAX_DEVICE_NUM; i++)
	{
		m_MvCam[i] = NULL;
		m_HwndDisplay[i] = NULL;
		m_nCurrentDevicesNum = -1;
		m_FrameCount[i] = FrameCount{ 0 };
	}
	m_StereoParams = StereoParams{};
	m_TargetMonitor = TargetMonitor{};
	m_SharedData = SharedData{};
}

void CBCRSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBCRSDlg, CDialogEx)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(32771, 32800, &CBCRSDlg::OnMenuCommand)// 菜单控件ID
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INITCAM, &CBCRSDlg::OnBnClickedBtnInitcam)
	ON_BN_CLICKED(IDC_BTN_CAPTURE, &CBCRSDlg::OnBnClickedBtnCapture)
	ON_BN_CLICKED(IDC_BTN_SAVEIMG, &CBCRSDlg::OnBnClickedBtnSaveimg)
	ON_BN_CLICKED(IDC_BTN_MEASURE, &CBCRSDlg::OnBnClickedBtnMeasure)
	ON_BN_CLICKED(IDC_BTN_SENDDATA, &CBCRSDlg::OnBnClickedBtnSenddata)
	ON_BN_CLICKED(IDC_BTN_CLOSECAM, &CBCRSDlg::OnBnClickedBtnClosecam)
	ON_BN_CLICKED(IDC_BTN_ZERO, &CBCRSDlg::OnBnClickedBtnZero)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CBCRSDlg 消息处理程序


void CBCRSDlg::PreInitDialog()
{
	// 初始化窗口之前加载配置文件
// ---------------------------------------------------------------
	// @简述：配置文件存在性检查
	// @说明：如果配置文件存在，跳过；如果配置文件不存在，如首次安装时
	//		  则创建新的空配置文件，需要在界面中设置。
	// ---------------------------------------------------------------

	// 定义配置文件路径
	std::vector<CString> strPath = { PATH_BUTTON_LOG, PATH_SYSTEM_CONFIG };
	for (auto &it : strPath)
	{
		// 查找文件
		CFileFind finder;
		// 如果文件不存在，则创建文件
		if (!finder.FindFile(it))
		{
			CFile file(it, CFile::modeCreate);
			file.Close();
		}
		finder.Close();
	}
	CDialogEx::PreInitDialog();
}

BOOL CBCRSDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CBCRSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// ---------------------------------------------------------------
	// @简述：菜单标签选项勾选
	// ---------------------------------------------------------------
	CString cstrMenuLabel = TEXT("");
	ReadIni(TEXT("MENU"), TEXT("TRIG"), cstrMenuLabel, PATH_BUTTON_LOG);
	if (TEXT("CONT") == cstrMenuLabel)OnMenuCommand(ID_MENU_CONT);
	else if (TEXT("SOFT") == cstrMenuLabel)OnMenuCommand(ID_MENU_SOFT);
	else if (TEXT("HARD") == cstrMenuLabel)OnMenuCommand(ID_MENU_HARD);
	ReadIni(TEXT("MENU"), TEXT("SAVE"), cstrMenuLabel, PATH_BUTTON_LOG);
	if (TEXT("JPG") == cstrMenuLabel)OnMenuCommand(ID_MENU_SAVE2JPG);
	else if (TEXT("PNG") == cstrMenuLabel)OnMenuCommand(ID_MENU_SAVE2PNG);
	else if (TEXT("BMP") == cstrMenuLabel)OnMenuCommand(ID_MENU_SAVE2BMP);
	else if (TEXT("TIFF") == cstrMenuLabel)OnMenuCommand(ID_MENU_SAVE2TIFF);
	ReadIni(TEXT("MENU"), TEXT("SHOW"), cstrMenuLabel, PATH_BUTTON_LOG);
	if (TEXT("SINGLE") == cstrMenuLabel)OnMenuCommand(ID_MENU_DISPLAY_SINGLE);
	else if (TEXT("DOUBLE") == cstrMenuLabel)OnMenuCommand(ID_MENU_DISPLAY_DOUBLE);

	// 初始化显示句柄
	InitDisplayHand();

	// 初始化相机参数
	InitStereoParams();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBCRSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBCRSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBCRSDlg::OnMenuCommand(UINT nID)
{
	switch (nID)
	{
	case ID_MENU_CAM_PARAM:
	{
		CCamParam dlg;
		dlg.DoModal();
	}break;
	case ID_MENU_INTERNAL_MATRIX:
	{
		CInternalMatrix dlg;
		dlg.DoModal();
	}break;
	case ID_MENU_EXTERNAL_MATRIX:
	{
		CExternalMatrix dlg;
		dlg.DoModal();
	}break;
	case ID_MENU_DISTORTION_MATRIX:
	{
		CDistortionMatrix dlg;
		dlg.DoModal();
	}break;
	case ID_MENU_CONT:
	{
		CheckMenuItem(MENU_IMG_CAPTURE,ID_MENU_CONT, ID_MENU_HARD, ID_MENU_CONT);
		WriteIni(TEXT("MENU"), TEXT("TRIG"), TEXT("CONT"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_SOFT:
	{
		CheckMenuItem(MENU_IMG_CAPTURE,ID_MENU_CONT, ID_MENU_HARD, ID_MENU_SOFT);
		WriteIni(TEXT("MENU"), TEXT("TRIG"), TEXT("SOFT"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_HARD:
	{
		CheckMenuItem(MENU_IMG_CAPTURE,ID_MENU_CONT, ID_MENU_HARD, ID_MENU_HARD);
		WriteIni(TEXT("MENU"), TEXT("TRIG"), TEXT("HARD"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_SAVE2JPG:
	{
		CheckMenuItem(MENU_IMG_FORMAT,ID_MENU_SAVE2JPG, ID_MENU_SAVE2TIFF, ID_MENU_SAVE2JPG);
		WriteIni(TEXT("MENU"), TEXT("SAVE"), TEXT("JPG"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_SAVE2PNG:
	{
		CheckMenuItem(MENU_IMG_FORMAT,ID_MENU_SAVE2JPG, ID_MENU_SAVE2TIFF, ID_MENU_SAVE2PNG);
		WriteIni(TEXT("MENU"), TEXT("SAVE"), TEXT("PNG"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_SAVE2BMP:
	{
		CheckMenuItem(MENU_IMG_FORMAT,ID_MENU_SAVE2JPG, ID_MENU_SAVE2TIFF, ID_MENU_SAVE2BMP);
		WriteIni(TEXT("MENU"), TEXT("SAVE"), TEXT("BMP"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_SAVE2TIFF:
	{
		CheckMenuItem(MENU_IMG_FORMAT,ID_MENU_SAVE2JPG, ID_MENU_SAVE2TIFF, ID_MENU_SAVE2TIFF);
		WriteIni(TEXT("MENU"), TEXT("SAVE"), TEXT("TIFF"), PATH_BUTTON_LOG);
	}break;
	case ID_MENU_DISPLAY_SINGLE:
	{
		CheckMenuItem(MENU_SET_DISPLAY,ID_MENU_DISPLAY_SINGLE, ID_MENU_DISPLAY_DOUBLE, ID_MENU_DISPLAY_SINGLE);
		WriteIni(TEXT("MENU"), TEXT("SHOW"), TEXT("SINGLE"), PATH_BUTTON_LOG);
		SwitchDisplayMode(1);
	}break;
	case ID_MENU_DISPLAY_DOUBLE:
	{
		CheckMenuItem(MENU_SET_DISPLAY,ID_MENU_DISPLAY_SINGLE, ID_MENU_DISPLAY_DOUBLE, ID_MENU_DISPLAY_DOUBLE);
		WriteIni(TEXT("MENU"), TEXT("SHOW"), TEXT("DOUBLE"), PATH_BUTTON_LOG);
		SwitchDisplayMode(2);
	}break;
	case ID_MENU_DISPLAY_RANGE:
	{
		CDisplayRange dlg;
		dlg.DoModal();
	}break;
	default:
		break;
	}
}



void CBCRSDlg::OnBnClickedBtnInitcam()
{
	// ---------------------------------------------------------------
	// @简述：相机初始化
	// @说明：枚举设备
	//        检查设备
	//        开启相机
	//        设置参数(曝光、增益、帧率、触发模式)
	//        弹窗消息
	// ---------------------------------------------------------------
	
	// 启用相机数
	int nUsingCamCount = 0;

	// 枚举设备
	EnumDevices();
	if (m_MvDevList.nDeviceNum < 1)
	{
		MessageBoxW(TEXT("无可用设备，请检查硬件连接情况！"), TEXT("警告"), MB_ICONWARNING);
		return;
	}
	// 检查设备	
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		if (!CMvCamera::IsDeviceAccessible(m_MvDevList.pDeviceInfo[i], MV_ACCESS_Monitor))
		{
			MessageBoxW(TEXT("等待硬件重置，请稍后重试！"), TEXT("提示"), MB_ICONWARNING);
			return;
		}
	}
	// 打开相机
	OpenDevice();
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		if (NULL != m_MvCam[i])
		{
			CheckDlgButton(IDC_CHK_CAM1 + i, TRUE);
			nUsingCamCount++;
		}
	}
	// 设置参数(曝光、增益、帧率、触发模式)
	SetExposure();
	SetGain();
	SetFPS();
	SetTrigger();

	// 弹窗消息
	CString info;
	info.Format(TEXT("在线相机数：%d\r\n启用相机数：%d"), m_MvDevList.nDeviceNum, nUsingCamCount);
	MessageBoxW(info, TEXT("初始化完成"));
}


void CBCRSDlg::OnBnClickedBtnCapture()
{
	// ---------------------------------------------------------------
	// @简述：
	// @说明：1.连续采集模式下点击“开始采集”进行采集操作，随后按钮显
	//			示“停止采集”，点击“停止采集”则停止采集；
	//		  2.软触发模式下点击“开始采集”则采集一次
	//		  3.硬触发模式下点击“开始采集”进入采集状态，接收到采集信
	//			号则采集一次；按钮切换同“1”
	// ---------------------------------------------------------------

	// 设备启用状态检查
	if (!(IsDlgButtonChecked(IDC_CHK_CAM1) || IsDlgButtonChecked(IDC_CHK_CAM2)))
	{
		MessageBoxW(TEXT("无可用设备"), TEXT("提示"), MB_ICONWARNING);
		return;
	}
	// 读取配置
	CString ini;
	ReadIni(TEXT("MENU"), TEXT("TRIG"), ini, PATH_BUTTON_LOG);
	// 按钮文本
	CString cstrBtnText;
	GetDlgItem(IDC_BTN_CAPTURE)->GetWindowTextW(cstrBtnText);

	if (TEXT("开始采集") == cstrBtnText)
	{
		if (!m_SignalFlag.bGrabbingFlag)
		{
			m_SignalFlag.bGrabbingFlag = true;
			StartGrabbing();
		}
		if (TEXT("CONT") == ini || TEXT("HARD") == ini)
		{
			GetDlgItem(IDC_BTN_CAPTURE)->SetWindowTextW(TEXT("停止采集"));
		}else if (TEXT("SOFT") == ini)
		{
			SoftwareTriggersOnce();
			GetDlgItem(IDC_BTN_CAPTURE)->SetWindowTextW(TEXT("开始采集"));
		}
	}
	else if (TEXT("停止采集") == cstrBtnText)
	{
		if (m_SignalFlag.bGrabbingFlag)
		{
			m_SignalFlag.bGrabbingFlag = false;
			StopGrabbing();
		}
		GetDlgItem(IDC_BTN_CAPTURE)->SetWindowTextW(TEXT("开始采集"));
	}
}


void CBCRSDlg::OnBnClickedBtnSaveimg()
{
	// 
}


void CBCRSDlg::OnBnClickedBtnMeasure()
{
	// ---------------------------------------------------------------
	// @简述：
	// @说明：1.点击“开始测距”后，Flag=true,按钮文本变为“停止测距”
	//        2.点击“停止测距”后，Flag=false,按钮文本变为“开始测距”
	// ---------------------------------------------------------------

	CString cstrBtnText;
	GetDlgItemTextW(IDC_BTN_MEASURE, cstrBtnText);
	if ( TEXT("开始测距")==cstrBtnText )
	{
		SetDlgItemTextW(IDC_BTN_MEASURE, TEXT("停止测距"));
		m_SignalFlag.bMeasureFlag = true;
		InitMonitor(1);// 初始化监视器
		SetTimer(TIMER_TARGET_DETECTION, 10, NULL);
		SetTimer(TIMER_LOCATION_COMPUTED, 10, NULL);
		// 刷新display2
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_SHOW);
	}
	else if (TEXT("停止测距")==cstrBtnText )
	{
		SetDlgItemTextW(IDC_BTN_MEASURE, TEXT("开始测距"));
		m_SignalFlag.bMeasureFlag = false;
		InitMonitor(0);// 重置监视器
		KillTimer(TIMER_LOCATION_COMPUTED);
		KillTimer(TIMER_TARGET_DETECTION);
	}
}


void CBCRSDlg::OnBnClickedBtnSenddata()
{
	// ---------------------------------------------------------------
		// @简述：
		// @说明：1.点击“发送数据”后，Flag=true,按钮文本变为“停止发送”
		//        2.点击“停止发送”后，Flag=false,按钮文本变为“发送数据”
		// ---------------------------------------------------------------

	CString cstrBtnText;
	GetDlgItemTextW(IDC_BTN_SENDDATA, cstrBtnText);
	if (TEXT("发送数据") == cstrBtnText)
	{
		InitSharedData(1);// 初始化共享数据
		CreateFileMap();// 创建共享内存
		if (NULL != m_SharedData.hSharedMemoryHandle)
		{
			SetDlgItemTextW(IDC_BTN_SENDDATA, TEXT("停止发送"));
			m_SignalFlag.bSendDataFlag = true;
		}
	}
	else if (TEXT("停止发送") == cstrBtnText)
	{
		CloseFileMap();
		InitSharedData(0);// 重置共享数据
		SetDlgItemTextW(IDC_BTN_SENDDATA, TEXT("发送数据"));
		m_SignalFlag.bSendDataFlag = false;
	}
}


void CBCRSDlg::OnBnClickedBtnClosecam()
{
	// 还原信号状态Flag
	m_SignalFlag = SignalFlag{ 0 };
	// 还原按钮状态
	GetDlgItem(IDC_BTN_CAPTURE)->SetWindowTextW(TEXT("开始采集"));
	GetDlgItem(IDC_BTN_MEASURE)->SetWindowTextW(TEXT("开始测距"));
	GetDlgItem(IDC_BTN_SENDDATA)->SetWindowTextW(TEXT("发送数据"));
	// 关闭设备
	CloseDevice();
	// 刷新display控件
	GetDlgItem(IDC_DISPLAY1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DISPLAY1)->ShowWindow(SW_SHOW);
	if (GetDlgItem(IDC_DISPLAY2)->IsWindowVisible())
	{
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_SHOW);
	}
}


void CBCRSDlg::OnBnClickedBtnZero()
{
	CString info;
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		// 重置帧数
		m_FrameCount[i] = FrameCount{ 0 };
		info.Format(TEXT("%d"), m_FrameCount[i].nFrameCount);
		GetDlgItem(IDC_EDT_CAPTURE_COUNT1 + i)->SetWindowTextW(info);
		info.Format(TEXT("%d"), m_FrameCount[i].nLostFrame);
		GetDlgItem(IDC_EDT_LOSS_COUNT1 + i)->SetWindowTextW(info);
	}
}

void CBCRSDlg::EnumDevices()
{
	CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_MvDevList);
}

void CBCRSDlg::OpenDevice()
{
	int nRet = MV_OK;
	for (unsigned int i = 0, j = 0; j < m_MvDevList.nDeviceNum; i++, j++)
	{
		if (NULL == m_MvCam[i])
		{
			m_MvCam[i] = new CMvCamera;// 在关闭软件之前回收
		}
		nRet = m_MvCam[i]->Open(m_MvDevList.pDeviceInfo[j]);
		if (MV_OK != nRet)
		{
			delete(m_MvCam[i]);
			m_MvCam[i] = NULL;
			i--;
			continue;
		}
		else
		{
			//探测网络最佳包大小(只对GigE相机有效)
			if (m_MvDevList.pDeviceInfo[j]->nTLayerType == MV_GIGE_DEVICE)
			{
				unsigned int nPacketSize = 0;
				nRet = m_MvCam[i]->GetOptimalPacketSize(&nPacketSize);
				if (nPacketSize > 0)
				{
					nRet = m_MvCam[i]->SetIntValue("GevSCPSPacketSize", nPacketSize);
					if (nRet != MV_OK)
					{
						MessageBoxW(TEXT("设置网络数据包大小失败"), TEXT("警告"), MB_ICONWARNING);
					}
				}
				else
				{
					MessageBoxW(TEXT("读取网络数据包大小失败"), TEXT("警告"), MB_ICONWARNING);
				}
			}
		}
	}
}

void CBCRSDlg::CloseDevice()
{
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		m_MvCam[i]->Close();
	}
}

void CBCRSDlg::StartGrabbing()
{
	// ---------------------------------------------------------------
	// @简述：开始采集
	// @说明：设置计时器更新帧数显示
	//        重置帧数
	//		  开始采集后创建子线程取流
	// ---------------------------------------------------------------

	// 计时器
	SetTimer(TIMER_DISPLAY_FRAME_COUNT, 10, NULL);

	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 重置帧数
		m_FrameCount[i] = FrameCount{ 0 };
		// 设备启用状态下，开始采集
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->StartGrabbing();
		}
		while (-1 != m_nCurrentDevicesNum)
		{
			Sleep(1);
		}
		m_nCurrentDevicesNum = i;
		// 创建工作线程
		AfxBeginThread(GetOneFrameFromVideoThread, this);
	}
	return;
}

void CBCRSDlg::StopGrabbing()
{
	// ---------------------------------------------------------------
	// @简述：停止采集
	// @说明：关闭计时器
	//        停止采集
	//        采集线程句柄设为nullptr
	// ---------------------------------------------------------------
	KillTimer(TIMER_DISPLAY_FRAME_COUNT);

	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 设备启用状态下，停止采集
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->StopGrabbing();
		}
	}
}

void CBCRSDlg::SoftwareTriggersOnce()
{
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		// 设备启用状态下，设置参数
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->CommandExecute("TriggerSoftware");
		}
	}
}

void CBCRSDlg::InitDisplayHand()
{
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		m_HwndDisplay[i] = GetDlgItem(IDC_DISPLAY1 + i)->GetSafeHwnd();
	}
}

void CBCRSDlg::DisplayFrameCount()
{
	int nRet = 0;
	int nFrameCount = 0;
	int nLostFrame = 0;
	CString info = TEXT("");
	MV_MATCH_INFO_NET_DETECT stMatchInfoNetDetect = { 0 };
	MV_MATCH_INFO_USB_DETECT stMatchInfoUSBDetect = { 0 };
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO stDevInfo = { 0 };
		m_MvCam[i]->GetDeviceInfo(&stDevInfo);
		if (stDevInfo.nTLayerType == MV_GIGE_DEVICE)
		{
			nRet = m_MvCam[i]->GetGevAllMatchInfo(&stMatchInfoNetDetect);
			nLostFrame = stMatchInfoNetDetect.nLostFrameCount;
			nFrameCount = stMatchInfoNetDetect.nNetRecvFrameCount;
		}
		else if (stDevInfo.nTLayerType == MV_USB_DEVICE)
		{
			nRet = m_MvCam[i]->GetU3VAllMatchInfo(&stMatchInfoUSBDetect);
			nLostFrame = stMatchInfoUSBDetect.nErrorFrameCount;
			nFrameCount = stMatchInfoUSBDetect.nReceivedFrameCount;
		}
		else
		{
			return;
		}
		// 采集帧数显示更新，最多显示1000000帧
		if (MV_OK == nRet)
		{
			m_FrameCount[i].nFrameCount = nFrameCount % 1000000;
			m_FrameCount[i].nLostFrame = nLostFrame % 1000000;
		}
		info.Format(TEXT("%d"), m_FrameCount[i].nFrameCount);
		GetDlgItem(IDC_EDT_CAPTURE_COUNT1 + i)->SetWindowTextW(info);
		info.Format(TEXT("%d"), m_FrameCount[i].nLostFrame);
		GetDlgItem(IDC_EDT_LOSS_COUNT1 + i)->SetWindowTextW(info);
	}

}

void CBCRSDlg::SetExposure()
{
	// 读取配置
	CString ini;
	ReadIni(TEXT("CONFIG"), TEXT("EXPOSURE"), ini, PATH_SYSTEM_CONFIG);
	if (_wtof(ini) < 0)
	{
		MessageBoxW(TEXT("曝光需设为非负值"), TEXT("参数错误"), MB_ICONERROR);
		return;
	}
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 设备启用状态下，设置参数
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->SetEnumValue("ExposureMode", 0);
			m_MvCam[i]->SetFloatValue("ExposureTime", _wtof(ini));
		}
	}
}

void CBCRSDlg::SetGain()
{
	// 读取配置
	CString ini;
	ReadIni(TEXT("CONFIG"), TEXT("GAIN"), ini, PATH_SYSTEM_CONFIG);
	if (_wtof(ini) < 0)
	{
		MessageBoxW(TEXT("增益需设为非负值"), TEXT("参数错误"), MB_ICONERROR);
		return;
	}
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 设备启用状态下，设置参数
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->SetEnumValue("GainAuto", 0);
			m_MvCam[i]->SetFloatValue("Gain", _wtof(ini));
		}
	}
}

void CBCRSDlg::SetFPS()
{
	// 读取配置
	CString ini;
	ReadIni(TEXT("CONFIG"), TEXT("FPS"), ini, PATH_SYSTEM_CONFIG);
	if (_wtof(ini) < 0)
	{
		MessageBoxW(TEXT("帧率需设为非负值"), TEXT("参数错误"), MB_ICONERROR);
		return;
	}
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 设备启用状态下，设置参数
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->SetBoolValue("AcquisitionFrameRateEnable", true);
			m_MvCam[i]->SetFloatValue("AcquisitionFrameRate", _wtof(ini));
		}
	}
}

void CBCRSDlg::SetTrigger()
{
	// 读取配置
	CString ini;
	ReadIni(TEXT("MENU"), TEXT("TRIG"), ini, PATH_BUTTON_LOG);
	UINT  nTriggerMode = 0, nTriggerSource = 0;
	TEXT("CONT") == ini ? nTriggerMode = TRIGGER_OFF : nTriggerMode = TRIGGER_ON;
	TEXT("SOFT") == ini ? nTriggerSource = SOFTWARE : nTriggerSource = HARDWARE;

	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		// 设备启用状态下，设置参数
		if (m_MvCam[i]->IsDeviceConnected())
		{
			m_MvCam[i]->SetEnumValue("TriggerMode", nTriggerMode);
			if (TRIGGER_ON == nTriggerMode)
			{
				m_MvCam[i]->SetEnumValue("TriggerSource", nTriggerSource);
			}
		}
	}
}

UINT CBCRSDlg::GetOneFrameFromVideoThread(LPVOID pParam)
{
	CBCRSDlg* bcrs = (CBCRSDlg*)pParam;
	int nCurrentDevicesNum = bcrs->m_nCurrentDevicesNum;
	bcrs->m_nCurrentDevicesNum = -1;
	bcrs->GetAndDisplayOneFrameFromVideo(nCurrentDevicesNum);
	return 0;
}

void CBCRSDlg::GetAndDisplayOneFrameFromVideo(int nDeviceNum)
{
	MVCC_INTVALUE_EX stIntValue = { 0 };
	int nRet = m_MvCam[nDeviceNum]->GetIntValue("PayloadSize", &stIntValue);
	if (nRet != MV_OK)
	{
		MessageBoxW(TEXT("获取帧尺寸失败"), TEXT("错误") , MB_ICONERROR);
		return;
	}
	unsigned int nDataSize = (unsigned int)stIntValue.nCurValue;
	unsigned char* pGrabBuf = (unsigned char*)malloc(sizeof(unsigned char) * nDataSize);
	if (pGrabBuf == NULL)
	{
		return;
	}
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	MV_DISPLAY_FRAME_INFO stDisplayInfo = { 0 };
	// ---------------------------------------------------------------
	// @简述：图像采集、显示与格式转换
	// @说明：1.采集、不计算位姿：不进行格式转换、显示采集的图像
	//        2.采集、计算位姿（物体不在视野内）：转换Cam1、检测目标、显示采集的图像
	//        3.采集、计算位姿（物体在视野内）：转换Cam1和Cam2、计算位姿、显示计算后的图像
	// ---------------------------------------------------------------
	while (m_SignalFlag.bGrabbingFlag)
	{
		// 视频流取帧
		nRet = m_MvCam[nDeviceNum]->GetOneFrameTimeout(pGrabBuf, nDataSize, &stImageInfo, 1000);
		// 不测距时，直接显示
		if (!m_SignalFlag.bMeasureFlag)
		{
			stDisplayInfo.hWnd = m_HwndDisplay[nDeviceNum];
			stDisplayInfo.pData = pGrabBuf;
			stDisplayInfo.nDataLen = stImageInfo.nFrameLen;
			stDisplayInfo.nWidth = stImageInfo.nWidth;
			stDisplayInfo.nHeight = stImageInfo.nHeight;
			stDisplayInfo.enPixelType = stImageInfo.enPixelType;
			m_MvCam[nDeviceNum]->DisplayOneFrame(&stDisplayInfo);
		}
		else// 测距
		{
			// 测距时只在display1中显示采集的图像
			if (0 == nDeviceNum)
			{
				stDisplayInfo.hWnd = m_HwndDisplay[nDeviceNum];
				stDisplayInfo.pData = pGrabBuf;
				stDisplayInfo.nDataLen = stImageInfo.nFrameLen;
				stDisplayInfo.nWidth = stImageInfo.nWidth;
				stDisplayInfo.nHeight = stImageInfo.nHeight;
				stDisplayInfo.enPixelType = stImageInfo.enPixelType;
				m_MvCam[nDeviceNum]->DisplayOneFrame(&stDisplayInfo);
			}
			// 检测目标
			if (!m_SignalFlag.bTargetFlag)
			{
				if (0 == nDeviceNum)
				{
					Convert2Mat(&stImageInfo, pGrabBuf, nDeviceNum);
				}
			}
			else
			{
				Convert2Mat(&stImageInfo, pGrabBuf, nDeviceNum);
			}
		}
	}
	free(pGrabBuf);
	pGrabBuf = NULL;
	return;
}


void CBCRSDlg::InitStereoParams()
{
	// StereoParams参数列表
	// cvMat IntrinsicMatrixCam1;
	// cvMat IntrinsicMatrixCam2;
	// cvMat RotationMatrixOfCam2;
	// cvMat TranslationMatrixOfCam2;
	// cvMat DistortionMatrixCam1;
	// cvMat DistortionMatrixCam2;
	// cvMat RoiCam1;
	// cvMat RoiCam2;
	// cvGpuMat RectifyMapCam1[2];
	// cvGpuMat RectifyMapCam2[2];

	{// cvMat IntrinsicMatrixCam1
		VCTSTR vsKeyName = {
			TEXT("CAM1_11"), TEXT("CAM1_12"), TEXT("CAM1_13"),
			TEXT("CAM1_21"), TEXT("CAM1_22"), TEXT("CAM1_23"),
			TEXT("CAM1_31"), TEXT("CAM1_32"), TEXT("CAM1_33") };
		StereoParams::InitMatWithDouble(m_StereoParams.IntrinsicMatrixCam1, cvSize(3, 3), TEXT("INTERNAL"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat IntrinsicMatrixCam2
		VCTSTR vsKeyName = {
			TEXT("CAM2_11"), TEXT("CAM2_12"), TEXT("CAM2_13"),
			TEXT("CAM2_21"), TEXT("CAM2_22"), TEXT("CAM2_23"),
			TEXT("CAM2_31"), TEXT("CAM2_32"), TEXT("CAM2_33") };
		StereoParams::InitMatWithDouble(m_StereoParams.IntrinsicMatrixCam2, cvSize(3, 3), TEXT("INTERNAL"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat RotationMatrixOfCam2
		VCTSTR vsKeyName = {
			TEXT("ROT_11"), TEXT("ROT_12"), TEXT("ROT_13"),
			TEXT("ROT_21"), TEXT("ROT_22"), TEXT("ROT_23"),
			TEXT("ROT_31"), TEXT("ROT_32"), TEXT("ROT_33") };
		StereoParams::InitMatWithDouble(m_StereoParams.RotationMatrixOfCam2, cvSize(3, 3), TEXT("EXTERNAL"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat TranslationMatrixOfCam2
		VCTSTR vsKeyName = {
			TEXT("TRAN_1"), TEXT("TRAN_2"), TEXT("TRAN_3") };
		StereoParams::InitMatWithDouble(m_StereoParams.TranslationMatrixOfCam2, cvSize(1, 3), TEXT("EXTERNAL"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat DistortionMatrixCam1
		VCTSTR vsKeyName = {
			TEXT("CAM1_K1"), TEXT("CAM1_K2"), TEXT("CAM1_P1"), TEXT("CAM1_P2") };
		StereoParams::InitMatWithDouble(m_StereoParams.DistortionMatrixCam1, cvSize(1, 4), TEXT("DISTORTION"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat DistortionMatrixCam2
		VCTSTR vsKeyName = {
			TEXT("CAM2_K1"), TEXT("CAM2_K2"), TEXT("CAM2_P1"), TEXT("CAM2_P2") };
		StereoParams::InitMatWithDouble(m_StereoParams.DistortionMatrixCam2, cvSize(1, 4), TEXT("DISTORTION"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat RoiCam1
		VCTSTR vsKeyName = {
			TEXT("CAM1_L"), TEXT("CAM1_T"), TEXT("CAM1_W"), TEXT("CAM1_H") };
		StereoParams::InitMatWithDouble(m_StereoParams.RoiCam1, cvSize(1, 4), TEXT("ROI"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	{// cvMat RoiCam2
		VCTSTR vsKeyName = {
			TEXT("CAM2_L"), TEXT("CAM2_T"), TEXT("CAM2_W"), TEXT("CAM2_H") };
		StereoParams::InitMatWithDouble(m_StereoParams.RoiCam2, cvSize(1, 4), TEXT("ROI"), vsKeyName, PATH_SYSTEM_CONFIG);
	}
	// 计算映射表，并将Mat转成GpuMat
	RectifyMapping();

#if 0
	// 测试程序
	double a, b, c, d, e, f, g, h, i;
	a = m_StereoParams.IntrinsicMatrixCam1.at<double>(0, 0);
	b = m_StereoParams.IntrinsicMatrixCam1.at<double>(0, 1);
	c = m_StereoParams.IntrinsicMatrixCam1.at<double>(0, 2);
	d = m_StereoParams.IntrinsicMatrixCam1.at<double>(1, 0);
	e = m_StereoParams.IntrinsicMatrixCam1.at<double>(1, 1);
	f = m_StereoParams.IntrinsicMatrixCam1.at<double>(1, 2);
	g = m_StereoParams.IntrinsicMatrixCam1.at<double>(2, 0);
	h = m_StereoParams.IntrinsicMatrixCam1.at<double>(2, 1);
	i = m_StereoParams.IntrinsicMatrixCam1.at<double>(2, 2);
#endif // 0
}

void CBCRSDlg::RectifyMapping()
{
	// 图像校正后，旋转矩阵R 投影矩阵P 重投影矩阵Q
	cvMat R1, R2, P1, P2, Q;
	// 图像校正之后，会对图像进行裁剪，validROI指裁剪后的有效区域
	cvRect validROI1, validROI2;
	// 
	cvMat mapCam1[2], mapCam2[2];
	cvSize imageSize = cvSize(IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2);

	cv::stereoRectify(m_StereoParams.IntrinsicMatrixCam1, m_StereoParams.DistortionMatrixCam1,
		m_StereoParams.IntrinsicMatrixCam2, m_StereoParams.DistortionMatrixCam2,
		imageSize, m_StereoParams.RotationMatrixOfCam2, m_StereoParams.TranslationMatrixOfCam2,
		R1, R2, P1, P2, Q, cv::CALIB_ZERO_DISPARITY, -1, imageSize, &validROI1, &validROI2);
	
	cv::initUndistortRectifyMap(m_StereoParams.IntrinsicMatrixCam1, m_StereoParams.DistortionMatrixCam1, R1, P1, imageSize, CV_32FC1, mapCam1[0], mapCam1[1]);
	cv::initUndistortRectifyMap(m_StereoParams.IntrinsicMatrixCam2, m_StereoParams.DistortionMatrixCam2, R2, P2, imageSize, CV_32FC1, mapCam2[0], mapCam2[1]);

	for (int i = 0; i < 2; i++)
	{
		m_StereoParams.RectifyMapCam1[i].upload(mapCam1[i]);
		m_StereoParams.RectifyMapCam2[i].upload(mapCam2[i]);
	}
	return;
}

void CBCRSDlg::RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight)
{
	if (NULL == pRgbData)
	{
		return;
	}
	for (unsigned int j = 0; j < nHeight; j++)
	{
		for (unsigned int i = 0; i < nWidth; i++)
		{
			unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
			pRgbData[j * (nWidth * 3) + i * 3] = pRgbData[j * (nWidth * 3) + i * 3 + 2];
			pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
		}
	}
	return;
}

bool CBCRSDlg::Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char* pData, int nCamNum)
{
	cv::Mat srcImage;
	if (pstImageInfo->enPixelType == PixelType_Gvsp_Mono8)
	{
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
	}
	else if (pstImageInfo->enPixelType == PixelType_Gvsp_RGB8_Packed)
	{
		RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
	}
	else
	{
		MessageBoxW(TEXT("不支持的文件格式！", TEXT("错误"), MB_ICONERROR));
		m_SignalFlag.bGrabbingFlag = false; //停止抓图
		return false;
	}

	if (NULL == srcImage.data)
	{
		return false;
	}

	lock_guard<mutex>guard(m_ImageCacheMutex);
	if (!m_StereoParams.ImageCache[nCamNum].empty())
		m_StereoParams.ImageCache[nCamNum].release();
	m_StereoParams.ImageCache[nCamNum].upload(srcImage);//CPU Mat upload to GPU Mat

	return true;
}

void CBCRSDlg::InitMonitor(int nMode)
{
	if (0 == nMode)
	{
		m_TargetMonitor.BackGroundImage.release();
		m_TargetMonitor = TargetMonitor{};
		return;
	}
	m_TargetMonitor.MonitorArea = cvRect(700, 1400, 1000, 100);
	while (m_TargetMonitor.BackGroundImage.empty())
	{
		Sleep(10);
		lock_guard<mutex>guard(m_ImageCacheMutex);
		if (!m_StereoParams.ImageCache[0].empty())
		{
			m_TargetMonitor.BackGroundImage = m_StereoParams.ImageCache[0](m_TargetMonitor.MonitorArea).clone();
		}
	}
}

bool CBCRSDlg::TargetDetection(cvGpuMat curImg, TargetMonitor tm)
{
	cvScalar Mean, StdDev;
	curImg = curImg(tm.MonitorArea);
	cv::cuda::absdiff(curImg, tm.BackGroundImage, curImg);
	cv::cuda::meanStdDev(curImg, Mean, StdDev);

	//dat.push_back(StdDev.val[0]);

	if (StdDev.val[0] > 20)
	{
		return true;
	}
	return false;
}

CptAgl CBCRSDlg::FindContours(cvGpuMat& gImg, int nImgID, cvGpuMat gMapx, cvGpuMat gMapy, cvMat Roi)
{
	CptAgl cvCptAgl = { 0 };
	// 缩小图片尺寸
	cvGpuMat gResize;
	cv::cuda::resize(gImg, gResize, cv::Size(), 0.5, 0.5);
	// 图形校正
	cvGpuMat gRemap;
	cv::cuda::remap(gResize, gRemap, gMapx, gMapy, cv::INTER_LINEAR);
	// 拷贝图像用于显示
	cvMat bgImg, tempImg;
	gRemap.download(tempImg);
	bgImg = tempImg.clone();
	if (1 == bgImg.channels())
	{
		cv::cvtColor(bgImg, bgImg, cv::COLOR_GRAY2BGR);//彩色
	}
	// 非灰度图转成灰度图
	if (1 != gRemap.channels())
	{
		cv::cuda::cvtColor(gRemap, gRemap, cv::COLOR_BGR2GRAY);
	}
	// ---------------------------------------------------------------
	// @简述：高斯滤波处理
	// ---------------------------------------------------------------
	cvGpuMat gGauss;
	// 高斯滤波器指针
	cv::Ptr<cv::cuda::Filter> pGaussianFilter;
	// 创建cuda高斯滤波器
	pGaussianFilter = cv::cuda::createGaussianFilter(CV_8UC1, CV_8UC1, cv::Size(5, 5), 3);
	// 执行高斯滤波器
	pGaussianFilter->apply(gRemap, gGauss);
	// ---------------------------------------------------------------
	// @简述：Canny算子提取轮廓
	// ---------------------------------------------------------------
	cvGpuMat gCanny;
	// 边缘检测指针
	cv::Ptr<cv::cuda::CannyEdgeDetector> pCannyDetector;
	// 创建边缘检测器
	pCannyDetector = cv::cuda::createCannyEdgeDetector(70, 180, 3, false);
	// 执行边缘检测器
	pCannyDetector->detect(gGauss, gCanny);
	// ---------------------------------------------------------------
	// @简述：形态学处理
	// ---------------------------------------------------------------
	cvGpuMat gDilate;
	// 构建矩形膨胀内核
	cvMat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	// 形态滤波器指针
	cv::Ptr<cv::cuda::Filter> pMorphologyFilter;
	// 创建膨胀运算滤波器
	pMorphologyFilter = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE, CV_8UC1, kernel);
	// 执行形态滤波器
	pMorphologyFilter->apply(gCanny, gDilate);
	// ---------------------------------------------------------------
	// @简述：查找ROI区域内物体轮廓
	// ---------------------------------------------------------------
	// ROI区域，Rect2d(x, y, width, height)
	cvRect2d rectRoi(Roi.at<double>(0, 0), Roi.at<double>(1, 0), Roi.at<double>(2, 0), Roi.at<double>(3, 0));
	// 预处理后的图像
	cvMat ImagePreprocess;
	gDilate.download(ImagePreprocess);
	//cv::imwrite("预处理后的图像.jpg", ImagePreprocess);// 输出测试
	// 轮廓Roi
	cvMat contourRoi = ImagePreprocess(rectRoi);
	//cv::imwrite("contourRoi.jpg", contourRoi);// 输出测试

	// 轮廓查找
	vector<vector<cvPoint>> contours;
	cv::findContours(contourRoi, contours, 0, cv::CHAIN_APPROX_SIMPLE, cv::Point(rectRoi.x, rectRoi.y));

#if 0 // 输出轮廓
	CString str = TEXT(""), temp = TEXT("");
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[0].size(); j++)
		{
			temp.Format(TEXT("%d"), contours[i][j].x);
			str = str + temp + TEXT(",");
			temp.Format(TEXT("%d"), contours[i][j].y);
			str = str + temp + TEXT(";");
		}
	}
#endif // 0
	// 轮廓查找
	for (int t = 0; t < contours.size(); t++)
	{
		// 最小外接矩形求轮廓中心
		cv::RotatedRect rrect = cv::minAreaRect(contours[t]);
		cvCptAgl.cpt = rrect.center;
		if (cvCptAgl.cpt.inside(rectRoi))
		{
			cvCptAgl.cpt = rrect.center;
			cvCptAgl.agl = rrect.angle;
			// 绘制最小矩形轮廓
			cv::Point2f points[4];
			rrect.points(points);
			for (int i = 0; i < 4; i++)
			{
				if (i < 3)
				{
					cv::line(bgImg, points[i], points[i + 1], cv::Scalar(0, 255, 0), 2, cv::LINE_AA, 0);
				}
				else
				{
					cv::line(bgImg, points[i], points[0], cv::Scalar(0, 255, 0), 2, cv::LINE_AA, 0);
				}
			}
			//绘制矩形的中心
			cv::circle(bgImg, cvCptAgl.cpt, 10, cv::Scalar(0, 255, 0), 2, cv::LINE_AA, 0);
			break;
		}
		else
		{
			cvCptAgl = CptAgl{ 0 };
		}
	}
	// 裁剪图像
	bgImg = bgImg(rectRoi);
	if (1==nImgID)// 只在display2中显示测距后的图像
	{
		ShowImageInPictureControl(bgImg, nImgID);
	}
	return CptAgl(cvCptAgl);
}

Pos CBCRSDlg::PositionAndPosture(cvGpuMat& img1, cvGpuMat& img2, StereoParams sp)
{
	Pos pos = { 0 };
	CptAgl caImg1 = { 0 }, caImg2 = { 0 };

	caImg1 = FindContours(img1, 0, sp.RectifyMapCam1[0], sp.RectifyMapCam1[1], sp.RoiCam1);
	caImg2 = FindContours(img2, 1, sp.RectifyMapCam2[0], sp.RectifyMapCam2[1], sp.RoiCam2);

	cvRect2d range(-1, -1, 1, 1);
	// caImg1.cpt、caImg2.cpt都非零时计算位姿
	if (!(caImg1.cpt.inside(range) || caImg2.cpt.inside(range)))
	{
		// 以左相机计算
		double dPosXInCamL, dPosYInCamL, dPosZInCamL, dAglInCamL;
		double dFocusXInCamL = sp.IntrinsicMatrixCam1.at<double>(0, 0);
		double dFocusYInCamL = sp.IntrinsicMatrixCam1.at<double>(1, 1);
		double dFocusInCamL = (dFocusXInCamL + dFocusYInCamL) / 2;
		// 以右相机计算 
		double dPosXInCamR, dPosYInCamR, dPosZInCamR, dAglInCamR;
		double dFocusXInCamR = sp.IntrinsicMatrixCam2.at<double>(0, 0);
		double dFocusYInCamR = sp.IntrinsicMatrixCam2.at<double>(1, 1);
		double dFocusInCamR = (dFocusXInCamR + dFocusYInCamR) / 2;

		double dBaseLine = sqrt(pow(sp.TranslationMatrixOfCam2.at<double>(0, 0), 2)
			+ pow(sp.TranslationMatrixOfCam2.at<double>(1, 0), 2)
			+ pow(sp.TranslationMatrixOfCam2.at<double>(2, 0), 2));//基线长度dBaseLine

		dPosZInCamL = dFocusInCamL * dBaseLine / (caImg1.cpt.x - caImg2.cpt.x);
		dPosXInCamL = caImg1.cpt.x * dPosZInCamL / dFocusXInCamL;
		dPosYInCamL = caImg1.cpt.y * dPosZInCamL / dFocusYInCamL;
		dAglInCamL = caImg1.agl;

		dPosZInCamR = dFocusInCamR * dBaseLine / (caImg1.cpt.x - caImg2.cpt.x);
		dPosXInCamR = dBaseLine + caImg2.cpt.x * dPosZInCamR / dFocusXInCamR;
		dPosYInCamR = caImg2.cpt.y * dPosZInCamR / dFocusYInCamR;
		dAglInCamR = caImg2.agl;

		pos.x = abs((dPosXInCamL + dPosXInCamR) / 2);
		pos.y = abs((dPosYInCamL + dPosYInCamR) / 2);
		pos.z = abs((dPosZInCamL + dPosZInCamR) / 2);
		pos.agl = abs((dAglInCamL + dAglInCamR) / 2);
	}
	return Pos(pos);
}

void CBCRSDlg::ShowImageInPictureControl(cvMat& mat, int nID)
{
	CImage cImage;
	int width = mat.cols;
	int height = mat.rows;
	int channels = mat.channels();
	cImage.Destroy();
	// 默认图像像素单通道占用1个字节
	cImage.Create(mat.cols, mat.rows, 8 * mat.channels()); 
	// 复制图像数据
	uchar* ps;
	uchar* pimg = (uchar*)cImage.GetBits();
	int step = cImage.GetPitch();
	for (int i = 0; i < height; ++i)
	{
		ps = (mat.ptr<uchar>(i));
		for (int j = 0; j < width; ++j)
		{
			if (channels == 1) // 灰度图
			{
				*(pimg + i * step + j) = ps[j];
			}
			else if (channels == 3) // 彩色图
			{
				for (int k = 0; k < 3; ++k)
				{
					*(pimg + i * step + j * 3 + k) = ps[j * 3 + k];
				}
			}
		}
	}
	// 图片控件ID
	int nDlgItemID = 0;
	0 == nID ? nDlgItemID = IDC_DISPLAY1 : nDlgItemID = IDC_DISPLAY2;
	CWnd* pWnd = GetDlgItem(nDlgItemID);
	CDC* pDC = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);
	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	cImage.Draw(pDC->m_hDC, rect);
	ReleaseDC(pDC);
	cImage.Destroy();
}

void CBCRSDlg::InitSharedData(int nMode)
{
	if (0 == nMode)
	{
		m_SharedData = SharedData{};
		return;
	}
	m_SharedData.cstrSharedMemoryName = TEXT("share_momory");
	m_SharedData.cstrReadSemaphoreName = TEXT("ReadSemaphore");
	m_SharedData.cstrWriteSemaphoreName = TEXT("WriteSemaphore");
}

void CBCRSDlg::CreateFileMap()
{
	//1.创建共享文件句柄，CreateFileMapping()函数创建一个文件映射内核对象
	m_SharedData.hSharedMemoryHandle = OpenFileMappingW(
		FILE_MAP_ALL_ACCESS, NULL, m_SharedData.cstrSharedMemoryName);
	if (NULL == m_SharedData.hSharedMemoryHandle)
	{
		MessageBoxW(TEXT("创建共享内存失败！"), TEXT("错误"), MB_ICONERROR);
		CloseFileMap();
		return;
	}
	//2.获取指向文件视图的指针，MapViewOfFile()函数负责把文件数据映射到进程的地址空间
	m_SharedData.dfcDataFromCam = (DataFromCam*)MapViewOfFile(
		m_SharedData.hSharedMemoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	//初始化信号量
	m_SharedData.hReadHandle = CreateSemaphoreW(NULL, 0, 1, m_SharedData.cstrReadSemaphoreName);
	m_SharedData.hWriteHandle = CreateSemaphoreW(NULL, 1, 1, m_SharedData.cstrWriteSemaphoreName);
}

void CBCRSDlg::CloseFileMap()
{
	//撤销文件视图
	UnmapViewOfFile(m_SharedData.dfcDataFromCam);
	//关闭共享句柄
	CloseHandle(m_SharedData.hSharedMemoryHandle);
}

void CBCRSDlg::SendSharedData()
{
	WaitForSingleObject(m_SharedData.hWriteHandle, INFINITE);
	CString str;
	DataFromCam dfc = {};
	GetDlgItem(IDC_EDT_POSX)->GetWindowTextW(str);
	dfc.dPosX = _wtof(str);
	GetDlgItem(IDC_EDT_POSY)->GetWindowTextW(str);
	dfc.dPosY = _wtof(str);
	GetDlgItem(IDC_EDT_POSZ)->GetWindowTextW(str);
	dfc.dPosZ = _wtof(str);
	GetDlgItem(IDC_EDT_AGL)->GetWindowTextW(str);
	dfc.dAgl = _wtof(str);
	dfc.dTimeStamp = ::GetTickCount64();
	memcpy(m_SharedData.dfcDataFromCam, &dfc, BUF_SIZE);
	ReleaseSemaphore(m_SharedData.hReadHandle, 1, NULL);
}

void CBCRSDlg::SendSharedData(DataFromCam dfc)
{
	WaitForSingleObject(m_SharedData.hWriteHandle, INFINITE);
	memcpy(m_SharedData.dfcDataFromCam, &dfc, BUF_SIZE);
	ReleaseSemaphore(m_SharedData.hReadHandle, 1, NULL);
}

void CBCRSDlg::CheckMenuItem(UINT nMenuItemPos, UINT nItemFirst, UINT nItemLast, UINT nItemChecked)
{
	GetMenu()->GetSubMenu(nMenuItemPos)->CheckMenuRadioItem(nItemFirst, nItemLast, nItemChecked, MF_BYCOMMAND);
}

void CBCRSDlg::SwitchDisplayMode(int mode)
{
	// ---------------------------------------------------------------
	// @简述：在“双图像”模式下，运行该段程序，获取Display1,2相对位置
	// ---------------------------------------------------------------
#if DEBUG 0
	CRect rect1;
	GetDlgItem(IDC_DISPLAY1)->GetWindowRect(rect1);
	ScreenToClient(rect1);
	CRect rect2;
	GetDlgItem(IDC_DISPLAY2)->GetWindowRect(rect2);
	ScreenToClient(rect2);
	int nCrack = rect2.left - rect1.Width() - rect1.left;
#endif // DEBUG 0

	int nCrack = 5;
	// 单图像显示，Display1使能且扩展为原来的两倍，Display2去使能
	if (1 == mode)
	{
		// Display2不可用，不可见
		GetDlgItem(IDC_DISPLAY2)->EnableWindow(0);
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_HIDE);
		// 扩展Display1区域
		CRect rect;
		GetDlgItem(IDC_DISPLAY1)->GetWindowRect(rect);
		ScreenToClient(rect);
		if (rect.Height() > rect.Width())
			GetDlgItem(IDC_DISPLAY1)->MoveWindow(rect.left, rect.top, rect.Width() * 2.0 + nCrack, rect.Height(), true);
	}
	else if (2 == mode)
	{
		// 压缩Display1区域
		CRect rect;
		GetDlgItem(IDC_DISPLAY1)->GetWindowRect(rect);
		ScreenToClient(rect);
		if (rect.Height() < rect.Width())
			GetDlgItem(IDC_DISPLAY1)->MoveWindow(rect.left, rect.top, (rect.Width() - nCrack) / 2.0, rect.Height(), true);
		// Display2可用，可见
		GetDlgItem(IDC_DISPLAY2)->EnableWindow(1);
		GetDlgItem(IDC_DISPLAY2)->ShowWindow(SW_SHOW);
	}
	// 刷新窗口
	Invalidate();
}





void CBCRSDlg::OnTimer(UINT_PTR nIDEvent)
{
	// 帧数刷新
	if (nIDEvent == TIMER_DISPLAY_FRAME_COUNT)
	{
		DisplayFrameCount();
	}	
	// 物体检测

	if (nIDEvent == TIMER_TARGET_DETECTION)
	{
		if (!m_SignalFlag.bTargetFlag)
		{
			cvGpuMat TempMat;
			{
				lock_guard<mutex>guard(m_ImageCacheMutex);
				if (m_StereoParams.ImageCache[0].empty()) return;
				TempMat = m_StereoParams.ImageCache[0].clone();
				m_StereoParams.ImageCache[0].release();
			}
			bool bRet = TargetDetection(TempMat, m_TargetMonitor);
			// 上升沿检测
			if ((!m_SignalFlag.bTargetInFlag) && bRet)
			{
				m_SignalFlag.bTargetInFlag = true;
			}
			// 下降沿检测
			if (m_SignalFlag.bTargetInFlag && (!bRet))
			{
				m_SignalFlag.bTargetOutFlag = true;
			}
			// 切换到定位计算
			if (m_SignalFlag.bTargetInFlag && m_SignalFlag.bTargetOutFlag)
			{
				m_SignalFlag.bTargetFlag = true;
			}
		}
	}

	// 定位计算
	if (nIDEvent == TIMER_LOCATION_COMPUTED)
	{
		if (m_SignalFlag.bTargetFlag && m_SignalFlag.bTargetInFlag && m_SignalFlag.bTargetOutFlag)
		{
			cvGpuMat gImg1, gImg2;
			{// 判断数据非空
				lock_guard<mutex>guard(m_ImageCacheMutex);
				if (m_StereoParams.ImageCache[0].empty() || m_StereoParams.ImageCache[1].empty())return;
				gImg1 = m_StereoParams.ImageCache[0].clone();
				gImg2 = m_StereoParams.ImageCache[1].clone();
				m_StereoParams.ImageCache[0].release();
				m_StereoParams.ImageCache[1].release();
			}
			// 进行定位计算
			Pos pos = { 0 };
			if (!(gImg1.empty() || gImg2.empty()))
			{
				// 位姿计算
				pos = PositionAndPosture(gImg1, gImg2, m_StereoParams);
			}
			m_SignalFlag.bTargetFlag = false;
			m_SignalFlag.bTargetInFlag = false;
			m_SignalFlag.bTargetOutFlag = false;
		
			// 显示位姿
			CString info = TEXT("");
			info.Format(TEXT("%.3lf"), pos.x);
			GetDlgItem(IDC_EDT_POSX)->SetWindowTextW(info);
			info.Format(TEXT("%.3lf"), pos.y);
			GetDlgItem(IDC_EDT_POSY)->SetWindowTextW(info);
			info.Format(TEXT("%.3lf"), pos.z);
			GetDlgItem(IDC_EDT_POSZ)->SetWindowTextW(info);
			info.Format(TEXT("%.3lf"), pos.agl);
			GetDlgItem(IDC_EDT_AGL)->SetWindowTextW(info);

			// 发送位姿数据
			if (m_SignalFlag.bSendDataFlag)
			{
				DataFromCam dfc = { ::GetTickCount64() ,pos.x ,pos.y ,pos.z ,pos.agl };
				SendSharedData(dfc);
			}
		}
	}


	CDialogEx::OnTimer(nIDEvent);
}


void CBCRSDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 关闭软件之前先关闭相机
	OnBnClickedBtnClosecam();
	// 关闭软件之前回收内存
	for (int i = 0; i < m_MvDevList.nDeviceNum; i++)
	{
		delete(m_MvCam[i]);
	}
	CDialogEx::OnClose();
}
