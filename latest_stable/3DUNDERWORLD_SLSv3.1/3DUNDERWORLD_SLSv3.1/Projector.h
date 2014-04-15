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

#include "cv.h"
#include "highgui.h"
#include "GrayCodes.h"

#include "SLS2012.h"

class Projector
{

	public:
	
		Projector(int projW,int projH);
	
		~Projector(void);

		int getHeight();
		int getWidth();

		void initProjectorWindow();

		void showImg(IplImage* img);
		void showImg(cv::Mat img);

	private:

		static int numOfProjectors;
		
		int projNum;
		int height;
		int width;

};

