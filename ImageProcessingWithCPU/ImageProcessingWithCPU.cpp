// ImageProcessingWithCPU.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
// 参数矩阵，只能启用其中一个
//#include "CameraParams_1_1.h"    // 由原始图像标定获得
#include "CameraParams_1_2.h"    // 1/2图像长宽标定获得
//#include "CameraParams_1_4.h"    // 1/4图像长宽标定获得
//#include "CameraParams_1_8.h"    // 1/8图像长宽标定获得
constexpr double ZOOM = 2 ;// ZOOM = 1、2、4、8


using namespace cv;
using namespace std;

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
#if 1

	// 从图像校正开始计时
	double timeZero = double(getTickCount());

	// 修改图像尺寸
	resize(imgL, imgL, Size(), 1 / ZOOM, 1 / ZOOM);
	//resize(imgR, imgR, Size(), 1 / ZOOM, 1 / ZOOM);

	// remap后，两相机图像行对齐
	remap(imgL, imgL, map1x, map1y, INTER_LINEAR);
	//remap(imgR, imgR, map2x, map2y, INTER_LINEAR);

	// 显示校正后的图像
	//DisplayImage("左相机：校正后", imgL);
	//DisplayImage("右相机：校正后", imgR);

	// 图像拼接并显示
	//DisplayImage("左右相机：校正后图像", ConcatImage(imgL, imgR));

	// ——————————————————————————
	// 图像处理，以左图为例
	Mat img = imgL.clone();
	// 转为灰度图
	cvtColor(img, img, cv::COLOR_BGR2GRAY);
	//DisplayImage("高斯滤波前", img);
	// 高斯滤波
	GaussianBlur(img, img, Size(5, 5), 3, 3);
	//DisplayImage("高斯滤波后", img);

	// canny算子提取轮廓
	Canny(img, img, 70, 180, 3, false);
	//DisplayImage("Canny边缘检测（未高斯滤波）", img);

	// 形态学处理，膨胀运算
	Mat kernel = getStructuringElement(cv::MORPH_RECT, Size(3, 3));
	dilate(img, img, kernel);
	//DisplayImage("形态学处理", img);

	// 轮廓查找
	// 原图(左:11  右:6)
	// 1/2(左:5  右:5)
	// 1/4(左:3  右:1)
	// 1/8(左:1  右:1)
	DrawContours(img, 11);
	//DisplayImage("轮廓特征", img);
	 //位姿计算

	cout << "Time: " << ((double)getTickCount() - timeZero) / getTickFrequency() * 1000 << "ms" << endl;
	////////////////////////////////////////////////////
	// 测试数据
#endif
	 // 原图像左右相机成像点坐标
	//Point2d ptL(1116, 1149), ptR(1643, 1149);//cpt
	//Point2d ptL(1262, 1560), ptR(1789, 1560);//pt1
	//Point2d ptL(790, 1440), ptR(1318, 1441);//pt2
	//Point2d ptL(968, 737), ptR(1496, 737);//pt3
	//Point2d ptL(1440, 856), ptR(1968, 857);//pt4

	 // 1/2图像左右相机成像点坐标
	//Point2d ptL(557, 574), ptR(821, 574);//cpt
	//Point2d ptL(631, 782), ptR(895, 781);//pt1  xc:-3.50675 yc:-53.9784 zc : -791.134
	//Point2d ptL(721, 426), ptR(985, 426);//pt4  xc:-33.5847 yc:65.0133  zc : -791.134

	// 1/4图像左右相机成像点坐标
	//Point2d ptL(277, 287), ptR(410, 287);//cpt
	//Point2d ptL(313, 388), ptR(445, 388);//pt1  xc:-1.43522 yc:-51.6484 zc : -791.155
	//Point2d ptL(357, 214), ptR(489, 214);//pt4  xc:-30.8895 yc:64.8457  zc : -791.155

	// 1/8图像左右相机成像点坐标
	//Point2d ptL(141, 144), ptR(207, 144);//cpt
	//Point2d ptL(158, 193), ptR(224, 193);//pt1  xc:-2.95743 yc:-49.6839 zc : -793.385
	//Point2d ptL(180, 107), ptR(246, 107);//pt4  xc:-32.3222 yc:65.1282  zc : -793.385
#if 0



	// 定位计算
	double fx = cameraMatrix1.at<double>(0, 0);
	double fy = cameraMatrix1.at<double>(1, 1);
	double u0 = cameraMatrix1.at<double>(0, 2);
	double v0 = cameraMatrix1.at<double>(1, 2);
	double ul = ptL.x, ur = ptR.x;
	double vl = ptL.y, vr = ptR.y;
	double d = ul - ur;
	double b = sqrt(pow(T.at<double>(0, 0), 2) + pow(T.at<double>(1, 0), 2) + pow(T.at<double>(2, 0), 2));

	double zc = b * fx / d;
	double xc = zc * (ul - u0) / fx;
	double yc = zc * (vl - u0) / fy;

	cout << "xc:" << xc << endl
		<< "yc:" << yc << endl
		<< "zc:" << zc << endl;
#endif // 0
	waitKey(0);
	return 0;
}



