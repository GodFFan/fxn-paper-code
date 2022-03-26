#pragma once


//摄像头的分辨率（图形尺寸：宽度）  
#define IMAGEWIDTH 612  
//摄像头的分辨率（图形尺寸：高度）  
#define IMAGEHEIGHT 512

//内参矩阵  MATLAB原生矩阵需要转置后使用
//fx  0    u0
//0   fy   v0
//0   0    1
#define CAMERAMATRIX57 (Mat_<double>(3, 3) << 1181.8615, 0.1267, 310.8560, 0.0000, 1181.7000, 254.2823, 0.0000, 0.0000, 1.0000)

//内参矩阵  MATLAB原生矩阵需要转置后使用
//fx  0    u0
//0   fy   v0
//0   0    1
#define CAMERAMATRIX82 (Mat_<double>(3, 3) << 1184.2741, 0.1041, 313.5232, 0.0000, 1184.3215, 269.3529, 0.0000, 0.0000, 1.0000)


//外参矩阵

//T平移矩阵 1相机为57 2相机为82
#define TRAN57_82 (Mat_<double>(3, 1) << 88.3276, 0.5856, 2.4209)
//R旋转矩阵 1相机为57 2相机为82 MATLAB原生矩阵需要转置后使用
#define ROT57_82 (Mat_<double>(3, 3) << 0.9997, 0.0025, -0.0233, -0.0024, 1.0000, 0.0042, 0.0233, -0.0042, 0.9997)


//相机畸变参数 K1 K2 P1 P2 K3
#define DISTORTION57 (Mat_<double>(4, 1) << -0.0234, 0.2453, 0.0002, 0.0000)
//相机畸变参数 K1 K2 P1 P2 K3
#define DISTORTION82 (Mat_<double>(4, 1) << -0.0195, -0.1229, -0.0002, 0.0008)

