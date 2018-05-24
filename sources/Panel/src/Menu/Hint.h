#pragma once
#include "Menu/MenuItems.h"
#include "Hardware/Controls.h"


class Hint
{
public:
    /// ��������� ���������
    static void Draw();
    /// ����� ����������. ���������� false, ���� ��������� �� ���������
    static bool ProcessControl(StructControl strCtrl);

private:
    /// \brief ������������ ��������� � �����������
    /// start, end - ��������� � �������� ������ choice ��� ������
    /// calculate - ���� true, �� ��������� �� ������������ - ������ ������ ��������
    static int DrawDetailedHint(Choice *choice, int x, int y, int width, int start, int end, bool calculate = false);
    /// ���������� ����� ������� � ���������
    static int NumPagesInHint(Choice *choice, int x, int y, int width);
    /// ������������ ������, ����������� ��� �������� ��������� �� ��������
    static void Calcualte(Choice *choice, int x, int y, int width);
    /// ���� true - ����� �������� ��������� �� �����
    static bool show;
    /// �������, ��� �������� ����� �������� ���������
    static const Item *item;
    /// ����� ����������, ��� �������� ����� �������� ���������
    static Control control;
    /// ���������� ������� � ���������
    static int numPages;
    /// ������� �������������� ��������
    static int currentPage;
    /// � ������ ������ ���������� ������ ��������
    static int firstItemOnSecondPage;
    /// ������������� � true �������� ��������, ��� Item ��� ������ ��� �������� � ��������� �������� �� ��������
    static bool needCalculate;
};
