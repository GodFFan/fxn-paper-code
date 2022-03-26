// BCRS_type.h: ͷ�ļ�
// ˫Ŀ�����λ����������Ͷ���

#ifndef _BCRS_TYPE_H_
#define _BCRS_TYPE_H_

#include <vector>

// ��ʱ��
#define TIMER_DISPLAY_FRAME_COUNT       1        // ��ʱ����ˢ��֡����ʾ
#define TIMER_TARGET_DETECTION          2        // ��ʱ����Ŀ����
#define TIMER_LOCATION_COMPUTED         3        // ��ʱ������λ����

// �˵���ǩ

#define MENU_CAM_PARAM         0     // �������
#define MENU_CAM_CALIBRATE     2     // ����궨
#define MENU_IMG_CAPTURE       4     // �ɼ�ģʽ
#define MENU_IMG_FORMAT        6     // ͼƬ��ʽ
#define MENU_SET_DISPLAY       8     // ��ʾ����

// ����/�����ļ�·��
#define PATH_BUTTON_LOG        TEXT(".//dat//BCRSBtnLog.ini")            // ���尴ť��־����¼��ť״̬
#define PATH_SYSTEM_CONFIG     TEXT(".//dat//BCRSSystemConfig.ini")      // ����ϵͳ������Ϣ

// ��������

#define MAX_DEVICE_NUM          2     // ����豸��
#define IMAGE_NAME_LEN          256   // ͼƬ���Ƴ���
#define IMAGE_WIDTH             2448  // ͼ����
#define IMAGE_HEIGHT            2048  // ͼ��߶�
#define BUF_SIZE                1024  // �����ڴ��С


// ����ģʽ
#define TRIGGER_OFF             0     // �ر��źŴ��������������ɼ�
#define TRIGGER_ON              1     // �����źŴ���
#define TRIGGER_NOT_SET         (-1)  // ����ģʽδ����

// ����Դ
#define SOFTWARE                7     // �������
#define HARDWARE                0     // Ӳ������

// ������������




typedef struct           // ֡��ͳ��
{
	long nFrameCount;    // ֡��
	long nLostFrame;     // ��֡��
}FrameCount;

typedef struct           // Flag
{
	bool bGrabbingFlag;  // ��ʼ�ɼ���true ֹͣ�ɼ���false
	bool bMeasureFlag;   // ��ʼ��ࣺtrue ֹͣ��ࣺfalse
	bool bSendDataFlag;  // ��ʼ���ͣ�true ֹͣ���ͣ�false
	bool bTargetFlag;    // Ŀ����֣�ture Ŀ��δ����false
	bool bTargetInFlag;  // Ŀ����룺ture Ŀ��δ����false
	bool bTargetOutFlag; // Ŀ�������ture Ŀ��δ����false
}SignalFlag;

typedef cv::Mat cvMat;
typedef cv::Size cvSize;
typedef cv::Rect cvRect;
typedef cv::Rect2d cvRect2d;
typedef cv::Point cvPoint;
typedef cv::Point2d cvPoint2d;
typedef cv::Scalar cvScalar;
typedef cv::cuda::GpuMat cvGpuMat;


typedef struct           // ���к�
{
	int nRow;
	int nCol;
}RowCol;


typedef struct           // ˫Ŀ�������
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
	/// ʹ��double�����ݳ�ʼ��Mat
	/// </summary>
	/// <param name="cvOutputArray">StereoParams��cvMat���ͱ���</param>
	/// <param name="cvSize">��ά������(�߶�)����(���)</param>
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

typedef struct           // Ŀ����
{
	cvRect MonitorArea;				// �������
	//cvScalar MeanThreshold;         // ��ֵ��ֵ
	//cvScalar StdDevThreshold;       // ��׼����ֵ
	cvGpuMat BackGroundImage;		// ����ģ��
}TargetMonitor;

typedef struct           // ��������ͽǶ�
{
	cvPoint2d cpt;				// ��������
	double    agl;				// ��ת�Ƕ�
}CptAgl;

typedef struct           // ����λ��
{
	double    x;				// x
	double    y;				// y
	double    z;				// z
	double    agl;				// deg
}Position, Pos;

typedef struct
{
public:
	DWORD64 dTimeStamp;//ʱ���
	double dPosX;//x
	double dPosY;//z
	double dPosZ;//y
	double dAgl;//�Ƕȣ�unit:deg
}DataFromCam;

typedef struct
{
	CString cstrSharedMemoryName;		// �����ڴ�����
	CString cstrReadSemaphoreName;		// ��ȡ�ź�������
	CString cstrWriteSemaphoreName;		// д���ź�������
	HANDLE hSharedMemoryHandle;			// �����ڴ���
	HANDLE hReadHandle;					// ��ȡ���
	HANDLE hWriteHandle;				// д����
	DataFromCam* dfcDataFromCam;        // λ��
}SharedData;









#endif // !1

