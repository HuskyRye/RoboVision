#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

cv::Mat image;
int low_h = 0;
int high_h = 179;
int low_s = 0;
int high_s = 255;
int low_v = 0;
int high_v = 255;
cv::Mat element;

void redraw(int = 0, void* = NULL)
{
	cv::Mat HSV;
	cv::cvtColor(image, HSV, cv::COLOR_BGR2HSV);
	vector<cv::Mat> hsvSplit;
	cv::split(HSV, hsvSplit);
	cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, HSV);

	cv::inRange(HSV, cv::Scalar(low_h, low_s, low_v), cv::Scalar(high_h, high_s, high_v), HSV);
	cv::morphologyEx(HSV, HSV, cv::MORPH_OPEN, element);
	cv::morphologyEx(HSV, HSV, cv::MORPH_CLOSE, element);
	cv::imshow("Test8", HSV);
}

int main()
{
	image = cv::imread("source\\3.png");
	element = cv::getStructuringElement(0, cv::Size(3, 3));

	cv::namedWindow("Control", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("LowH", "Control", &low_h, 179, redraw);
	cv::createTrackbar("HighH", "Control", &high_h, 179, redraw);

	cv::createTrackbar("LowS", "Control", &low_s, 255, redraw);
	cv::createTrackbar("HighS", "Control", &high_s, 255, redraw);

	cv::createTrackbar("LowV", "Control", &low_v, 255, redraw);
	cv::createTrackbar("HighV", "Control", &high_v, 255, redraw);

	redraw();
	cv::waitKey();
}