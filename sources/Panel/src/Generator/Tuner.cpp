#include "defines.h"
#include "Display/Painter.h"
#include "Display/Text.h"
#include "Display/WaveGraphics.h"
#include "Generator/MathSupport.h"
#include "Generator/Parameters.h"
#include "Generator/Tuner.h"
#include "Generator/Wave.h"
#include "Settings/Settings.h"


using namespace Primitives;


Tuner::Tuner(ParameterValue *_param) : param(_param), indicator(param)
{

}


void Tuner::Draw()
{
    indicator.Draw();
}


bool Tuner::OnKeyControl(const Control)
{
    return false;
}


void Indicator::Draw()
{
    Chan ch = param->GetForm()->GetWave()->GetChannel();

    int x = WaveGraphics::X();
    int y = WaveGraphics::Y(ch.GetInverse());

    Rectangle(WaveGraphics::Width(), WaveGraphics::Height()).DrawFilled(x, y, Color::BLUE_10, Color::WHITE);

    DrawTitle(x, y, WaveGraphics::Width());

    DrawValue(x, y + 50);
}


void Indicator::DrawTitle(int x, int y, int width)
{
    Font::Store();

    Font::Set(TypeFont::_GOSTB20);

    Text(param->Name(LANGUAGE)).DrawInCenterRect(x, y, width, 30, Color::WHITE);

    Font::Restore();
}


void Indicator::DrawValue(int x, int y)
{
    Order::E order = Order::One;

    pString str = MathFloatValue::GetStringValue(param->value, param->IsSigned(), 10, &order);

    Text(str).Draw(x, y);
}
