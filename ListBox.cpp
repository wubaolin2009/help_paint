#include"stdafx.h"
#include"ListBox.h"
#include "resource.h"
extern HINSTANCE hInst;
void DrawEraser(HDC hdc,int current)
{
	//There'are 4 erasers

	int top;  //top of each Rect
	int left = 18;
	int right = 22 ;
	int bottom;
	for(int i = 0;i<4;i++)
	{
		top = 4*(i+1) +12*i + (4 -i);
		bottom = top + 12 - (4-i);
		left --;
		right ++;
		Rectangle(hdc,left,top,right,bottom);
	}

	//restore the biggest rect
	top = 4*(current+1) +12*current;
	bottom = top + 12;
	left = 12;
	bottom = top + 12;
	
	/*Selected Rect */
	RECT rectFocus = {left,top,right,bottom};
	InvertRect(hdc,&rectFocus);
	

}


void DrawZoom(HDC hdc,int current)
{
	TCHAR sz1[] = TEXT("1x");
	TCHAR sz2[] = TEXT("2x");
	TCHAR sz3[] = TEXT("6x");
	TCHAR sz4[] = TEXT("8x");

	RECT rectFocus = {0,current*16,40,current*16 + 16};

	TextOut(hdc,12,0,sz1,2);
	TextOut(hdc,12,16,sz2,2);
	TextOut(hdc,12,32,sz3,2);
	TextOut(hdc,12,48,sz4,2);

	/*Selected Rect*/

	InvertRect(hdc,&rectFocus);
}


void DrawGun(HDC hdc,int current)
{
/*	TCHAR sz1[] = TEXT("1x");
	TCHAR sz2[] = TEXT("2x");
	TCHAR sz3[] = TEXT("3x");*/
	static HDC hdcmem = CreateCompatibleDC(hdc);
	static HBITMAP bitmap = LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BRUSH) );
	SelectObject(hdcmem,bitmap);
	BitBlt(hdc,0,0,40,75,hdcmem,0,0,SRCCOPY);

	RECT rectFocus = {0,current*25,40,current*25 + 25};

//	TextOut(hdc,12,0,sz1,2);
//	TextOut(hdc,12,16,sz2,2);
//	TextOut(hdc,12,32,sz3,2);

	/*Selected Rect*/

	InvertRect(hdc,&rectFocus);
}

void DrawBrush(HDC hdc,int current)
{
	Ellipse(hdc,15,3,25,13);
	Ellipse(hdc,13,17,27,31);
	Rectangle(hdc,15,35,25,45);
	Rectangle(hdc,13,49,27,63);
	RECT rectNow = {2,current*16,38,current*16 + 16};
	InvertRect(hdc,&rectNow);
}


void DrawLine(HDC hdc,int current)
{
	int iLineWide = current*2 + 2;   //LineWidth		
	HBRUSH hrush;
   
	RECT rectFocus = {0,current*12,36,current*12 + 12};

	int pos;  // Calculate the position of each line

	hrush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
	for(int i = 0;i<5;i++)
	{
		pos = 12*i + (12 - (i+1))/2 ;
	  	::Rectangle(hdc,4,pos ,36,pos + i +1);
	}	

	/*Selected Rect*/

	InvertRect(hdc,&rectFocus);

	SelectObject(hdc,hrush);
}

void DrawRect(HDC hdc,int current)
{
		//There'are 3 rect
	HBRUSH hb;
	HPEN   hp;  //backup 
	int top;  //top of each Rect
	int left = 2;
	int right = 2 + 34;
	int bottom;
	RECT rectFocus = {0,current*18,36,current*18 + 18};
	
/*1st Rect*/
	top = 4*1 ;
	bottom = top + 12;
	hb = (HBRUSH)SelectObject(hdc,::GetStockObject(NULL_BRUSH) );
	Rectangle(hdc,left,top,right,bottom);

/*2nd Rect*/
	 SelectObject(hdc,GetStockObject(GRAY_BRUSH) );
	top = 4*2 +12*1 ;
	bottom = top + 12;
	Rectangle(hdc,left,top,right,bottom);


/*3rd Rect*/
	hp = (HPEN)SelectObject(hdc,GetStockObject(NULL_PEN) );
	top = 4*3 +12*2 +2;
	bottom = top + 12;
	Rectangle(hdc,left,top,right,bottom);

/*Selected Rect*/

	InvertRect(hdc,&rectFocus);
	
/*Restore dc status*/
	SelectObject(hdc,hp);
	SelectObject(hdc,hb);
}



