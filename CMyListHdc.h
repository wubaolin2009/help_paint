#ifndef _CMYLIST_HDC_H
#define _CMYLIST_HDC_H
#include <Windows.h>

enum Error_code
{
	ErrorFull,
	ErrorInvaildPointer,
	ErrorInvaildOperate
};

struct Node  
{

	HDC item;
	RECT rect;
	HBITMAP hbitMap;
	bool bCanDelete;
	Node* next;
	Node(){ next = NULL ; bCanDelete = false;}
	Node(const HDC& item,Node* next = NULL)
	{bCanDelete = false;}

};


class CMyListHdc
{
public:
	static const  int max_size_of_list;   //should be 4

public:
	CMyListHdc();

	void push(const HWND& , HDC&,RECT rect);

	bool bCanUndo()
	{	
		return bUndo;
	}

	bool bCanRestore()
	{
		return bRestore && iCheck;
	}

	void Undo(RECT& rect);

	void Clear();

	void Restore(RECT& rect);

	RECT  Top(const HWND& hwnd,HDC& item);

	HDC GetLast();
	
private:
	Node * m_head;

	Node * m_rear;
public:
	Node * m_current;

	Node * SetFormer(const Node* pNode);

	bool bUndo;

	bool bRestore;

	int iCheck;


	
};

#endif