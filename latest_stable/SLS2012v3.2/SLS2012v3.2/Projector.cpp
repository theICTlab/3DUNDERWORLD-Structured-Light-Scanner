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
#include "Projector.h"

int Projector::numOfProjectors=0;

Projector::Projector(int projW,int projH)
{
	Projector::numOfProjectors++;

	projNum=numOfProjectors;

	width=projW;
	height=projH;

	initProjectorWindow();
	
	
}

void Projector::initProjectorWindow()
{
	cvNamedWindow("Projector Window",CV_WINDOW_NORMAL);
	cvResizeWindow("Projector Window",width,height);
	cvMoveWindow("Projector Window", width+316, -20);
	cvSetWindowProperty("Projector Window", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}

void Projector::showImg(IplImage* img)
{
	cvShowImage("Projector Window", img );
}

void Projector::showImg(cv::Mat img)
{
	cv::imshow("Projector Window", img );
}

Projector::~Projector(void)
{
	cvDestroyWindow("Projector Window");
}
