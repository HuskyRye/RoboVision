#include "RoboVision.h"

void detectArmor()
{
	// cv::VideoCapture capture("source\\haha.mp4");
	cv::VideoCapture capture;
	try {
		capture.open(0);
	}
	catch (std::exception ex) {
		std::cout << ex.what() << std::endl;
	}
	if (!capture.isOpened())
		std::cerr << "Camera open error" << std::endl;
	// capture.set(CV_CAP_PROP_EXPOSURE, -7);		// ÆØ¹â Ä¬ÈÏ -7
	Enemy_color enemy_color = Enemy_color::BLUE;

	cv::Mat frame;
	std::vector<cv::RotatedRect> lights;
	std::vector<ArmorInfo> armors;

	int fd = uart_open("/dev/ttyTHS2");
	if (fd>0) {
		int err;
		do {
			err = uart_init(fd, 115200, 0, 8, 1, 'N');
		} while (err == UART_ERROR);
	}
	else {
		std::cout << "Can't Open Serial Port!\n";
		return;
	}

	std::vector<cv::Point3f> armor_points;
	armor_points.emplace_back(cv::Point3f(0.0, 0.0, 0.0));
	armor_points.emplace_back(cv::Point3f(50, 0.0, 0.0));
	armor_points.emplace_back(cv::Point3f(50, 40, 0.0));
	armor_points.emplace_back(cv::Point3f(0.0, 40, 0.0));

	double camera_m[] = { 540.66275, 0, 309.8505, 0, 543.3125, 237.8745, 0, 0, 1 };
	cv::Mat camera_matrix(3, 3, CV_64F, camera_m);

	double camera_d[] = { -0.08135425, 0.69895325, 0.0035774035, -0.004424435, -1.76639 };
	cv::Mat camera_distortion(5, 1, CV_64F, camera_d);

	while (cv::waitKey(33) != 27) {
		capture >> src_img;
		if (src_img.empty())
			break;
		detectLights(enemy_color, lights);
		possibleArmors(lights, armors);
		if (!armors.empty()) {
			ArmorInfo final_armor = slectFinalArmor(armors);
			// int pixle = static_cast<int>(final_armor.rect.center.x - 320);
			// send_pixle(fd, pixle);
			cv::Mat rvec;
			cv::Mat tvec;
			cv::solvePnP(armor_points, final_armor.vertex, camera_matrix, camera_distortion, rvec, tvec);
			float yaw = rvec.at<float>[0][1] + atan((tvec.at<float>[0][0] - 25) / tvec.at<float>[0][2]);
			send_angle(fd, yaw);
		}
		else if (DEBUG && DEBUG_FINAL_ARMOR) {
			show_selectArmor = src_img.clone();
			cv::imshow("selectArmor", show_selectArmor);
		}
		lights.clear();
		armors.clear();
	}
}

cv::Mat distillationColor(const cv::Mat & src, Enemy_color color)
{
	std::vector<cv::Mat> bgr;
	cv::split(src, bgr);
	if (color == Enemy_color::BLUE)
		return bgr[0] - bgr[2];
	else
		return bgr[2] - bgr[0];
}

