#include "stdafx.h"
#include "red_circle.h"
#include "E:\opencv\opencv\build\include\opencv2\opencv.hpp"
#include "E:\opencv\opencv\build\include\opencv2\core\core.hpp"
#include "E:\opencv\opencv\build\include\opencv2\highgui\highgui.hpp"
#include "E:\opencv\opencv\build\include\opencv2\imgproc\imgproc.hpp"
#include "E:\opencv\opencv\build\include\opencv2\imgproc\imgproc_c.h"
#include <windows.h>
#include <iostream>

using namespace cv;
using namespace std;
#define MAX_LOADSTRING 100

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR   lpCmdLine, _In_ int  nCmdShow)
{
	Mat image0 = imread("F:\\red_light.jpg");

	if (image0.empty())
	{
		std::exit(-1);
	}

	Mat image1 = image0.clone();

	medianBlur(image1, image1, 3);//中值滤波

	Mat hsv_image;
	cvtColor(image1, hsv_image, COLOR_BGR2HSV);//RGB to HSV

	Mat lower_red_hue_range;
	Mat upper_red_hue_range;
	inRange(hsv_image, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
	inRange(hsv_image, Scalar(160, 100, 100), Scalar(179, 255, 255), upper_red_hue_range);//红色像素点HSV范围
	Mat red_hue_image;
	addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
	GaussianBlur(red_hue_image, red_hue_image, Size(9, 9), 2, 2);//高斯滤波，平滑处理


	Mat green_hue_range;
	inRange(hsv_image, Scalar(35, 100, 100), Scalar(77, 255, 255), green_hue_range);//绿色像素点HSV范围
	GaussianBlur(green_hue_range, green_hue_range, Size(9, 9), 2, 2);//高斯滤波，平滑处理


	std::vector<Vec3f> circles_red;
	circles_red.clear();
	HoughCircles(red_hue_image, circles_red, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);//霍夫圆环检测红灯

	std::vector<Vec3f> circles_green;
	circles_green.clear();
	HoughCircles(green_hue_range, circles_green, CV_HOUGH_GRADIENT, 1, green_hue_range.rows / 8, 100, 20, 0, 0);//霍夫圆环检测绿灯
	
	// 根据画圆的个数来判定指示灯的颜色
	if (circles_red.size() == 0 && circles_green.size() == 0)
	{
		MessageBox(NULL, _T("无指示灯"), _T("结果"), MB_OK | MB_ICONWARNING);
	}
	
	for (size_t current_circle = 0; current_circle < circles_red.size(); ++current_circle)
	{	
		Point center(std::round(circles_red[current_circle][0]), std::round(circles_red[current_circle][1]));
		int radius = std::round(circles_red[current_circle][2]);

		if(circles_red.size() == 1 && radius != 0)
			MessageBox(NULL, _T("指示灯为红色"), _T("结果"), MB_OK | MB_ICONWARNING);
		else
			MessageBox(NULL, _T("指示灯为绿色"), _T("结果"), MB_OK | MB_ICONWARNING);

		circle(image1, center, radius, Scalar(0, 0, 255), 5);
	}

	namedWindow("红绿灯检测标记", WINDOW_AUTOSIZE);
	imshow("红绿灯检测标记", image1);

	waitKey(0);

	return 0;
}
