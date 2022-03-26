#pragma once

//����ͷ�ķֱ��ʣ�ͼ�γߴ磺���ȣ�  
#define IMAGEWIDTH 1224
//����ͷ�ķֱ��ʣ�ͼ�γߴ磺�߶ȣ�  
#define IMAGEHEIGHT 1024

//�ڲξ���  MATLABԭ��������Ҫת�ú�ʹ��
//fx  0    u0
//0   fy   v0
//0   0    1
#define CAMERAMATRIX57 (Mat_<double>(3, 3) << 2367.2504, 0.1487, 620.5070, 0.0000, 2366.9181, 507.8949, 0.0000, 0.0000, 1.0000)

//�ڲξ���  MATLABԭ��������Ҫת�ú�ʹ��
//fx  0    u0
//0   fy   v0
//0   0    1
#define CAMERAMATRIX82 (Mat_<double>(3, 3) << 2367.8041, 0.1505, 626.2855, 0.0000, 2367.8523, 537.1130, 0.0000, 0.0000, 1.0000)

//��ξ���

//Tƽ�ƾ��� 1���Ϊ57 2���Ϊ82
#define TRAN57_82 (Mat_<double>(3, 1) << 88.2220, 0.5856, 0.9108)
//R��ת���� 1���Ϊ57 2���Ϊ82 MATLABԭ��������Ҫת�ú�ʹ��
#define ROT57_82 (Mat_<double>(3, 3) << 0.9997, 0.0024, -0.0233, -0.0023, 1.0000, 0.0046, 0.0233, -0.0046, 0.9997)


//���������� K1 K2 P1 P2 K3
#define DISTORTION57 (Mat_<double>(5, 1) << -0.0229, 0.2483, 0.0001, -0.0001, 1.4929)
//���������� K1 K2 P1 P2 K3
#define DISTORTION82 (Mat_<double>(5, 1) << -0.0247, 0.2022, -0.0003, 0.0005, 1.5762)
