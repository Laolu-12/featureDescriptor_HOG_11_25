#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
//���� 1.����������ص��ݶ�ǿ�Ⱥͷ��� 2.�Ƕȷ������� 3.ͼ��ָ��cell,��cell�ֱ����ֱ��ͼ 
//4.��ϳ��ܵ�ֱ��ͼ 5.ֱ��ͼ֮��ľ������

int cellsize = 16;//ÿ��cell�Ĵ�С
int nAngle = 8;//�Ƕ�8�ȷ�����

//��������
int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSize);
float normL2(float * Hist1, float * Hist2, int siz);
int main()
{
	cv::Mat refMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\hogTemplate.jpg",0);
	cv::Mat plfMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img1.jpg",0);
	cv::Mat bgfMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img2.jpg",0);

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
	//������������ͼƬ��HOG
	reCode = calcHOG(refMat, ref_hist, nAngle, blockSize);
	reCode = calcHOG(plfMat, pl_hist, nAngle, blockSize);
	reCode = calcHOG(bgfMat, bg_hist, nAngle, blockSize);

	if (reCode != 0) {
		return -1;
	}

	//����ֱ��ͼ����
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
	//�����ݶ�
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);

	//x�����ݶȣ�y�����ݶȣ��ݶȣ��Ƕȣ������������/�Ƕ�
	cartToPolar(gx, gy, mag, angle, true);

	//nY��ʾ����cell������nX��ʾ����cell����
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

			//��ֵͼ��,����ÿ��cell
			roiMat = src(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);

			//��ǰcell��һ��Ԫ���������е�λ��
			int head = (i*nX + j)*nAngle;

			
			for (int n = 0; n < roiMat.rows; n++)
			{
				for (int m = 0; m < roiMat.cols; m++)
				{
					//����Ƕ����ĸ�bin��ͨ��int�Զ�ȡ��ʵ��
					int pos = (int)(roiAgl.at<float>(n, m) / binAngle);
					//�����ص��ֵΪȨ��
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


