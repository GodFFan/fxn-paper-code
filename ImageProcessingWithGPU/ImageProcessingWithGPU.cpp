// ImageProcessingWithGPU.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
// ImageProcessingWithCPU.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
// 第三方库包含
#include <opencv2/opencv.hpp> 
#include <opencv2/imgproc.hpp>
//#include <opencv2/imgproc/types_c.h>
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafilters.hpp>

// 参数矩阵，只能启用其中一个
#include "CameraParams_1_1.h"    // 由原始图像标定获得
//#include "CameraParams_1_2.h"    // 1/2图像长宽标定获得
//#include "CameraParams_1_4.h"    // 1/4图像长宽标定获得
//#include "CameraParams_1_8.h"    // 1/8图像长宽标定获得
constexpr double ZOOM = 1;// ZOOM = 1、2、4、8


using namespace cv;
using namespace std;
using namespace cv::cuda;
// ——————————————————————————
typedef struct
{
	Point2i cpt;
	int agl;
}CptAgl;



//显示图片
void DisplayImage(String winname, Mat img)
{
#if 1
	namedWindow(winname, WINDOW_NORMAL);
	imshow(winname, img);
#endif // 0
}
// 图像拼接
Mat ConcatImage(Mat& img1, Mat& img2)
{
	Mat retMat;
	// 图像拼接
	hconcat(img1, img2, retMat);
	// 绘制横线
	int nLineCount = 10;
	double nStepHeight = retMat.size().height / (double)nLineCount;
	for (int i = 1;i < nLineCount;i++)
	{
		line(retMat, Point(-1, i * nStepHeight),
			Point(retMat.size().width, i * nStepHeight), Scalar(0, 255, 0), 4, LINE_AA);
	}
	return Mat(retMat);
}
// 绘制第几个轮廓的特征
CptAgl DrawContours(Mat& img, int nID)
{
	CptAgl cvCptAgl = {};
	vector<vector<Point>> contours;
	findContours(img, contours, cv::CHAIN_APPROX_SIMPLE, CHAIN_APPROX_SIMPLE, Point());

	// 最小外接矩形求轮廓中心
	cv::RotatedRect rrect = cv::minAreaRect(contours[nID]);
	cvCptAgl.cpt = rrect.center;

	cvCptAgl.agl = rrect.angle;
	// 绘制最小矩形轮廓
	cv::Point2f points[4];
	rrect.points(points);
	for (int i = 0; i < 4; i++)
	{
		if (i < 3)
		{
			cv::line(img, points[i], points[i + 1], cv::Scalar(255, 255, 255), 2, cv::LINE_AA, 0);
		}
		else
		{
			cv::line(img, points[i], points[0], cv::Scalar(255, 255, 255), 2, cv::LINE_AA, 0);
		}

	}
	// 文字偏移位置
	int offsetX = -180, offsetY = -30;//原图
	//int offsetX = -90, offsetY = -15;//1/2
	//int offsetX = -45, offsetY = -8;//1/4
	//int offsetX = -30, offsetY = -5;//1/8

	// 角点坐标
	for (int i = 0; i < 4; i++)
	{
		int x = points[i].x, y = points[i].y;
		String str = "pt" + to_string(i + 1) + ":(" + to_string(x) + "," + to_string(y) + ")";
		putText(img, str, Point2f(points[i].x + offsetX, points[i].y + offsetY), 0, 1.5, Scalar(255, 255, 255), 3);
	}
	//中心点信息
	cv::circle(img, cvCptAgl.cpt, 10, Scalar(255, 255, 255), 2, cv::LINE_AA, 0);
	String strcpt = "cpt:(" + to_string(cvCptAgl.cpt.x) + "," + to_string(cvCptAgl.cpt.y) + ")";
	//String stragl = "agl:" + to_string(cvCptAgl.agl);
	putText(img, strcpt, Point2f(cvCptAgl.cpt.x + offsetX, cvCptAgl.cpt.y + offsetY), 0, 1.5, Scalar(255, 0, 0), 3);
	//putText(img, stragl, Point2f(cvCptAgl.cpt.x + offsetX, cvCptAgl.cpt.y + offsetY * 3), 0, 1.5, Scalar(255, 0, 0), 3);

	return CptAgl(cvCptAgl);
}










// ——————————————————————————

