#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

enum class ArmorColor { BLUE = 0, RED = 1 };

ArmorColor color = ArmorColor::BLUE;

void BGR(cv::Mat& src, cv::Mat& res)
{
	vector<cv::Mat> BGRSplit;
	cv::split(src, BGRSplit);
	if (color == ArmorColor::BLUE)
		res = BGRSplit[0] - BGRSplit[2];
	else
		res = BGRSplit[2] - BGRSplit[0];
}


/*
void HSV(cv::Mat& src, cv::Mat& res)
{
cv::cvtColor(src, res, cv::COLOR_BGR2HSV);
if (color == ArmorColor::BLUE)
cv::inRange(res, cv::Scalar(60, 43, 46), cv::Scalar(140, 255, 255), res);
else
cv::inRange(res, cv::Scalar(170, 43, 46), cv::Scalar(180, 255, 255), res);
}
*/

int main()
{
	cv::Mat image = cv::imread("source\\8_4.jpg");

	cv::Mat result;
	vector<vector<cv::Point>> contours;

	BGR(image, result);
	/* thresh
		exposure 10 -> 25~35 (+- 10)
		exposure  9 -> 35~45 (+- 10)
		exposure  8 -> 45~55 (+- 10)
	*/
	cv::threshold(result, result, 45, 255, CV_THRESH_BINARY);

	cv::findContours(result, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// cv::drawContours(image, contours, -1, cv::Scalar(0, 255, 0), CV_FILLED);

	
	for (int i = 0; i < contours.size(); ++i) {
		if (contours[i].size() < 5)
			continue;
		cv::RotatedRect r = cv::fitEllipse(contours[i]);
		ellipse(image, r, cv::Scalar(0, 255, 0), -1, 8);
	}
	
	cv::imshow("Image", image);

	cv::waitKey();
	return 0;
}