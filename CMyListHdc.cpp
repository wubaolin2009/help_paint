#include "stdafx.h" 
#include "CMyListHdc.h"


const int CMyListHdc::max_size_of_list = 4;
CMyListHdc::CMyListHdc()
{
	//m_hWnd = hWnd;
	//HDC hdc = GetDC(hWnd);
	Node * node,* nodenext;
	//HBITMAP hbit;
	node = new Node();
	m_head = node;
	for(int i = 0 ;i< max_size_of_list -1 ;i++)
	{
		nodenext = new Node();
		node->next = nodenext;
		node = node ->next;
	}
	m_rear = node;
	m_rear->next = NULL;
	m_current = NULL;
	bUndo = false;
	bRestore = false;
	iCheck = 0;

/*	Node* head = m_head;
	for(int i = 0;i<max_size_of_list;i++)
	{
		hdc[i] = CreateCompatibleDC(hdc);
		hbit = CreateCompatibleBitmap(hdc,PAINTRECT_WIDTH,360);
		SelectObject(hdc[i],hbit);
		HPEN hPen = (HPEN)SelectObject(hdc[i],GetStockObject(WHITE_PEN));
		Rectangle(hdc[i],0,0,PAINTRECT_WIDTH,360);
		SelectObject(hdc[i],hPen);
		head->item = hdc[i];
		head = head->next;
	}
	ReleaseDC(hWnd,hdc);*/
};

void CMyListHdc::push(const HWND& hWnd , HDC& item,RECT rect)
{
	
	iCheck = 0;
	bUndo = true;
	bRestore = false;
	if(m_current == NULL)
	{
		m_current = m_head;
		bUndo= false;
	}
	else 
		m_current = m_current->next;
	if(m_current == NULL)
	{
		
		if(m_head->bCanDelete)
		{
			DeleteDC(m_head->item);
			DeleteObject(m_head->hbitMap);
			m_head->bCanDelete = false;
		}

		m_rear->next = m_head;
		m_head = m_head->next;
		m_rear = m_rear->next;
		m_current = m_rear;
		m_rear ->next = NULL;
	}

	if(m_current->bCanDelete)
	{
		DeleteDC(m_current->item);
		DeleteObject(m_current->hbitMap);
	}

	m_current->hbitMap = CreateCompatibleBitmap(item,rect.right - rect.left,rect.bottom - rect.top);
	m_current->item = CreateCompatibleDC(item);
	m_current->rect.left = rect.left;
	m_current->rect.right = rect.right;
	m_current->rect.top = rect.top;
	m_current->rect.bottom = rect.bottom;
	SelectObject(m_current->item,m_current->hbitMap);
	BitBlt(m_current->item,0,0,rect.right - rect.left,rect.bottom - rect.top,item,0,0,SRCCOPY);
	m_current->bCanDelete = true;	

}

HDC CMyListHdc::GetLast()
{
	return m_current->item;
}
RECT CMyListHdc::Top(const HWND& hwnd,HDC& item)
{			
		RECT rectt;
		HBITMAP hbitbac = CreateCompatibleBitmap(item,1,1);
		DeleteObject(SelectObject(item,hbitbac));
		DeleteDC(item);
		DeleteObject(hbitbac);
		item = CreateCompatibleDC(m_current->item);
		GetClientRect(hwnd,&rectt);
		hbitbac = CreateCompatibleBitmap(m_current->item,rectt.right - rectt.left,rectt.bottom - rectt.top);
		SelectObject(item,hbitbac);
		BitBlt(item,0,0,rectt.right - rectt.left,rectt.bottom - rectt.top,m_current->item,0,0,SRCCOPY);		
		return m_current->rect;
		//item= m_current->item;
}


void CMyListHdc::Undo(RECT& rect)
{
	if(!bUndo)
		throw (ErrorInvaildOperate);
	bRestore = true;
	iCheck++;
	m_current = SetFormer(m_current);	
	bUndo = (m_current == m_head)?false:true;
	rect.right = m_current->rect.right;
	rect.left = m_current->rect.left;
	rect.bottom = m_current->rect.bottom;
	rect.top = m_current->rect.top;
}


void CMyListHdc::Restore(RECT& rect)
{	
	if(!bRestore || !iCheck)
		throw (ErrorInvaildOperate);
	bUndo = true;
	iCheck --;
	if(iCheck == 0)
		bRestore = false;
	m_current = m_current->next;
	if(iCheck!= 0)
		bRestore = (m_current == m_rear)?false:true;
	rect.right = m_current->rect.right;
	rect.left = m_current->rect.left;
	rect.bottom = m_current->rect.bottom;
	rect.top = m_current->rect.top;

}

Node * CMyListHdc::SetFormer(const Node* pNode)
{
	if(pNode == m_head || m_head == NULL || pNode == NULL)
		throw (ErrorInvaildPointer);
	Node * node = m_head;
	while(node)
	{
		if(node ->next == pNode)
			return node;
		node = node ->next;
	}
	return NULL;
}

void CMyListHdc::Clear()
{
	Node* node = m_head;
	while(node != NULL)
	{
		DeleteDC(node->item);
		DeleteObject(node->hbitMap);
		node = node->next;
	}
	m_current = NULL;
	bUndo = false;
	bRestore = false;
	iCheck = 0;
}
