#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
//步骤 1.计算各个像素的梯度强度和方向 2.角度方向量化 3.图像分割成cell,按cell分别计算直方图 
//4.组合成总的直方图 5.直方图之间的距离计算

int cellsize = 16;//每个cell的大小
int nAngle = 8;//角度8等分量化

//函数声明
int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSize);
float normL2(float * Hist1, float * Hist2, int siz);
int main()
{
	cv::Mat refMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\hogTemplate.jpg",0);
	cv::Mat plfMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\img1.jpg",0);
	cv::Mat bgfMat = imread("C:\\Users\\Lenovo\\Pictures\\数图\\img2.jpg",0);

	int nAngle = 8;
	int blockSize = 16;
	int nX = refMat.cols / blockSize;
	int nY = refMat.rows / blockSize;

	int bins = nX*nY*nAngle;


	float * ref_hist = new float[bins];
	memset(ref_hist, 0, sizeof(float)*bins);
	float * pl_hist = new float[bins];
	memset(pl_hist, 0, sizeof(float)*bins);
	float * bg_hist = new float[bins];
	memset(bg_hist, 0, sizeof(float)*bins);

	int reCode = 0;
	//计算三张输入图片的HOG
	reCode = calcHOG(refMat, ref_hist, nAngle, blockSize);
	reCode = calcHOG(plfMat, pl_hist, nAngle, blockSize);
	reCode = calcHOG(bgfMat, bg_hist, nAngle, blockSize);

	if (reCode != 0) {
		return -1;
	}

	//计算直方图距离
	float dis1 = normL2(ref_hist, pl_hist, bins);
	float dis2 = normL2(ref_hist, bg_hist, bins);

	std::cout << "distance between reference and img1:" << dis1 << std::endl;
	std::cout << "distance between reference and img2:" << dis2 << std::endl;

	(dis1 <= dis2) ? (std::cout << "img1 is similar" << std::endl) : (std::cout << "img2 is similar" << std::endl);


	imshow("ref", refMat);
	imshow("img1", plfMat);
	imshow("img2", bgfMat);

	waitKey(0);

	delete[] ref_hist;
	delete[] pl_hist;
	delete[] bg_hist;
	destroyAllWindows();

	return 0;


}

int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSiz)
{
	cv::Mat gx, gy;
	cv::Mat mag, angle;
	//计算梯度
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);

	//x方向梯度，y方向梯度，梯度，角度，决定输出弧度/角度
	cartToPolar(gx, gy, mag, angle, true);

	//nY表示纵向cell个数，nX表示横向cell个数
	int nX = src.cols / cellsize;
	int nY = src.rows / cellsize;

	int binAngle = 360 / nAngle;
	
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
			roiMat = src(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);

			//当前cell第一个元素在数组中的位置
			int head = (i*nX + j)*nAngle;

			
			for (int n = 0; n < roiMat.rows; n++)
			{
				for (int m = 0; m < roiMat.cols; m++)
				{
					//计算角度在哪个bin，通过int自动取整实现
					int pos = (int)(roiAgl.at<float>(n, m) / binAngle);
					//以像素点的值为权重
					hist[head + pos] += roiMag.at<float>(n, m);
				}
			}


		}
	}
	return 0;
}

float normL2(float * Hist1, float * Hist2, int size)
{
	float sum = 0;
	for (int i = 0; i < size; i++) {
		sum += (Hist1[i] - Hist2[i])*(Hist1[i] - Hist2[i]);
	}
	sum = sqrt(sum);
	return sum;
}


