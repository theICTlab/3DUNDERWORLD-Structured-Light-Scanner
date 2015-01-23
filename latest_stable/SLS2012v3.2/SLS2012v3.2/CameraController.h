//------------------------------------------------------------------------------------------------------------
//* Copyright © 2010-2013 Immersive and Creative Technologies Lab, Cyprus University of Technology           *
//* Link: http://ict.cut.ac.cy                                                                               *
//* Software developer(s): Kyriakos Herakleous                                                               *
//* Researcher(s): Kyriakos Herakleous, Charalambos Poullis                                                  *
//*                                                                                                          *
//* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.*
//* Link: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US                                        *
//------------------------------------------------------------------------------------------------------------

#pragma once

#include "WebCam.h"
#include "CanonCamera.h"

class CameraController
{
	public:
		CameraController(bool webFlag);

		~CameraController(void);

		void startLiveview();
		void endLiveview();

		void UpdateView();

		void captureImg();

		//store pictures on path, curently available only for webCam
		void captureImg(char* path);

		int getNumOfCams();

		bool isWebCam();
		bool isCanonCam();

		void resetSaveCount();

	private:
		bool web;
		WebCam *webCam;
		CanonCamera *canonCam;
};

