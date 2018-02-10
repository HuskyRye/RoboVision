#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

int main()
{
	cv::Mat image = cv::imread("6.png");
	cv::Mat ROI = image(cv::Rect(480, 270, 960, 540));
	cv::Mat HSV;
	cv::cvtColor(ROI, HSV, cv::COLOR_BGR2HSV);
	cv::Scalar hsv_high(180, 255, 255);
	cv::Mat result;

	for (int h = 0; h <= 0; h += 50) {
		for (int s = 50; s <= 50; ) {
			for (int v = 150; v <= 255; v += 10) {
				cv::inRange(HSV, cv::Scalar(h, s, v), hsv_high, result);
				cv::imwrite("test4_1_"+to_string(h)+'_'+to_string(s)+'_'+to_string(v)+".jpg", result);
				cout << "test4_1_" + to_string(h) + '-' + to_string(s) + '-' + to_string(v) + ".jpg\n";
				cv::waitKey(30);
			}
		}
	}
}