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
	cv::namedWindow("Projector Window",cv::WINDOW_NORMAL);
	cv::resizeWindow("Projector Window",width,height);
	cv::moveWindow("Projector Window", width+316, -20);
	cv::setWindowProperty("Projector Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
}

// OpenCV_4
//void Projector::showImg(IplImage* img)
//{
//	cv::imshow("Projector Window", (cv::Mat)img );
//}

void Projector::showImg(cv::Mat img)
{
	cv::imshow("Projector Window", img );
}

Projector::~Projector(void)
{
	cv::destroyWindow("Projector Window");
}
