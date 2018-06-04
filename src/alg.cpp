#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <Eigen/Dense>
#include <Eigen/Core>     // to use Eigen::Map
#include "contourDetection.h"

using namespace cv;
using namespace std;

Mat imageSegmentation(const cv::Mat& color_image, SegmentationType type, ImageROI roi, int theshold) {
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

Mat ContourToYXdata(const Mat &contours, int rowMax)   // function to transfer the contour data into XY coordinate data for optimization purposes
{
  // contours: Contour data
  // rowMax: number of row of the image
	// See matlab function imgdata2plotdata
  Mat XYdataReturn;
  contours.col(1) = rowMax - contours.col(1) - 1;
	// contours.col(1) = contours.col(1) + 1;  // For obtain MATLAB liked data
  return contours;
}

Mat ContourDetect(Mat &image, SegmentationType type, ImageROI roi, int canny_thresh)
{
	Mat segmentedImage = imageSegmentation(image, type, roi);
  int segmentedImageRow = segmentedImage.rows;
	vector<Vec4i> hierarchy;
	Mat canny_output;
	vector<vector<Point> > contours;
  Canny( segmentedImage, canny_output, canny_thresh, canny_thresh*2, 3 );
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  // waitKey(0);
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
  Mat XYdata = ContourToYXdata(contourDataT, segmentedImageRow);
	return XYdata;
}
