//
//  FaceAverage.h
//  faceAverage
//
//  Created by Pierre Proske on 3/3/17.
//
//

#ifndef FaceAverage_h
#define FaceAverage_h

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"

ofImage processImages(vector<cv::Mat> images, vector<vector<cv::Point2f> > allPoints, int w, int h);
void similarityTransform(vector<cv::Point2f>& inPoints, vector<cv::Point2f>& outPoints, cv::Mat &tform);
void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, vector<cv::Point2f> &srcTri, vector<cv::Point2f> &dstTri);
void warpTriangle(cv::Mat &img1, cv::Mat &img2, vector<cv::Point2f> t1, vector<cv::Point2f> t2);
void constrainPoint(cv::Point2f &p, cv::Size sz);
void calculateDelaunayTriangles(cv::Rect rect, vector<cv::Point2f> &points, vector< vector<int> > &delaunayTri);

#endif /* FaceAverage_h */
