#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::Mat image = cv::imread("6.png");
	cv::Mat HSV;
	cv::cvtColor(image, HSV, cv::COLOR_BGR2HSV);
	cv::Scalar hsv_low(0, 50, 230);
	cv::Scalar hsv_high(180, 255, 255);
	cv::inRange(HSV, hsv_low, hsv_high, HSV);
	cv::imshow("Test6", HSV);
	cv::waitKey();

	cv::Mat element = cv::getStructuringElement(0, cv::Size(3, 3));

	cv::dilate(HSV, HSV, element, cv::Point(-1, -1), 3);
	cv::imshow("Test6", HSV);
	cv::waitKey();

	cv::erode(HSV, HSV, element, cv::Point(-1, -1), 1);
	cv::imshow("Test6", HSV);
	cv::waitKey();

	cv::waitKey();
}