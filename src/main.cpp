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

Mat imgToYXdata(const Mat &I)   // function to transfer the image data into XY coordinate data for optimization purposes
{
  // See matlab function imgdata2plotdata
  Mat Index = (Mat_<int>(1,2) << 0,0);  // Initialize with 0,0 to concat
  Mat IndexReturn;
  int colMax = I.cols;    // column size
  int rowMax = I.rows;    // row size
  for (int i = 0; i < colMax; i++ ) {
        for (int j = 0; j < rowMax; j++) {
            if (I.at<uchar>(j, i) == 0) {
              Mat ind = (Mat_<int>(1,2) << j,i);
              vconcat(Index, ind, Index);
            }
        }
    }
  Index.col(0) = rowMax - Index.col(0) - 1;
  Index(Range(1, Index.rows), Range(0,Index.cols)).copyTo(IndexReturn);
  IndexReturn.col(1) = IndexReturn.col(1) + 1;      // For obtain MATLAB like data (C++ index starts at 0) //TODO: consider remove after verfication of the alg.
  return IndexReturn;                 // a n by 2 vector with (Y,X) data
}

int main(int argc, char const *argv[]) {
  if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
  cout << "Ready" << '\n';
  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Canny( image, canny_output, thresh, thresh*2, 3 );
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  Mat coutourData(contours[0][0]);  // create a mat to store the contour
  for(int i= 0; i < contours.size(); i++)
      {
          for(int j= 0; j < contours[i].size();j++) // run until j < contours[i].size();
          {
              Mat temp(contours[i][j]);
              vconcat(coutourData, temp, coutourData); //do whatever
          }
      }
  coutourData(Range(1, coutourData.rows), Range(0,coutourData.cols)).copyTo(coutourData);
  Mat XYdataContour = imgToYXdata(coutourData);
  Mat XYdataContourT;
  transpose(XYdataContour, XYdataContourT);
  cout << XYdataContour.rows << " " << XYdataContour.cols << '\n';
  cout << XYdataContourT.rows << " " << XYdataContourT.cols << '\n';
  ofstream save;
  save.open("save.txt");
  save << XYdataContourT;
  save.close();
  return 0;
}
