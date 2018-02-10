#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::VideoCapture capture("4.MOV");
	if (!capture.isOpened())
		cout << "VideoCaputre failed to open\n";

	cv::namedWindow("Test");


	cv::Mat frame;
	cv::Mat result;

	cv::Mat temp;

	// RGB - blue
	cv::Scalar rgb_blue_low(190, 190, 190);
	cv::Scalar rgb_blue_high(255, 255, 255);

	// RGB - red
	cv::Scalar rgb_red_low(210, 210, 210);
	cv::Scalar rgb_red_high(255, 255, 255);

	cv::Scalar hsv_blue_low(100, 43, 35);
	cv::Scalar hsv_blue_high(124, 255, 255);

	while (true) {
		capture.read(frame);

		cv::cvtColor(frame, temp, cv::COLOR_BGR2HSV);
		cv::inRange(temp, hsv_blue_low, hsv_blue_high, result);

		//cv::inRange(frame, rgb_red_low, rgb_red_high, result);
		imshow("Test", result);
		cv::waitKey(1000/60);
	}
}