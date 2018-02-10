#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

enum class ArmorColor {	BLUE = 0, RED = 1 };

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

void drawRect(cv::RotatedRect& r, cv::Rect& brect, cv::Mat& frame)
{
	cv::Point2f vertices[4];
	r.points(vertices);
	for (int i = 0; i < 4; i++)
		line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0));
	cv::rectangle(frame, brect, cv::Scalar(0, 0, 255));
}

void lightDetect(vector<vector<cv::Point>>& contours, vector<cv::RotatedRect>& light, cv::Mat& frame)
{
	for (int i = 0; i < contours.size(); ++i) {
		if (contours[i].size() < 30)
			continue;
		cv::RotatedRect r = cv::fitEllipse(contours[i]);
		cv::Rect brect = r.boundingRect();

		if (brect.height <= brect.width)
			continue;

		light.push_back(r);

		// draw RotatedRect & bounidingRect
		drawRect(r, brect, frame);
	}
}

void armorDetect(vector<cv::RotatedRect>& light, vector<cv::RotatedRect>& vArmor, cv::Mat& frame, int angle, double rate)
{
	for (int i = 0; i < light.size(); ++i) {
		for (int j = i + 1; j < light.size(); ++j) {
			if (abs(light[i].angle - light[j].angle) < angle
				&& abs(light[i].size.height - light[j].size.height)
				<= (light[i].size.height + light[j].size.height) / rate
				&& abs(light[i].size.width - light[j].size.width)
				<= (light[i].size.width + light[j].size.width) / rate
				&& abs(light[i].center.y-light[j].center.y)
				<= (light[i].size.height + light[j].size.height) / rate
				&& abs(light[i].center.x-light[j].center.x)
				<= (light[i].size.height + light[j].size.height) * rate)
			{
				cv::RotatedRect Armor;
				Armor.center = (light[i].center + light[j].center) / 2;
				cv::circle(frame, Armor.center, 3, cv::Scalar(0, 255, 255), -1, 8);
			}
		}
	}
}

void setCamera(cv::VideoCapture& capture)
{
	
	//capture.set(CV_CAP_PROP_EXPOSURE, -8);		// 曝光 默认 -8
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);		// 宽度 默认 640
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);		// 高度 默认 480
	capture.set(CV_CAP_PROP_FPS, 60);				// 帧数 默认 120
	
	capture.set(CV_CAP_PROP_EXPOSURE, -10);			// 曝光

}

int main()
{
	// for video
	cv::VideoCapture capture("source\\10_blue.mp4");

	// for camera
	// cv::VideoCapture capture(0);

	if (!capture.isOpened())
		cout << "VideoCaputre failed to open\n";

	// setCamera(capture);

	cv::Mat frame;
	cv::Mat result;

	/*
		press ESC(27) to stop
		1000ms/30(fps)=33ms
		1000ms/60(fps)=17ms
		1000ms/100(fps)=10ms
	*/
	while (cv::waitKey(17) != 27) {
		capture >> frame;
		BGR(frame, result);
		/* thresh
		exposure 10 -> 25~35 (+- 10)
		exposure  9 -> 35~45 (+- 10)
		exposure  8 -> 45~55 (+- 10)
		*/
		cv::threshold(result, result, 10, 255, CV_THRESH_BINARY);
		cv::imshow("Threshold", result);

		vector<vector<cv::Point>> contours;
		cv::findContours(result, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		// cv::drawContours(frame, contours, -1, cv::Scalar(0, 255, 0), CV_FILLED);
		
		vector<cv::RotatedRect> light;
		lightDetect(contours, light, frame);
		
		vector<cv::RotatedRect> vArmor;
		armorDetect(light, vArmor, frame, 12, 1.25);
		cv::imshow("T9", frame);
	}
	return 0;
}