void detectLights(Enemy_color enemy_color, std::vector<cv::RotatedRect>& lights)
{
	cv::Mat light = distillationColor(src_img, enemy_color);
	cv::Mat binary_brightness;
	cv::Mat gray_img;
	cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);
	cv::threshold(gray_img, binary_brightness, 200, 255, CV_THRESH_BINARY);

	float thresh = (enemy_color == Enemy_color::BLUE) ? 90 : 50;
	cv::Mat binary_color;
	cv::threshold(light, binary_color, thresh, 255, CV_THRESH_BINARY);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(binary_color, binary_color, element, cv::Point(-1, -1), 1);

	cv::Mat binary_light = binary_brightness & binary_color;

	std::vector<std::vector<cv::Point>> contours_light;
	cv::findContours(binary_light, contours_light, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> contours_brightness;
	cv::findContours(binary_brightness, contours_brightness, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	if (DEBUG && DEBUG_LIGHT)
		show_detectLights = src_img.clone();

	lights.reserve(contours_brightness.size());
	std::vector<int> is_processes(contours_brightness.size());
	for (unsigned int i = 0; i < contours_light.size(); ++i) {
		for (unsigned int j = 0; j < contours_brightness.size(); ++j) {
			if (!is_processes[j]) {
				if (cv::pointPolygonTest(contours_brightness[j], contours_light[i][0], true) >= 0.0) {
					cv::RotatedRect single_light = cv::minAreaRect(contours_brightness[j]);

					cv::Rect brect = single_light.boundingRect();
					if (brect.height <= brect.width)
						continue;
					// float light_aspect_ratio =
				 	// std::max(single_light.size.width, single_light.size.height) / std::min(single_light.size.width, single_light.size.height);
					// if (light_aspect_ratio > 6)
					//	continue;

					lights.push_back(single_light);
					if (DEBUG && DEBUG_LIGHT)
						drawRotatedRect(show_detectLights, single_light, cv::Scalar(0, 255, 0), 2);
					is_processes[j] = true;
					break;
				}
			}
		}
	}
	if (DEBUG && DEBUG_LIGHT)
		cv::imshow("detectLights", show_detectLights);
}

void calArmorInfo(std::vector<cv::Point2f>& armor_points, cv::RotatedRect left_light, cv::RotatedRect right_light)
{
	cv::Point2f left_points[4], right_points[4];
	left_light.points(left_points);
	right_light.points(right_points);
	cv::Point2f right_lu, right_ld, lift_ru, lift_rd;
	std::sort(left_points, left_points + 4, [](const cv::Point2f &p1, const cv::Point2f &p2) { return p1.x < p2.x; });
	std::sort(right_points, right_points + 4, [](const cv::Point2f &p1, const cv::Point2f &p2) { return p1.x < p2.x; });
	if (right_points[0].y < right_points[1].y) {
		right_lu = right_points[0];
		right_ld = right_points[1];
	}
	else {
		right_lu = right_points[1];
		right_ld = right_points[0];
	}

	if (left_points[2].y < left_points[3].y) {
		lift_ru = left_points[2];
		lift_rd = left_points[3];
	}
	else {
		lift_ru = left_points[3];
		lift_rd = left_points[2];
	}
	armor_points.push_back(lift_ru);
	armor_points.push_back(right_lu);
	armor_points.push_back(right_ld);
	armor_points.push_back(lift_rd);
}

void possibleArmors(const std::vector<cv::RotatedRect>& lights, std::vector<ArmorInfo>& armors)
{
	if (DEBUG && DEBUG_POSSIBLE_PARMOR)
		show_possibleArmors = src_img.clone();
	for (int i = 0; i < lights.size(); ++i) {
		for (int j = i + 1; j < lights.size(); ++j) {
			auto edge1 = std::minmax(lights[i].size.width, lights[i].size.height);
			auto edge2 = std::minmax(lights[j].size.width, lights[j].size.height);
			if ((abs(lights[i].angle - lights[j].angle) < 30)
				&& abs(edge1.second - edge2.second) <= (edge1.second + edge2.second) / 5				// 1.15
				&& abs(edge1.first - edge1.first) <= ((edge1.first + edge2.first) / 2)
				&& abs(lights[i].center.y - lights[j].center.y) <= (edge1.second + edge2.second) / 1.25		// 1.5
				&& abs(lights[i].center.x - lights[j].center.x) <= (edge1.second + edge2.second) * 1.5		// 1.5
				&& abs(lights[i].center.x - lights[j].center.x) >= (edge1.second + edge2.second) / 1.6		// 2
				){

				float lights_dis = std::sqrt((lights[i].center.x - lights[j].center.x) * (lights[i].center.x - lights[j].center.x) +
					(lights[i].center.y - lights[j].center.y) * (lights[i].center.y - lights[j].center.y));
				double center_angle =
					std::atan((lights[i].center.y - lights[j].center.y) / (lights[i].center.x - lights[j].center.x)) * 180 / CV_PI;

				cv::RotatedRect rect;
				rect.angle = static_cast<float>(center_angle);
				rect.center = (lights[i].center + lights[j].center) / 2;
				float armor_width = static_cast<float>(lights_dis) - std::max(edge1.first, edge2.first);
				float armor_height = std::max<float>(edge1.second, edge2.second);
				rect.size.width = std::max<float>(armor_width, armor_height);
				rect.size.height = std::min<float>(armor_width, armor_height);

				std::vector<cv::Point2f> armor_points;
				if (lights[i].center.x < lights[j].center.x)
					calArmorInfo(armor_points, lights[i], lights[j]);
				else
					calArmorInfo(armor_points, lights[j], lights[i]);
				armors.emplace_back(ArmorInfo(rect, armor_points));
				if (DEBUG && DEBUG_POSSIBLE_PARMOR)
					drawRotatedRect(show_possibleArmors, rect, cv::Scalar(0, 255, 0), 2);
			}
		}
	}
	if (DEBUG && DEBUG_POSSIBLE_PARMOR)
		cv::imshow("possibleArmors", show_possibleArmors);
}

ArmorInfo slectFinalArmor(std::vector<ArmorInfo> &armors)
{
	std::sort(armors.begin(),
		armors.end(),
		[](const ArmorInfo &p1, const ArmorInfo &p2) { return p1.rect.size.area() > p2.rect.size.area(); });

	if (DEBUG && DEBUG_FINAL_ARMOR) {
		show_selectArmor = src_img.clone();
		drawRotatedRect(show_selectArmor, armors[0].rect, cv::Scalar(0, 255, 0), 2);
		cv::circle(show_selectArmor, armors[0].rect.center, 3, cv::Scalar(0, 255, 255), -1, 8);
		cv::imshow("selectArmor", show_selectArmor);
	}
	return armors[0];
}

void drawRotatedRect(const cv::Mat& img, const cv::RotatedRect& rect, const cv::Scalar& color, int thickness)
{
	cv::Point2f vertices[4];
	rect.points(vertices);
	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], color, thickness, CV_AA);
}

