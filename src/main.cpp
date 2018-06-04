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

int main(int argc, char const *argv[]) {
  if( argc != 3)  // load 2 images one as target and the another as current
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
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
  return 0;
}
