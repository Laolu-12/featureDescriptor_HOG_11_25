#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
//���� 1.����������ص��ݶ�ǿ�Ⱥͷ��� 2.�Ƕȷ������� 3.ͼ��ָ��cell,��cell�ֱ����ֱ��ͼ 
//4.��ϳ��ܵ�ֱ��ͼ 5.ֱ��ͼ֮��ľ������

int cellsize = 16;//ÿ��cell�Ĵ�С
int nAngle = 8;//�Ƕ�8�ȷ�����
int main()
{
	cv::Mat refMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\hogTemplate.jpg",0);
	cv::Mat plfMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img1.jpg",0);
	cv::Mat bgfMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img2.jpg",0);


}

Mat calcHog(Mat refMat,Mat hist,int nAngle,int blocksize)
{
	cv::Mat gx, gy;
	cv::Mat mag, angle;
	//�����ݶ�
	Sobel(refMat, gx, CV_32F, 1, 0, 1);
	Sobel(refMat, gy, CV_32F, 0, 1, 1);

	//x�����ݶȣ�y�����ݶȣ��ݶȣ��Ƕȣ������������/�Ƕ�
	cartToPolar(gx, gy, mag, angle, true);

	//nY��ʾ����cell������nX��ʾ����cell����
	int nX = refMat.cols / cellsize;
	int nY = refMat.rows / cellsize;
	
	for (int i = 0; i < nY; i++)
	{
		for (int j = 0; j < nX; j++)
		{
			cv::Mat roiMat;
			cv::Mat roiMag;
			cv::Mat roiAgl;
			Rect roi = Rect(j*cellsize, i*cellsize, cellsize, cellsize);

			roi.x = j*cellsize;
			roi.y = i*cellsize;

			//��ֵͼ��,����ÿ��cell
			roiMat = refMat(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);

			//��ǰcell��һ��Ԫ���������е�λ��
			int head = (i*nX + j)*nAngle;

			
			for (int n = 0; n < roiMat.rows; n++)
			{
				for (int m = 0; m < roiMat.cols; m++)
				{

				}
			}


		}
	}
}


