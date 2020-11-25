#include<opencv2/opencv.hpp>
#include<iostream>

#include <math.h>

using namespace cv;
using namespace std;


/**********************�ֶ�ʵ�֣�ͨ�� HOG (Histogram-of-Oriented-Gradients)�Ƚ�ͼ�����ƶ�*************************/

//��������
int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSize);
float normL2(float * Hist1, float * Hist2, int siz);

int main()
{

	//����ͼ��
	cv::Mat refMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\hogTemplate.jpg", 0);
	cv::Mat plMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img1.jpg", 0);
	cv::Mat bgMat = imread("C:\\Users\\Lenovo\\Pictures\\��ͼ\\img2.jpg", 0);

	if (refMat.empty() || plMat.empty() || bgMat.empty()) {
		std::cout << "failed to read image!:" << std::endl;
		return -1;
	}

	//��������
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
	reCode = calcHOG(plMat, pl_hist, nAngle, blockSize);
	reCode = calcHOG(bgMat, bg_hist, nAngle, blockSize);

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
	imshow("img1", plMat);
	imshow("img2", bgMat);

	waitKey(0);

	delete[] ref_hist;
	delete[] pl_hist;
	delete[] bg_hist;
	destroyAllWindows();

	return 0;
}

//�ֶ�ʵ�� HOG (Histogram-of-Oriented-Gradients) 
int calcHOG(cv::Mat src, float * hist, int nAngle, int cellSize)
{

	if (cellSize> src.cols || cellSize> src.rows) {
		return -1;
	}

	//��������
	int nX = src.cols / cellSize;
	int nY = src.rows / cellSize;

	int binAngle = 360 / nAngle;

	//�����ݶȼ��Ƕ�
	Mat gx, gy;
	Mat mag, angle;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	// x�����ݶȣ�y�����ݶȣ��ݶȣ��Ƕȣ������������or�Ƕ�
	// Ĭ���ǻ���radians��ͨ�����һ����������ѡ��Ƕ�degrees.

	/************************************************
	cartToPolar���� ����ģ�ͣ�
	cartToPolar(InputArray x, InputArray y,
	OutputArray magnitude, OutputArray angle,
	bool angleInDegrees = false);
	�������ܣ�
	. InputArray x/InputArray y�������ݶ�x/y
	.  OutputArray magnitude�������ֵ
	. OutputArray angle������Ƕ�

	**************************************************/
	cartToPolar(gx, gy, mag, angle, true);

	cv::Rect roi;
	roi.x = 0;
	roi.y = 0;
	roi.width = cellSize;
	roi.height = cellSize;

	for (int i = 0; i < nY; i++) {
		for (int j = 0; j < nX; j++) {

			cv::Mat roiMat;
			cv::Mat roiMag;
			cv::Mat roiAgl;

			roi.x = j*cellSize;
			roi.y = i*cellSize;

			//��ֵͼ��
			roiMat = src(roi);
			roiMag = mag(roi);
			roiAgl = angle(roi);

			//��ǰcell��һ��Ԫ���������е�λ��
			int head = (i*nX + j)*nAngle;

			for (int n = 0; n < roiMat.rows; n++) {
				for (int m = 0; m < roiMat.cols; m++) {
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



int detectPeople()
{

	VideoCapture cap("../testImages\\vtest.avi");

	//�����Ƶ��ʧ��
	if (!cap.isOpened()) {
		std::cout << "Unable to open video!" << std::endl;
		return -1;
	}

	Mat frame;

	//����HOG������
	HOGDescriptor hog;
	//ΪSVM����������Ĭ�ϲ���
	//getDefaultPeopleDetector() �������˼���Ĭ�ϲ�����(64x128 windows).
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	while (1) {

		cap >> frame;

		std::vector<cv::Rect> found;

		//������
		//0.����ͼ��
		//1.����������(����)
		//2.SVM�������е���ֵ����ʾ�����ͷ��೬ƽ��ľ���  
		//3.HoG��ⴰ���ƶ�ʱ�Ĳ���(ˮƽ����ֱ)
		//4.padding����ԭͼ��Χ�������
		//5.����������ʱ���������
		//6.������ֵ
		hog.detectMultiScale(frame, found, 0.2, cv::Size(8, 8), cv::Size(16, 16), 1.05, 2);

		//������
		std::vector<cv::Rect>::const_iterator it;

		std::cout << "found:" << found.size() << std::endl;
		//��⺯���᷵�ر�ʵ��Ŀ�����Դ�һЩ�ļ�����˴˴���΢��Сһ�����
		for (it = found.begin(); it != found.end(); ++it) {
			cv::Rect r = *it;
			//cvRound��ȡ������
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
		}

		cv::namedWindow("result", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
		cv::imshow("result", frame);
		cv::waitKey(10);

	}

	return 0;
}

int harrisDetector()
{

	//	VideoCapture cap(0);
	VideoCapture cap("../testImages\\vtest.avi");

	//�����Ƶ��ʧ��
	if (!cap.isOpened()) {
		std::cout << "Unable to open camera!" << std::endl;
		return -1;
	}

	Mat dst, dst_norm, dst_norm_scaled;

	//��������
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;

	Mat frame;
	Mat gray;

	int thresh = 200;

	while (1) {

		cap >> frame;

		cvtColor(frame, gray, CV_RGB2GRAY);

		Mat dst = Mat::zeros(frame.size(), CV_32FC1);

		//0.����ͼ�񣬵�ͨ��8λͼ�� or ����ͼ��
		//1.�������ӦHarris��Ӧ���������ΪCV_32FC1
		//2.��������ֵ�Ĵ��ڴ�С
		//3.Sobel���ӵĴ��ڴ�С
		//4.��ʽ�е�kֵ����ϸ���μ�
		//5.��Ե����
		cornerHarris(gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

		//��һ��
		normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

		//�ú���ʵ��3�����ܣ����Գ߶ȣ�ȡ����ֵ��ת����uchar���͵�Mat
		//dst=saturate(alpha*src+beta)
		//Ĭ��ֵalpha==1 beta==0
		//�ʱ�����ʵ�ʾ���ת��Ϊ����ʾ��uchar����
		convertScaleAbs(dst_norm, dst_norm_scaled);

		//��ǳ�����ֵ������
		for (int j = 0; j < dst_norm.rows; j++)
		{
			for (int i = 0; i < dst_norm.cols; i++)
			{
				if ((int)dst_norm.at<float>(j, i) > thresh)
				{
					circle(dst_norm_scaled, Point(i, j), 5, Scalar(0), 2, 8, 0);
				}
			}
		}

		imshow("corner", dst_norm_scaled);

		waitKey(30);
	}

	return 0;
}