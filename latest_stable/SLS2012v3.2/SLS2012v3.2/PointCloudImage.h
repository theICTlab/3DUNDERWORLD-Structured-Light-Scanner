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
#include "Utilities.h"

#include <iostream>
#include <fstream>

#define MAXSIZE (1600*1200)

class PointCloudImage
{
	public:

		PointCloudImage(int imageW,int imageH, bool color);
		~PointCloudImage(void);

		bool setPoint(int i_w, int j_h, cv::Point3f point, cv::Vec3f colorBGR);
		bool setPoint(int i_w, int j_h, cv::Point3f point);

		bool getPoint(int i_w, int j_h, cv::Point3f &pointOut);
		bool getPoint(int i_w, int j_h, cv::Point3f &pointOut, cv::Vec3f &colorOut);

		bool addPoint(int i_w, int j_h, cv::Point3f point, cv::Vec3f color);
		bool addPoint(int i_w, int j_h, cv::Point3f point);

		bool hasColor();

		void exportNumOfPointsPerPixelImg(char path[]);
		void exportXYZ(char *path,bool exportOffPixels=true, bool colorFlag=true);

		int getWidth();
		int getHeight();

	private:
		
		int w;
		int h;
		bool colorFlag;

		cv::Mat points;
		cv::Mat numOfPointsForPixel;
		cv::Mat color;
};

