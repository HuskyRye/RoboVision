#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::VideoCapture capture("5.MOV");
	if (!capture.isOpened())
		cout << "VideoCaputre failed to open\n";

	cv::namedWindow("Test5");

	cv::Scalar hsv_blue_low(0, 50, 240);
	cv::Scalar hsv_blue_high(180, 255, 255);

	cv::Mat frame;
	cv::Mat HSV;
	cv::Mat ROI;
	cv::Mat result;

	while (true) {
		capture.read(frame);
		ROI = frame(cv::Rect(480, 270, 960, 540));
		cv::cvtColor(ROI, HSV, cv::COLOR_BGR2HSV);
		cv::inRange(HSV, hsv_blue_low, hsv_blue_high, result);

		//cv::inRange(frame, rgb_red_low, rgb_red_high, result);
		imshow("Test5", result);
		cv::waitKey(1000 / 60);
	}
}