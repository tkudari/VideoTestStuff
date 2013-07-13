
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "cv.h"
using namespace cv;
using namespace std;

int main() {
//Indicates whether this is the first time in the loop of frames.
bool first = true;
CvRect bndRect = cvRect(0,0,0,0);
CvPoint pt1, pt2;

//Create a new window.
cvNamedWindow("My Window", CV_WINDOW_AUTOSIZE);

CvCapture *input;
IplImage* colourImage;
IplImage* difference;
IplImage* temp;
CvSize imgSize;
//todetect
//CvMemStorage *hstorage;

//imgSize.width = 352;
//imgSize.height = 240;
int avgX = 0;

input = cvCaptureFromCAM(0);
if (!input) {
	cout << "Can't open file" ;
}

colourImage = cvQueryFrame(input);
imgSize = cvGetSize(colourImage);

IplImage* greyImage = cvCreateImage( imgSize, IPL_DEPTH_8U, 1);
IplImage* movingAverage = cvCreateImage( imgSize, IPL_DEPTH_32F, 3);
IplImage* motionHistory = cvCreateImage( imgSize, IPL_DEPTH_8U, 3);
//todetect
//CvHaarClassifierCascade *cascade;
//cascade = ( CvHaarClassifierCascade* )cvLoad( "/Users/tejuskudari/mexopencv/mexopencv/test/palm.xml", 0, 0, 0 );
//hstorage = cvCreateMemStorage( 0 );
///




while (true) {
	colourImage = cvQueryFrame(input);

	//If this is the first time, initialize the images.
	if (first) {
		difference = cvCloneImage(colourImage);
		temp = cvCloneImage(colourImage);
		cvConvertScale(colourImage, movingAverage, 1.0, 0.0);
		first = false;
	}
	//else, make a running average of the motion.
	else {
		cvRunningAvg(colourImage, movingAverage, 0.020, NULL);
	}

	//Convert the scale of the moving average.
	cvConvertScale(movingAverage,temp, 1.0, 0.0);

	//Minus the current frame from the moving average.
	cvAbsDiff(colourImage,temp,difference);

	//Convert the image to grayscale.
	cvCvtColor(difference,greyImage,CV_RGB2GRAY);

	//Convert the image to black and white.
	cvThreshold(greyImage, greyImage, 70, 255, CV_THRESH_BINARY);

	//Dilate and erode to get people blobs
	cvDilate(greyImage, greyImage, 0, 18);
	cvErode(greyImage, greyImage, 0, 10);



	//Find the contours of the moving images in the frame.
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	cvFindContours( greyImage, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

//todetect
//	CvSeq *hand = cvHaarDetectObjects(greyImage, cascade, hstorage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(100, 100));


	for( ; contour != 0; contour = contour->h_next ) {
	//Get a bounding rectangle around the moving object.
	bndRect = cvBoundingRect(contour, 0);

	pt1.x = bndRect.x;
	pt1.y = bndRect.y;
	pt2.x = bndRect.x + bndRect.width;
	pt2.y = bndRect.y + bndRect.height;

	//Get an average X position of the moving contour.
	avgX = (pt1.x + pt2.x) / 2;
	cout << "avgX here = " << avgX << "\n";

	cvRectangle(colourImage, pt1, pt2, CV_RGB(255,0,0), 1);
	}

	//Show the frame.
	cvShowImage("My Window", colourImage);

	//Wait for the user to see it.
	cvWaitKey(10);


}
return 0;
}


