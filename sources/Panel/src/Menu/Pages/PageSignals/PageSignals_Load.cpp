#include "stdafx.h"
#ifndef WIN32
#include "Menu/Pages/Include/PageSignals.h"
#include "Display/Painter.h"
#include "FDrive/FDrivePanel.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageLoad;
Page *PageSignals::PageLoad::pointer = (Page *)&pageLoad;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_BUTTON( bUp,                                                                                            ///< ��������� �������� - �������� - �����
    "�����", "����",
    "", "",
    pageLoad, FuncActive, FDrive::PressUp, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_BUTTON( bDown,                                                                                           ///< ��������� �������� - �������� - ����
    "����", "Down",
    "", "",
    pageLoad, FuncActive, FDrive::PressDown, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_BUTTON(bEnter,                                                                                          ///< ��������� �������� - �������� - Enter
    "Enter", "Enter",
    "", "",
    pageLoad, FuncActive, FDrive::PressEnter, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_Choose()
{
}

DEF_BUTTON(bChoose,                                                                                    ///< ��������� �������� - �������� - ����������
    "�������", "Choose",
    "", "",
    pageLoad, FuncActive, OnPress_Choose, FuncDraw
)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool OnKey(Control)
{
    return false;
}

DEF_PAGE_4( pageLoad,                                                                                                  ///< ��������� �������� - ��������
    "��������", "LOAD",
    "", "",
    &bUp,       ///< ��������� �������� - �������� - �����
    &bDown,     ///< ��������� �������� - �������� - ����
    &bEnter,    ///< ��������� �������� - �������� - Enter
    &bChoose,   ///< ��������� �������� - �������� - ����������
    Page::Settings_Load, PageSignals::pointer, FuncActive, FuncPress, OnKey, FDrive::Draw
)
