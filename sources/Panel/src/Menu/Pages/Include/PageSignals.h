#pragma once
#include "Menu/MenuItems.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PageSignals
{
public:
    static void Init();

    static void OnPress_Channel(bool);

    static void OnPress_Form(bool);

    static Page *pointer;

    //------------------------------------------------------------------------------------------------------------------------------------------------
    class PageLoad
    {
    public:
        static Page *pointer;
    };
};