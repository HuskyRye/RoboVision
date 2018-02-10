#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::Mat image = cv::imread("3.png", cv::WINDOW_AUTOSIZE);
	cv::imshow("Src", image);

	/*
	cv::Mat imgYUV;
	cv::cvtColor(image, imgYUV, cv::COLOR_BGR2YUV);
	// cv::imshow("RoboVision", imgYUV);
	// cv::waitKey(1000);

	cv::Scalar bluelow(0, 170, -157);			// U for blue from *** to ***
	cv::Scalar bluehigh(255, 255, 0);
	*/

	cv::Scalar low(199, 199, 199);
	cv::Scalar high(255, 255, 255);
	
	cv::Mat result;
	// cv::inRange(imgYUV, bluelow, bluehigh, result);
	cv::inRange(image, low, high, result);
	cv::imshow("RoboVision", result);
	cv::waitKey();

	return 0;
}