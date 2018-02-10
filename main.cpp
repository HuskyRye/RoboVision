#include "RoboVision.h"

cv::Mat src_img;
cv::Mat show_detectLights;
cv::Mat show_possibleArmors;
cv::Mat show_selectArmor;

int main()
{
	if (DEBUG) {
		cv::namedWindow("detectLights", CV_WINDOW_NORMAL);
		cv::namedWindow("possibleArmors", CV_WINDOW_NORMAL);
		cv::namedWindow("selectArmor", CV_WINDOW_NORMAL);
	}
	detectArmor();
 }