#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <Eigen/Dense>
#include <Eigen/Core>     // to use Eigen::Map

#include <iostream>
#include <cmath>
#include <fstream>

using namespace cv;
using namespace std;

int thresh = 100;
Mat canny_output;
vector<vector<Point> > contours;

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

ImageROI roi = {350, 1100, 400, 1460};
vector<Vec4i> hierarchy;

cv::Mat imageSegmentation(const cv::Mat& color_image, SegmentationType type, ImageROI roi, int theshold = 50) {
	Rect2i ROI{roi.x_min, roi.y_min, roi.x_max - roi.x_min, roi.y_max - roi.y_min};            // create a rectangle for selecting ROI (x_min, y_min, x_max-x_min, y_max-y_min)
	Mat imageROI = color_image(ROI);
	Mat binary_image;                            // Binary image
	switch(type) {
	case SegmentationType::Black:
	{
		Mat imageGray;                              // Gray scale image
		cv::cvtColor(imageROI, imageGray, CV_BGR2GRAY);
		const int maxValue = 255;                         // Maximum value
		cv::threshold(imageGray, binary_image, theshold, maxValue, THRESH_BINARY);      // Binary image
	}
	break;
	case SegmentationType::Red:
	{
		cv::Mat hsv_img;
		cv::cvtColor(imageROI, hsv_img, cv::COLOR_BGR2HSV);

		// Threshold the HSV image, keep only the red pixels
		cv::Mat mask1;
		cv::Mat mask2;
		cv::inRange(hsv_img, cv::Scalar(0, 100, 100), cv::Scalar(theshold, 255, 255), mask1);
		cv::inRange(hsv_img, cv::Scalar(179-theshold, 100, 100), cv::Scalar(179, 255, 255), mask2);
		cv::Mat detectRed = mask1 | mask2;        // With this red will be white and rest will be black
		// Reverse black and white to fit with the active contour alg.
		cv::bitwise_not(detectRed, binary_image);
	}
	break;
	}

	Mat filtered_image;
	cv::erode(binary_image, filtered_image, Mat());
	cv::dilate(binary_image, filtered_image, Mat());

	return filtered_image;
}

int main(int argc, char const *argv[]) {
  if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  std::cout << "size of the image: " << image.rows << " " << image.cols << '\n';
  SegmentationType type(Black);
  Mat segmentedImage = imageSegmentation(image, type, roi);
  Canny( segmentedImage, canny_output, thresh, thresh*2, 3 );
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  waitKey(0);
  Mat contourData(contours[0][0]);  // create a mat to store the contour
  int index = 0;
  std::cout << "contour size " << contours.size() << '\n';
  // Find the contour with most points(it must be the right contour)
  for(int i= 0; i < contours.size(); i++)
  {
    if (contours[index].size() < contours[i].size())
    {
      index = i;
    }
  }
  // Save all the contour data
  for(int j= 0; j < contours[index].size();j++) // run until j < contours[i].size();
  {
    Mat temp(contours[index][j]);
    hconcat(contourData, temp, contourData); //do whatever
  }
  Mat contourDataT;
  transpose(contourData, contourDataT);
  contourDataT(Range(1, contourDataT.rows), Range(0,contourDataT.cols)).copyTo(contourDataT);
  ofstream contourdata;
  contourdata.open("contourdata.dat");
  contourdata << contourDataT;
  contourdata.close();
  return 0;
}
