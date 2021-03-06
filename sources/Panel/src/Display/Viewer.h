#pragma once


/*
    ������������ �������� ���������
*/


class Parameter;


class Viewer
{
public:

    Viewer(Parameter *param);

    // ���������� �������� �� ��������� � ����������� y. �������� ��������� ��������� � xName, �������� - � xValue, ������� ��������� - c xUnits
    void Draw(int y, int xName, int xValue, int xUnits, Chan::E ch);

    void DrawName(int x, int y, int width, Chan::E ch);

private:

    void DrawValue(int y, int xValue, int xUnits);

    void DrawChoice(int y, int xValue, int xUnits);

    void DrawComposite(int y, int xValue, int xUnits);

    void DrawButton(int y, int xValue, int xUnits);

    Parameter *param;
};
