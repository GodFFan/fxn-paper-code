// BCRS_type.h: 头文件
// 双目相机定位软件数据类型定义

#ifndef _BCRS_TYPE_H_
#define _BCRS_TYPE_H_

#include <vector>

// 计时器
#define TIMER_DISPLAY_FRAME_COUNT       1        // 计时器：刷新帧数显示
#define TIMER_TARGET_DETECTION          2        // 计时器：目标检测
#define TIMER_LOCATION_COMPUTED         3        // 计时器：定位计算

// 菜单标签

#define MENU_CAM_PARAM         0     // 相机参数
#define MENU_CAM_CALIBRATE     2     // 相机标定
#define MENU_IMG_CAPTURE       4     // 采集模式
#define MENU_IMG_FORMAT        6     // 图片格式
#define MENU_SET_DISPLAY       8     // 显示设置

// 配置/数据文件路径
#define PATH_BUTTON_LOG        TEXT(".//dat//BCRSBtnLog.ini")            // 定义按钮日志，记录按钮状态
#define PATH_SYSTEM_CONFIG     TEXT(".//dat//BCRSSystemConfig.ini")      // 定义系统配置信息

// 定义数据

#define MAX_DEVICE_NUM          2     // 最大设备号
#define IMAGE_NAME_LEN          256   // 图片名称长度
#define IMAGE_WIDTH             2448  // 图像宽度
#define IMAGE_HEIGHT            2048  // 图像高度
#define BUF_SIZE                1024  // 共享内存大小


// 触发模式
#define TRIGGER_OFF             0     // 关闭信号触发，启用连续采集
#define TRIGGER_ON              1     // 启用信号触发
#define TRIGGER_NOT_SET         (-1)  // 触发模式未设置

// 触发源
#define SOFTWARE                7     // 软件触发
#define HARDWARE                0     // 硬件触发

// 定义数据类型




typedef struct           // 帧数统计
{
	long nFrameCount;    // 帧数
	long nLostFrame;     // 丢帧数
}FrameCount;

typedef struct           // Flag
{
	bool bGrabbingFlag;  // 开始采集：true 停止采集：false
	bool bMeasureFlag;   // 开始测距：true 停止测距：false
	bool bSendDataFlag;  // 开始发送：true 停止发送：false
	bool bTargetFlag;    // 目标出现：ture 目标未出：false
	bool bTargetInFlag;  // 目标进入：ture 目标未进：false
	bool bTargetOutFlag; // 目标出来：ture 目标未出：false
}SignalFlag;

typedef cv::Mat cvMat;
typedef cv::Size cvSize;
typedef cv::Rect cvRect;
typedef cv::Rect2d cvRect2d;
typedef cv::Point cvPoint;
typedef cv::Point2d cvPoint2d;
typedef cv::Scalar cvScalar;
typedef cv::cuda::GpuMat cvGpuMat;


typedef struct           // 行列号
{
	int nRow;
	int nCol;
}RowCol;


typedef struct           // 双目相机参数
{
public:
	cvMat IntrinsicMatrixCam1;
	cvMat IntrinsicMatrixCam2;
	cvMat RotationMatrixOfCam2;
	cvMat TranslationMatrixOfCam2;
	cvMat DistortionMatrixCam1;
	cvMat DistortionMatrixCam2;
public:
	cvMat RoiCam1;
	cvMat RoiCam2;
	cvGpuMat RectifyMapCam1[2];
	cvGpuMat RectifyMapCam2[2];
	cvGpuMat ImageCache[MAX_DEVICE_NUM];
public:
	/// <summary>
	/// 使用double型数据初始化Mat
	/// </summary>
	/// <param name="cvOutputArray">StereoParams中cvMat类型变量</param>
	/// <param name="cvSize">二维数组行(高度)和列(宽度)</param>
	/// <param name="lpAppName"></param>
	/// <param name="vsKeyName"></param>
	/// <param name="lpFileName"></param>
	static void InitMatWithDouble(cvMat &cvOutputArray, cvSize cvSize, LPCWSTR lpAppName, VCTSTR vsKeyName, LPCWSTR lpFileName)
	{
		cvOutputArray.create(cvSize, CV_64FC1);
		CString cstrKeyValue;
		for (int i = 0, j = 0, k = 0; i < vsKeyName.size(); k++,i++)
		{
			ReadIni(lpAppName, vsKeyName.at(i), cstrKeyValue, lpFileName);
			if (k < cvSize.width)
			{
				cvOutputArray.at<double>(j, k) = _wtof(cstrKeyValue);
			}
			else
			{
				j++;k = 0;
				cvOutputArray.at<double>(j, k) = _wtof(cstrKeyValue);
			}
		}
	};
	static void InitMatWithInt(cvMat& cvOutputArray, cvSize cvSize, LPCWSTR lpAppName, VCTSTR vsKeyName, LPCWSTR lpFileName)
	{
		cvOutputArray.create(cvSize, CV_32SC1);
		CString cstrKeyValue;
		for (int i = 0, j = 0, k = 0; i < vsKeyName.size(); k++,i++)
		{
			ReadIni(lpAppName, vsKeyName.at(i), cstrKeyValue, lpFileName);
			if (k < cvSize.width)
			{
				cvOutputArray.at<int>(j, k) = _wtoi(cstrKeyValue);
			}
			else
			{
				j++;k = 0;
				cvOutputArray.at<int>(j, k) = _wtoi(cstrKeyValue);
			}
		}
	};
}StereoParams;

typedef struct           // 目标监控
{
	cvRect MonitorArea;				// 监控区域
	//cvScalar MeanThreshold;         // 均值阈值
	//cvScalar StdDevThreshold;       // 标准差阈值
	cvGpuMat BackGroundImage;		// 背景模板
}TargetMonitor;

typedef struct           // 中心坐标和角度
{
	cvPoint2d cpt;				// 中心坐标
	double    agl;				// 旋转角度
}CptAgl;

typedef struct           // 物体位姿
{
	double    x;				// x
	double    y;				// y
	double    z;				// z
	double    agl;				// deg
}Position, Pos;

typedef struct
{
public:
	DWORD64 dTimeStamp;//时间戳
	double dPosX;//x
	double dPosY;//z
	double dPosZ;//y
	double dAgl;//角度，unit:deg
}DataFromCam;

typedef struct
{
	CString cstrSharedMemoryName;		// 共享内存名字
	CString cstrReadSemaphoreName;		// 读取信号量名字
	CString cstrWriteSemaphoreName;		// 写入信号量名字
	HANDLE hSharedMemoryHandle;			// 共享内存句柄
	HANDLE hReadHandle;					// 读取句柄
	HANDLE hWriteHandle;				// 写入句柄
	DataFromCam* dfcDataFromCam;        // 位姿
}SharedData;









#endif // !1