//摄像头的分辨率/图片尺寸
Size imageSize = Size(IMAGEWIDTH, IMAGEHEIGHT);
Mat cameraMatrix1 = CAMERAMATRIX57;
//对应Matlab所得1相机畸变参数K1K2P1P2K3
Mat distCoeffs1 = DISTORTION57;
//对应matlab里的2相机内参矩阵（需要转置）
Mat cameraMatrix2 = CAMERAMATRIX82;
//对应Matlab所得2相机畸变参数
Mat distCoeffs2 = DISTORTION82;
//R旋转矩阵
Mat R = ROT57_82;
//T平移矩阵
Mat T = TRAN57_82;
//映射表  
Mat map1x, map1y, map2x, map2y;
//校正旋转矩阵R，投影矩阵P 重投影矩阵Q
Mat R1, R2, P1, P2, Q;
//图像校正之后，会对图像进行裁剪，这里的validROI就是指裁剪之后的区域
Rect validROI1, validROI2;

int main()
{
	// 读取图片
	Mat imgL = imread(".//测试用例//左相机//CamL4.jpg");
	Mat imgR = imread(".//测试用例//右相机//CamR4.jpg");

	//// 显示原图像
	//DisplayImage("左相机：原始图像", imgL);
	//DisplayImage("右相机：原始图像", imgR);

	// 图像拼接并显示
	//DisplayImage("左右相机：原始图像", ConcatImage(imgL, imgR));

	// ——————————————————————————

	// 计算两相机校正后的旋转矩阵
	stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, imageSize, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY,//这里是CALIB_ZERO_DISPARITY 而不是0
		-1, imageSize, &validROI1, &validROI2);
	initUndistortRectifyMap(cameraMatrix1, distCoeffs1, R1, P1, imageSize, CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(cameraMatrix2, distCoeffs2, R2, P2, imageSize, CV_32FC1, map2x, map2y);
	
	Mat img = imgL.clone();
	GpuMat gImg, gMapx, gMapy;
	gMapx.upload(map1x);
	gMapy.upload(map1y);
	gImg.upload(img);
	// 从图像校正开始计时

	do
	{
		waitKey(1000);

	double timeZero = double(getTickCount());
	//------------------------------------------------------
	// 缩小图片尺寸
	GpuMat gResize;

	cv::cuda::resize(gImg, gResize, cv::Size(), 1,1);
	// 图形校正
	GpuMat gRemap;
	cv::cuda::remap(gResize, gRemap, gMapx, gMapy, cv::INTER_LINEAR);
	// 拷贝图像用于显示
	Mat bgImg, tempImg;
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
	GpuMat gGauss;
	// 高斯滤波器指针
	cv::Ptr<cv::cuda::Filter> pGaussianFilter;
	// 创建cuda高斯滤波器
	pGaussianFilter = cv::cuda::createGaussianFilter(CV_8UC1, CV_8UC1, cv::Size(5, 5), 3);
	// 执行高斯滤波器
	pGaussianFilter->apply(gRemap, gGauss);
	// ---------------------------------------------------------------
	// @简述：Canny算子提取轮廓
	// ---------------------------------------------------------------
	GpuMat gCanny;
	// 边缘检测指针
	cv::Ptr<cv::cuda::CannyEdgeDetector> pCannyDetector;
	// 创建边缘检测器
	pCannyDetector = cv::cuda::createCannyEdgeDetector(70, 180, 3, false);
	// 执行边缘检测器
	pCannyDetector->detect(gGauss, gCanny);
	// ---------------------------------------------------------------
	// @简述：形态学处理
	// ---------------------------------------------------------------
	GpuMat gDilate;
	// 构建矩形膨胀内核
	Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	// 形态滤波器指针
	cv::Ptr<cv::cuda::Filter> pMorphologyFilter;
	// 创建膨胀运算滤波器
	pMorphologyFilter = cv::cuda::createMorphologyFilter(cv::MORPH_DILATE, CV_8UC1, kernel);
	// 执行形态滤波器
	pMorphologyFilter->apply(gCanny, gDilate);
	// ---------------------------------------------------------------
	// @简述：查找ROI区域内物体轮廓
	// ---------------------------------------------------------------
	// 预处理后的图像
	Mat ImagePreprocess;
	gDilate.download(ImagePreprocess);
	//cv::imwrite("预处理后的图像.jpg", ImagePreprocess);// 输出测试

	// 轮廓查找
	vector<vector<Point>> contours;
	cv::findContours(ImagePreprocess, contours, 0, cv::CHAIN_APPROX_SIMPLE);


	cout << "Time: " << ((double)getTickCount() - timeZero) / getTickFrequency() * 1000 << "ms" << endl;

	} while (true);
#if 1



#endif // 0
	waitKey(0);
	return 0;
}



