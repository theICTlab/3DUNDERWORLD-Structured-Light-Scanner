
#include "stdafx.h"
#include "SLS2012.h"
#include <iostream>
#include <fstream>
using std::ofstream;
#include "cv.h"
#include "highgui.h"

#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"

#define STRICT
#include <windows.h>
#include <algorithm>
using std::min;
using std::max;
#include <gdiplus.h>
#include "GrayCodes.h"

#include <conio.h>

#include "CanonCamera.h"
#include <atlimage.h>
#include "Projector.h"

#define GRAY_NUM 42 




IplImage* projCalibBoard ;
	
GrayCodes *grayCodes;

Projector *proj;


bool captureCalib(CanonCamera *camera)
{
	//startCameraLiveView
	camera->startLiveview();

	
	int key;

	while(true)
	{
		

		camera->UpdateView();

		key = cvWaitKey(10);

		///If enter is pressed then capture the image
		if (key == 13)
		{
			camera->captureImg();
		}
		
		//32-> enter / 27-> esc 
		if(key == 32 || key == 27)
		{
			break;
		}

		
	}

	camera->endLiveview();

	cvWaitKey(100);
	if(key == 27)
		return 0;
	else 
		return 1;

}




void scan(CanonCamera *camera[],int camCount)
{
	

	int grayCount=0;

	while(true)
	{
		int key;

		for(int i=0; i<camCount; i++)
			camera[i]->captureImg();
			
		grayCount++;

		std::cout<<grayCount<<"\n";

		if(grayCount==grayCodes->getNumOfImgs())
			break;
		proj->showImg(grayCodes->getNextImg());
			
		key=cvWaitKey(10);
	
		if(key == 27)
			break;
				
	}
}


int canonScanMain(bool scanOnly)	
{
	std::cout << "Starting PSLS..." << std::endl;
	
	int projW = 1024;
	int projH = 768;

	grayCodes= new GrayCodes(projW,projH);
	grayCodes->load();
	
	projCalibBoard = cvLoadImage("scanCanon/cal.png");


	//init windows

	proj=new Projector(projW,projH);

	EdsInitializeSDK();

	CanonCamera *camera[5];

	camera[0] = new CanonCamera();

	//EdsError	 err = EDS_ERR_OK;

	//EdsCameraRef camera[5] ;//= NULL;
	
	int numOfCams =	camera[0]->getNumOfCams();
	
	for(int i=1; i<numOfCams; i++)
	{
		camera[i] = new CanonCamera();
	}

	
	proj->showImg(grayCodes->getImg(0));



	
	bool continue_val=true;

	if(!scanOnly)
	{
		for(int i=0; i<numOfCams; i++)
		{
			//cvShowImage("Projector Window",grayCodes->getImg(0));
			cvWaitKey(100);

			continue_val = captureCalib(camera[i]);

			if(!continue_val)
				break;

			proj->showImg(projCalibBoard);

			cvWaitKey(100);

			continue_val = captureCalib(camera[i]);
		
			if(!continue_val)
				break;

		}
	}


	if(continue_val)
	{
		proj->showImg(grayCodes->getNextImg());
		cvWaitKey(100);
		scan(camera, numOfCams);
	}
	
	for(int i=0; i<numOfCams; i++)
	{
		delete camera[i];
	}

	EdsTerminateSDK();


	///Destroy the window
	delete proj;
	
	return 0;


	


	
}




