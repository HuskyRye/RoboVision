#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int h;
int s;
int v;
cv::Mat HSV;
cv::Mat result;

cv::Scalar hsv_low(0, 0, 0);
cv::Scalar hsv_high(180, 255, 255);

void on_trackbar(int, void*)
{
	hsv_low = (h, s, v);
	cv::inRange(HSV, hsv_low, hsv_high, result);
	cv::imshow("Test3", result);
}

int main()
{
	cv::Mat image = cv::imread("6.png");
	cv::Mat ROI = image(cv::Rect(480, 270, 960, 540));
	cv::cvtColor(ROI, HSV, cv::COLOR_BGR2HSV);
	cv::namedWindow("Test3");
	// To DO put three side bar here
	cv::imshow("Test3", HSV);
	cv::waitKey();
}