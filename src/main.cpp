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
vector<Vec4i> hierarchy;
RNG rng(12345); // for contour drawing

int main(int argc, char const *argv[]) {
  if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Canny( image, canny_output, thresh, thresh*2, 3 );
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  Mat contourData(contours[0][0]);  // create a mat to store the contour
  int index = 0;
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
