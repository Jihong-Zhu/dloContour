#pragma once

#include <opencv2/opencv.hpp>

struct ImageROI
{
  int x_min;
  int x_max;
  int y_min;
  int y_max;
};

enum SegmentationType
{
  Black,
  Red,
};

cv::Mat ContourToYXdata(const cv::Mat &contours, int rowMax);
cv::Mat imageSegmentation(const cv::Mat& color_image, SegmentationType type, ImageROI roi, int theshold = 50);
cv::Mat ContourDetect(cv::Mat &image, SegmentationType type, ImageROI roi, int canny_thresh);
