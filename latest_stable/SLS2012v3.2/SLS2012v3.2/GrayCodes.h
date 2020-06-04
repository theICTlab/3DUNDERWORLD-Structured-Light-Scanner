//------------------------------------------------------------------------------------------------------------
//* Copyright © 2010-2013 Immersive and Creative Technologies Lab, Cyprus University of Technology           *
//* Link: http://ict.cut.ac.cy                                                                               *
//* Software developer(s): Kyriakos Herakleous                                                               *
//* Researcher(s): Kyriakos Herakleous, Charalambos Poullis                                                  *
//*                                                                                                          *
//* This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.*
//* Link: http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US                                        *
//------------------------------------------------------------------------------------------------------------

#include "stdafx.h"

#ifndef __GRAY_CODES_H__
#define __GRAY_CODES_H__

#include "stdafx.h"
#include <iostream>
#include <fstream>
using std::ofstream;
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include "Utilities.h"

#define GRAY_MAX_NUM 100

namespace cv
{
	using std::vector;
};

class GrayCodes	{
	public: 
		///Constructor
		GrayCodes(int projW, int projH);

		///Destructor
		~GrayCodes();

		void unload();
		int getNumOfImgs();


		// OpenCV_4
		// IplImage* getNextImg();
		cv::Mat& getNextImg();

		// OpenCV_4
		//IplImage* getImg(int num);
		cv::Mat& getImg(int num);

		void generateGrays();

		void save();
		static int grayToDec(cv::vector<bool> gray);
		int GrayCodes::getNumOfRowBits();
		int GrayCodes::getNumOfColBits();
		
	protected:

		// OpenCV_4
		//IplImage* grayCodes[GRAY_MAX_NUM];
		//IplImage* colorCodes[GRAY_MAX_NUM];

		cv::Mat m_grayCodes[GRAY_MAX_NUM];
		cv::Mat m_colorCodes[GRAY_MAX_NUM];

		void calNumOfImgs();

		void allocMemForImgs();

		bool imgsLoaded;

		int numOfImgs;
		int numOfRowImgs;
		int numOfColImgs;

		int currentImgNum;
		
		int height;
		int width;
};

#endif