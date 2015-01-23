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

#ifndef __SLS_2012_H__
#define __SLS_2012_H__

#include "cv.h"
#include <direct.h>

extern int proj_h;
extern int proj_w;
extern int black_threshold;
extern int white_threshold;
extern int webCamID ;
extern int cam_w;
extern int cam_h;
extern cv::Point2i projectorWinPos;
extern bool autoContrast;
extern bool saveAutoContrast;
extern bool raySampling;

#endif