#include "Display.h"
#include "Painter.h"
#include "Globals.h"
#include "Font/Font.h"
#include "Utils/Math.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint8 color;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Painter_BeginScene(Color col)
{
    color = col;

    for (int x = 0; x < 320; ++x)
    {
        for (int y = 0; y < 240; ++y)
        {
            Painter_SetPoint(x, y);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawHLine(int y, int x0, int x1)
{
    for(int x = x0; x <= x1; ++x)
    {
        Painter_SetPoint(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawLine(int x1, int y1, int x2, int y2)
{
    if ((x2 - x1) == 0 && (y2 - y1) == 0)
    {
        ++x1;
    }
    int x = x1;
    int y = y1;
    int dx = (int)fabsf((float)(x2 - x1));
    int dy = (int)fabsf((float)(y2 - y1));
    int s1 = Math_Sign(x2 - x1);
    int s2 = Math_Sign(y2 - y1);
    int temp;
    int exchange = 0;
    if (dy > dx)
    {
        temp = dx;
        dx = dy;
        dy = temp;
        exchange = 1;
    }
    int e = 2 * dy - dx;
    int i = 0;
    for (; i <= dx; i++)
    {
        Painter_SetPoint(x, y);
        while (e >= 0)
        {
            if (exchange)
            {
                x += s1;
            }
            else
            {
                y += s2;
            }
            e = e - 2 * dx;
        }
        if (exchange)
        {
            y += s2;
        }
        else
        {
            x += s1;
        }
        e = e + 2 * dy;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawRectangle(int x, int y, int width, int height)
{
    Painter_DrawHLine(y, x, x + width);
    Painter_DrawHLine(y + height, x, x + width);
    Painter_DrawVLine(x, y, y + height);
    Painter_DrawVLine(x + width, y, y + height);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_DrawVLine(int x, int y0, int y1)
{
    for(int y = y0; y <= y1; ++y)
    {
        Painter_SetPoint(x, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_EndScene(void)
{
    Display_ToggleBuffers();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_FillRegion(int x, int y, int width, int height)
{
    for (int i = y; i <= y + height; ++i)
    {
        Painter_DrawHLine(i, x, x + width);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_RunDisplay(void)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetColor(Color col)
{
    color = col;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void CalculateCurrentColor(void)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
Color Painter_GetColor(void)
{
    return COLOR_FILL;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void Painter_SetPoint(int x, int y)
{
    if(x >= 0 && x < 320 && y >= 0 && y < 240)
    {
        *(Display_GetBuffer() + y * 320 + x) = color;
    }
}
