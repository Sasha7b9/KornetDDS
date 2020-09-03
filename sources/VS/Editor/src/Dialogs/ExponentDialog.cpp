#include "defines.h"
#include "Form.h"
#include "Utils.h"
#include "Dialogs/ExponentDialog.h"
#include "Controls/SpinControl.h"
#pragma warning(push, 0)
#include <wx/spinctrl.h>
#include <wx/statline.h>
#pragma warning(pop)


enum
{
    ID_TEXTCTRL_POWER
};


wxPanel *ExponentDialog::CreatePanelPower()
{
    wxPanel *panel = new wxPanel(this);

    new wxStaticBox(panel, wxID_ANY, wxT("���������� �������"), wxDefaultPosition, wxSize(130, 75));

    scPower = new SpinControl(panel, wxID_ANY, wxT(""), { 20, 20 }, { 100, 20 }, 0, std::numeric_limits<int>::max(), 2000, this, wxCommandEventHandler(Dialog::OnControlEvent), wxT(""));

    return panel;
}


ExponentDialog::ExponentDialog() : Dialog(wxT("��������� ����������������� �������"), true)
{
    wxBoxSizer *vBox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hBoxPanels = new wxBoxSizer(wxHORIZONTAL);

    hBoxPanels->Add(CreatePanelPolarity());
    hBoxPanels->AddStretchSpacer();
    hBoxPanels->Add(CreatePanelPower());
    vBox->Add(hBoxPanels);

    SetBoxSizer(vBox, { 221, 80 });

    scPower->SetValue(1);
}


void ExponentDialog::SendAdditionForm()
{
    double tau = Math::LimitationBelow<double>(scPower->GetValue() + 1, 2.0);

    double x0 = Point::NUM_POINTS - 1;
    double y0 = Point::AVE;

    double x1 = x0 - 1.0;
    double y1 = y0 * (1.0 - 1.0 / tau);

    double k = (std::log(y0) - std::log(y1)) / (x0 - x1);

    int start = static_cast<int>(std::log(y0) / k - x0);

    for (int i = 0; i < Point::NUM_POINTS; i++)
    {
        double value = std::exp(k * (i + start));

        uint16 uValue = static_cast<uint16>(Point::AVE + value);

        data[i] = Math::Limitation<uint16>(uValue, Point::MIN, Point::MAX);
    }

    TheForm->SetAdditionForm(data);

    points.clear();
}
