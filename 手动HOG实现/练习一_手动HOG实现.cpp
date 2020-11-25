#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
//步骤 1.计算各个像素的梯度强度和方向 2.角度方向量化 3.图像分割成cell,按cell分别计算直方图 
//4.组合成总的直方图 5.直方图之间的距离计算

int cellsize = 16;//每个cell的大小
int nAngle = 8;//角度8等分量化
int main()
{
	cv::Mat refMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\hogTemplate.jpg",0);
	cv::Mat plfMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\img1.jpg",0);
	cv::Mat bgfMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\img2.jpg",0);


}

Mat calcHog(Mat refMat,Mat hist,int nAngle,int blocksize)
{
	cv::Mat gx, gy;
	cv::Mat mag, angle;
	//计算梯度
	Sobel(refMat, gx, CV_32F, 1, 0, 1);
	Sobel(refMat, gy, CV_32F, 0, 1, 1);

	//x方向梯度，y方向梯度，梯度，角度，决定输出弧度/角度
	cartToPolar(gx, gy, mag, angle, true);

	//nY表示纵向cell个数，nX表示横向cell个数
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

			//赋值图像,处理每个cell
			roiMat = refMat(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);

			//当前cell第一个元素在数组中的位置
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


