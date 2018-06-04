#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <Eigen/Dense>
#include <Eigen/Core>     // to use Eigen::Map
#include "contourDetection.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace cv;
using namespace std;

int canny_thresh = 100;
ImageROI roi = {350, 1100, 400, 1460};
SegmentationType type(Black);

int main(int argc, char const *argv[])
{
  if( argc != 3)  // load 2 images one as target and the another as current
    {
     cout <<" Required two images input " << endl;
     return -1;
    }
  Mat target;
  Mat current;
  target = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  current = imread(argv[2], CV_LOAD_IMAGE_COLOR);
  Mat targetContour = ContourDetect(target, type, roi, canny_thresh);
  ofstream contourdata;
  contourdata.open("contourdata.dat");
  contourdata << targetContour;
  contourdata.close();
  // sampling
  int coldata = targetContour.cols;  // column size
	int rowdata = targetContour.rows;  // row size
  Eigen::Map<Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> dataEigen(targetContour.ptr<int>(), rowdata, coldata);
  double radius = 10.0;
  int counterMax = 4;
  int counter = 0;
  Eigen::MatrixXi sampledata = dataEigen.block<1,2>(0,0);
  for (int i = 0; i < dataEigen.rows(); ++i)
  {
    Eigen::RowVector2i v1 = dataEigen.block<1,2>(i,0) - sampledata.block<1,2>(sampledata.rows() - 1,0);
    double disToPoint1 = v1.norm();
    if (disToPoint1 > radius)
    {
      bool flag = 0;
      for (int j = 0; j < sampledata.rows(); ++j)
      {
        Eigen::RowVector2i v2 = dataEigen.block<1,2>(i,0) - sampledata.block<1,2>(j,0);
        double disToPoint2 = v2.norm();
        if (disToPoint2 < radius)
        {
          flag = 1;
          counter++;
          break;
        }
      }
      if (flag == 0)
      {
        Eigen::MatrixXi temp(sampledata.rows() + dataEigen.block<1,2>(i,0).rows(), sampledata.cols());
        temp << sampledata, dataEigen.block<1,2>(i,0);
        sampledata = temp;
      }
    }
    if (counter > counterMax)
    {
      break;
    }
  }
  ofstream sample;
  sample.open("sample.dat");
  sample << sampledata;
  sample.close();
  return 0;
}
