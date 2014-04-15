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

#include <iostream>
#include <fstream>
using std::ofstream;
#include "cv.h"
#include "highgui.h"
#include "GrayCodes.h"
#include "Projector.h"



class WebCam

{
	public:
		WebCam(int id=0,int camW=1600,int camH=1200);

		~WebCam(void);

		void startLiveview();
		void endLiveview();

		void UpdateView();

		void captureImg();
		void captureImg(char* dirPath);

		int getNumOfCams();
		void resetSaveCount();

	private:

		int saveCount;
		bool liveView;
		CvCapture *capture;
};

