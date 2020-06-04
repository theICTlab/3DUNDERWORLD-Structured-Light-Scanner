//------------------------------------------------------------------------------------------------------------
//* Copyright © 2010-2013 Immersive and Creative Technologies Lab, Cyprus University of Technology           *
//* Link: http://ict.cut.ac.cy                                                                               *
//* Software developer(s): Kyriakos Herakleous                                                               *
//* Researcher(s): Kyriakos Herakleous, Charalambos Poullis                                                  *
//*                                                                                                          *
//* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.*
//* Link: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US                                        *
//------------------------------------------------------------------------------------------------------------

#include "StdAfx.h"
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include "WebCam.h"


WebCam::WebCam(int id, int camW, int camH)
{
	liveView=false;
	saveCount=1;

	// OpenCV_4
	// capture = cvCaptureFromCAM(id);
	capture.open(id + cv::CAP_ANY);
	if (!capture.isOpened()) {
		std::cerr << "ERROR! Unable to open camera\n";
		return;
	}
	capture.set(cv::CAP_PROP_FRAME_WIDTH, camW ); 
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, camH);
}

void WebCam::resetSaveCount()
{
	saveCount = 1;
}

WebCam::~WebCam(void)
{
	//cvReleaseCapture(&capture);
	capture.release();
}

void WebCam::startLiveview()
{
	cv::namedWindow("Scanner Window",cv::WINDOW_AUTOSIZE);
	cv::resizeWindow("Scanner Window",640,480);

	liveView=true;
}

void WebCam::endLiveview()
{
	cv::destroyWindow("Scanner Window");
	liveView=false;
}

void WebCam::UpdateView()
{

		if(!liveView)
			return;

		// OpenCV_4
		// IplImage* retrieved_frame = cvRetrieveFrame(capture);
		cv::Mat retrieved_frame;
		capture >> retrieved_frame;

		// IplImage* resampled_image = cvCreateImage(cvSize(640, 480), retrieved_frame->depth, retrieved_frame->nChannels);
		cv::Mat resampled_image(cv::Size(640, 480), retrieved_frame.depth(), retrieved_frame.channels());

		cv::resize(retrieved_frame, resampled_image, cv::Size(640, 480),cv::INTER_LINEAR);

		///Show the image in the window given
		cv::imshow("Scanner Window", resampled_image);

		//cvReleaseImage(&resampled_image);
		resampled_image.release();
}

void WebCam::captureImg()
{
	std::stringstream path;
	cv::Mat frame;

	path<<"scan/capture/"<< saveCount <<".png";
	
	// OpenCV_4
	// cvRetrieveFrame(capture);
	// cvSaveImage(path.str().c_str(), cvRetrieveFrame(capture));
	capture.read(frame);
	cv::imwrite(path.str(), frame);
	saveCount++;

}

void WebCam::captureImg(char* dirPath)
{
	std::stringstream path;

	path<<dirPath<< saveCount <<".png";
	
	cv::Mat resampled_image;
	capture.read(resampled_image);
	cv::waitKey(20);
	capture.read(resampled_image);
	cv::waitKey(20);
	capture.read(resampled_image);

	cv::imwrite(path.str().c_str(), resampled_image);
	saveCount++;
}

int WebCam::getNumOfCams()
{
	return 1;
}