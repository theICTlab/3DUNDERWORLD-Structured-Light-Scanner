
#include "stdafx.h"
#include <iostream>
#include <fstream>
using std::ofstream;
#include "cv.h"
#include "highgui.h"
#include "GrayCodes.h"
#include "Projector.h"

#include "SLS2012.h"
 
IplImage* calBoard;

GrayCodes *grayCode;


CvCapture* initCam(int Cam_RES_X, int Cam_RES_Y, int cam_id=0)
{

	CvCapture *capture = cvCaptureFromCAM(cam_id);
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, Cam_RES_X ); 
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, Cam_RES_Y);

	return capture;

}



void initCVWindows()
{
	cvNamedWindow("Scanner Window",CV_WINDOW_AUTOSIZE);
	cvResizeWindow("Scanner Window",640,480);
}


int scanMain()	
{
	
	std::cout << "Starting PSLS..." << std::endl;
	std::cout << "\nStep 1: Camera Calibration\nStep 2: Camera Projector Calibration\nStep 3: Scanning\n\nPress 'enter' to snap a picture,'space' to go to the next step or 'esc' to exit!" << std::endl;
	
	int camW =	1600;
	int camH =	1200;
	int projW = 1024;
	int projH = 768;
	

	///Get the required camera
	int camera_id = 0;
	
	///Create a camera
	CvCapture *capture;
	capture= initCam(camW,camH,camera_id);

	

	calBoard = cvLoadImage("scanWebCam/cal.png");

	//loadGrayCodes();
	grayCode = new GrayCodes(projW,projH);
	grayCode->load();

	Projector *proj=new Projector(projW,projH);
	proj->showImg(grayCode->getImg(0));

	initCVWindows();



	int saveCount=1;
	int mode=1;
	int grayCount=0;

	while(true)	
	{
		///Grab a frame
		//cvGrabFrame(capture);

		IplImage *retrieved_frame = cvRetrieveFrame(capture);

		IplImage* resampled_image = cvCreateImage(cvSize(640, 480), retrieved_frame->depth, retrieved_frame->nChannels);

		cvResize(retrieved_frame, resampled_image, CV_INTER_LINEAR);

		///Show the image in the window given
		cvShowImage("Scanner Window", resampled_image);
			
		int key = cvWaitKey(10);

		//27 esc 32 space

		//If enter is pressed then capture the image
		if (key == 13)
		{
			if(mode==1)
			{
				std::stringstream path;

				path<<"scanWebCam/camCalib/"<< saveCount <<".bmp";

				cvSaveImage(path.str().c_str(), cvRetrieveFrame(capture));
				saveCount++;
			}
			
			if(mode==2)
			{
				std::stringstream path;

				path<<"scanWebCam/projCalib/"<< saveCount <<".bmp";

				cvSaveImage(path.str().c_str(), cvRetrieveFrame(capture));
				saveCount++;
			}

			if(mode==3)
			{
				std::stringstream path;

				path<<"scanWebCam/dataSet/"<< saveCount <<".bmp";

				cvSaveImage(path.str().c_str(), cvRetrieveFrame(capture));
				saveCount++;
				grayCount++;
				if(grayCount==grayCode->getNumOfImgs())
					break;
				proj->showImg(grayCode->getNextImg());

			}

			
		}
		
		if(key == 32)
		{
			if(mode<3)
			{
				mode++;
				saveCount=1;

				if(mode==2)
				{
					//char sel;
					proj->showImg( calBoard);

				}
				if(mode==3)
				{
					//char sel;
					proj->showImg( grayCode->getNextImg());
				}

			}

		}

		if(key == 27)
			break;
				
		///Release memory
		cvReleaseImage(&resampled_image);
	}
	
	
	///Destroy the window
	delete proj;
	cvDestroyWindow("Scanner Window");

	cvReleaseCapture(&capture);

	return 0;
}