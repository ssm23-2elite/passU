// PictureEx.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PictureEx.h"

// CPictureEx

IMPLEMENT_DYNAMIC(CPictureEx, CStatic)

	CPictureEx::CPictureEx()
{
	Transparent = RGB(255, 255, 255);
}

CPictureEx::~CPictureEx()
{
}

BEGIN_MESSAGE_MAP(CPictureEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CPictureEx 메시지 처리기입니다.

void CPictureEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	HBITMAP old,bmp  = GetBitmap();
	BITMAP bminfo;
	CDC memDC;

	memDC.CreateCompatibleDC(&dc);
	old = (HBITMAP)::SelectObject(memDC.m_hDC,bmp);
	::GetObject(bmp,sizeof(BITMAP),&bminfo);
	::TransparentBlt(dc.m_hDC,0,0,bminfo.bmWidth,bminfo.bmHeight, memDC.m_hDC,0,0,bminfo.bmWidth,bminfo.bmHeight,Transparent);
	::SelectObject(memDC.m_hDC,old);
	memDC.DeleteDC();
	// 그리기 메시지에 대해서는 CStatic::OnPaint()을(를) 호출하지 마십시오.
}