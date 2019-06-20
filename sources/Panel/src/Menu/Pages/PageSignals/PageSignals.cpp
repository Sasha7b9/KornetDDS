#include "stdafx.h"
#ifndef WIN32
#include "Display/InputWindow.h"
#include "Settings/Settings.h"
#include "Menu/MenuItems.h"
#include "Menu/Pages/AddPageInput.h"
#include "Menu/Pages/Include/PageSignals.h"
#include "Generator/Signals.h"
#include "Generator/GeneratorPanel.h"
#include "Menu/Menu.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern const PageBase pageSignals;
Page *PageSignals::pointer = (Page *)&pageSignals;
extern ChoiceParameterBase cParameters;
/// ����� ������� ����� ������
static int numForm = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEF_CHOICE_2( cChannel,                                                                                                                                  //--- ��������� �������� - ����� ---
    "�����",
    "����� ������ ��� ���������."
    ,
    "A", "���������� ����������� ������� �� ������ A.",
    "B", "���������� ����������� ������� �� ������ B.",
    FLAG_1, BIT_CHANNEL, pageSignals, Item::FuncActive, PageSignals::OnPress_Channel, FuncDraw
)


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_8( cFormA,                                                                                                                                    //--- ��������� �������� - ����� ---
    "�����",
    "����� ����� �������."
    ,
    FORM_RU(Form::Sine),         "�����",
    FORM_RU(Form::RampPlus),     "����������� ����",
    FORM_RU(Form::RampMinus),    "��������� ����",
    FORM_RU(Form::Triangle),     "�����������",
    FORM_RU(Form::Meander),      "������",
    FORM_RU(Form::Impulse),      "�����������",
    FORM_RU(Form::PacketImpuls), "������",
    FORM_RU(Form::DDS),          "������������",
    numForm, pageSignals, Item::FuncActive, PageSignals::OnPress_Form, FuncDraw
)

DEF_CHOICE_7( cFormB,                                                                                                                                    //--- ��������� �������� - ����� ---
    "�����",
    "����� ����� �������."
    ,
    FORM_RU(Form::Sine),      "�����",
    FORM_RU(Form::RampPlus),  "����������� ����",
    FORM_RU(Form::RampMinus), "��������� ����",
    FORM_RU(Form::Triangle),  "�����������",
    FORM_RU(Form::Meander),   "������",
    FORM_RU(Form::Impulse),   "�����������",
    FORM_RU(Form::DDS),       "������������",
    numForm, pageSignals, Item::FuncActive, PageSignals::OnPress_Form, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static void OnPress_ChnageParameter()
{
    FORM_CURRENT->ChangeParameter();
}

DEF_BUTTON( bChangeParameter,                                                                                                        //--- ��������� �������� - ������ �������� ��������� ---
    "��������",
    "��������� ���� ����� ���������.",
    pageSignals, Item::FuncActive, OnPress_ChnageParameter, FuncDraw
)

DEF_CHOICE_PARAMETER( cParameters,                                                                                                                    //--- ��������� �������� - �������� ---
    "��������",
    "����� ��������� ��� ���������.",
    pageSignals, Item::FuncActive, OnPress_ChnageParameter, WAVE(Chan::A).GetForm(0)
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DEF_CHOICE_2( cTypeTune,                                                                                                                               //--- ��������� �������� - ������� ---
    "�������",
    "���������� ������������� ������� �������� �������� � ���������� ����������.",
    "����������", "������� �������� ������� ���������� ����������, ����� ������� ������ �������������.",
    "����������", "������� �������� ������� ���������� ��������� � ���������� ���������� �� ������.",
    FLAG_1, BIT_TUNE_FULL, pageSignals, Item::FuncActive, FuncChangedChoice, FuncDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
static bool OnKey_PageSignals(Control &key)
{
    if(key.Is(Control::Esc) && key.action.IsRelease())
    {
        return FORM_CURRENT->CloseOpenedParameter();
    }

    return false;
}

DEF_PAGE_8( pageSignals, //-V641 //-V1027
    "��������� ��������",
    "",
    &cChannel,                                      ///< ��������� �������� - �����
    &cFormA,                                        ///< ��������� �������� - �����
    &cParameters,                                   ///< ��������� �������� - ��������
    &bChangeParameter,                              ///< ��������� �������� - ������ �������� ���������
    PageSignals::PageLoad::pointer,                 ///< ��������� �������� - ��������
    &cTypeTune,                                     ///< ��������� �������� - �������
    PageSignals::PageCalibration::PageA::pointer,   ///< ��������� �������� - ���������� A
    PageSignals::PageCalibration::PageB::pointer,   ///< ��������� �������� - ���������� B
    Page::Settings, Menu::mainPage, Item::FuncActive, FuncPress, OnKey_PageSignals, FuncBeforeDraw
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageSignals::Init()
{
    OnPress_Channel(true);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageSignals::OnPress_Form(bool)
{
    ChoiceBase *choice = (ChoiceBase *)pageSignals.items[1];    // ��������� �� ChoiceBase, �������� ������ ��������� ����� �������� ������                        //-V1027     

    WAVE_CURRENT.SetIndexForm(choice->CurrentIndex());          // ���������� ��� �������� ������� ������ ����� �� ChoiceBase

    cParameters.form = FORM_CURRENT;

    Generator::TuneChannel(CURRENT_CHANNEL);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PageSignals::OnPress_Channel(bool)
{
    cParameters.form = FORM_CURRENT;
    numForm = FORM_CURRENT->value;
    pageSignals.items[1] = Chan(CURRENT_CHANNEL).IsA() ? (Item *)&cFormA : (Item *)&cFormB; //-V641
}
