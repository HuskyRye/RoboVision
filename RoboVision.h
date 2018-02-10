#pragma once
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "uart.h"

#define DEBUG true
#define DEBUG_LIGHT true
#define DEBUG_POSSIBLE_PARMOR true
#define DEBUG_FINAL_ARMOR true

enum class Enemy_color { BLUE = 0, RED = 1 };

class ArmorInfo {
public:
	ArmorInfo(cv::RotatedRect armor_rect, std::vector<cv::Point2f> armor_vertex, float armor_stddev = 0.0) {
		rect = armor_rect;
		vertex = armor_vertex;
		stddev = armor_stddev;
	}
public:
	cv::RotatedRect rect;
	std::vector<cv::Point2f> vertex;
	float stddev;
};

extern cv::Mat src_img;
extern cv::Mat show_detectLights;
extern cv::Mat show_possibleArmors;
extern cv::Mat show_selectArmor;

void detectArmor();
cv::Mat distillationColor(const cv::Mat& src, Enemy_color color);
void detectLights(Enemy_color enemy_color, std::vector<cv::RotatedRect>& lights);
void calArmorInfo(std::vector<cv::Point2f>& armor_points, cv::RotatedRect left_light, cv::RotatedRect right_light);
void possibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<ArmorInfo>& armors);
ArmorInfo slectFinalArmor(std::vector<ArmorInfo> &armors);
// void predictArmor(const cv::Mat now);
void drawRotatedRect(const cv::Mat& img, const cv::RotatedRect& box, const cv::Scalar& color, int thickness);