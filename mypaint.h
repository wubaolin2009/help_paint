#pragma once

#include "resource.h"
#include "mypaint.h"
#include "stdafx.h"

#define MAX_LOADSTRING 100
#define BUTTON_COUNT 16
#define BUTTON_COUNT2 28
#define FIRST_BUTTON 600
#define FIRST_COLOR 700
//�ͻ������ο����߶�:
#define PAINTRECT_WIDTH 640
#define PAINTRECT_HEIGHT 480


struct  brush
{
	POINT PointFrom;			//�������
	POINT PointTo;				//�յ�����
	COLORREF FrontColor;		//ǰ��ɫ
	COLORREF BackColor;			//����ɫ
	UINT MyStyle;				//��ˢ��ϸ����״��ʵ
};
