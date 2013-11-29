#pragma once

#include "resource.h"
#include "mypaint.h"
#include "stdafx.h"

#define MAX_LOADSTRING 100
#define BUTTON_COUNT 16
#define BUTTON_COUNT2 28
#define FIRST_BUTTON 600
#define FIRST_COLOR 700
//客户区矩形宽度与高度:
#define PAINTRECT_WIDTH 640
#define PAINTRECT_HEIGHT 480


struct  brush
{
	POINT PointFrom;			//起点坐标
	POINT PointTo;				//终点坐标
	COLORREF FrontColor;		//前景色
	COLORREF BackColor;			//背景色
	UINT MyStyle;				//笔刷粗细或形状虚实
};
