
// BCRSDlg.h: 头文件
//

#pragma once
#include "MvCamera.h"
#include <atomic>
#include <mutex>



using namespace std;
// CBCRSDlg 对话框
class CBCRSDlg : public CDialogEx
{
// 构造
public:
	CBCRSDlg(CWnd* pParent = nullptr);	// 标准构造函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BCRS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual void PreInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMenuCommand(UINT nID);

public:
	// 按钮事件
	afx_msg void OnBnClickedBtnInitcam();
	afx_msg void OnBnClickedBtnCapture();
	afx_msg void OnBnClickedBtnSaveimg();
	afx_msg void OnBnClickedBtnMeasure();
	afx_msg void OnBnClickedBtnSenddata();
	afx_msg void OnBnClickedBtnClosecam();
	afx_msg void OnBnClickedBtnZero();

public: 
	void EnumDevices();		 // 枚举设备
	void OpenDevice();		 // 打开设备
	void CloseDevice();		 // 关闭设备
	void StartGrabbing();	 // 开始抓图
	void StopGrabbing();	 // 结束抓图
	void SoftwareTriggersOnce(); // 软件触发采集一次 
	void InitDisplayHand();      // 初始化显示句柄
	void DisplayFrameCount();    // 帧数显示
	void SetExposure(); // 设置曝光
	void SetGain();     // 设置增益
	void SetFPS();      // 设置帧率
	void SetTrigger();  // 设置触发

	static UINT GetOneFrameFromVideoThread(LPVOID pParam);			 // 视频流取帧线程
	void GetAndDisplayOneFrameFromVideo(int nDeviceNum);			 // 视频流取帧并显示
protected:
	// --------------------------------------------------------------- //
	//                       海康威视相机控制                          //
	// --------------------------------------------------------------- //
	MV_CC_DEVICE_INFO_LIST m_MvDevList;							     // 设备信息列表
	CMvCamera* m_MvCam[MAX_DEVICE_NUM];								 // 海康相机SDK实例
	HWND m_HwndDisplay[MAX_DEVICE_NUM];								 // 图像显示句柄
	atomic<int> m_nCurrentDevicesNum;						         // 当前设备号
	FrameCount m_FrameCount[MAX_DEVICE_NUM];						 // 帧数统计
	SignalFlag m_SignalFlag;                                         // Flag

	// --------------------------------------------------------------- //
	//                         OpenCV图像处理                          //
	// --------------------------------------------------------------- //

public:
	void InitStereoParams();                                         // 初始化相机参数
	void RectifyMapping();                                           // 极线匹配映射表
	// 彩色图片RGB转成BGR
	void RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);
	// 图像转成cv::Mat类型数据
	bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char* pData, int nCamNum);
	void InitMonitor(int nMode = 0);                                 // 初始化监视器
	bool TargetDetection(cvGpuMat curImg, TargetMonitor tm);         // 目标检测
	// 图像处理查找轮廓
	CptAgl FindContours(cvGpuMat& gImg, int nImgID, cvGpuMat gMapx, cvGpuMat gMapy, cvMat Roi);
	// 计算位姿信息
	Pos PositionAndPosture(cvGpuMat& img1, cvGpuMat& img2, StereoParams sp);
	// 在图片控件中显示图像
	void ShowImageInPictureControl(cvMat& mat, int nID);

protected:
	mutex m_ImageCacheMutex;
	StereoParams m_StereoParams;                                     // 双目相机参数
	TargetMonitor m_TargetMonitor;                                   // 目标监控

	// --------------------------------------------------------------- //
	//                         相机发送数据                            //
	// --------------------------------------------------------------- //
public:
	void InitSharedData(int nMode = 0);
	void CreateFileMap();
	void CloseFileMap();
	void SendSharedData();
	void SendSharedData(DataFromCam dfc);
protected:
	SharedData m_SharedData;

public:
	// 勾选菜单项目
	void CheckMenuItem(UINT nMenuItemPos, UINT nItemFirst, UINT nItemLast, UINT nItemChecked);
	// 切换单双图像显示，默认显示双图像
	void SwitchDisplayMode(int mode = 2);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};
