#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::VideoCapture capture("5.MOV");
	if (!capture.isOpened())
		cout << "VideoCaputre failed to open\n";

	cv::namedWindow("Test7");

	cv::Scalar hsv_blue_low(0, 50, 205);
	cv::Scalar hsv_blue_high(180, 255, 255);

	cv::Mat frame;
	cv::Mat ROI;
	cv::Mat element = cv::getStructuringElement(0, cv::Size(3, 3));

	while (true) {
		capture.read(frame);

		ROI = frame(cv::Rect(480, 270, 960, 540));
		cv::cvtColor(ROI, ROI, cv::COLOR_BGR2HSV);
		cv::inRange(ROI, hsv_blue_low, hsv_blue_high, ROI);
		cv::dilate(ROI, ROI, element, cv::Point(-1, -1), 3);
		cv::erode(ROI, ROI, element, cv::Point(-1, -1), 1);

		imshow("Test7", ROI);
		cv::waitKey(1000 / 60);
	}
}