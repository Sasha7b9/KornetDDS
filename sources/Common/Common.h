#pragma once
#include "defines.h"


struct TypeForm
{
    enum E
    {
        Sine,           // �����
        RampPlus,       // ����+
        RampMinus,      // ����-
        Triangle,       // �����������
        Meander,        // ������
        Impulse,        // ��������
        PacketImpuls,   // ����� ���������
        Free,           // ������������ ������
        Count
    };
};


struct Order
{
    enum E
    {
        Mega,
        Kilo,
        One,
        Milli,
        Micro,
        Nano,
        Count
    };

    static pString Suffix(Order::E order, uint lang);

    // ���������� �������, �������������� �������
    static int GetPow10(Order::E order);
};


struct DoubleValue
{
    explicit DoubleValue(double v);

    // ���� �������� �� ���������� �������������. ��� ���� ������ �������� ����� ���� ���� ("+" ��� "-"), ������� ����� ���������� �� ����� ������ ("."),
    // � order ��������, �� ������� ����� ��������� �������� ����� (3 - �������� �� 1000, -3 - ��������� �� 1000)
    explicit DoubleValue(const char *const buffer, int order = 0);

    void FromUnits(int units, uint mUnits, uint uUnits, uint nUnits, int sign);
    void FromDouble(double v);
    void FromUINT64(uint64 v) { value = v; }
    void FromString(const char * const buffer, int order);

    double ToDouble() const;
    uint64 ToUINT64() const { return value; }

    void Div(uint div);
    void Mul(uint mul);
    void Add(DoubleValue value);
    void Sub(DoubleValue value);
    // �������� �� 10 � ������� pow
    void MulPow10(int pow);

    void SetSign(int sign);

    // ���������� ����
    int Sign() const;

    // ���������� ����� �����
    int Integer() const;
      
    // ���������� ���������� ���������� � ������� �����
    int FractNano() const;

    uint64 Abs() const;

    // ����������:
    // Order::Mega  - DoubleValue::Integer() >= 1e6
    // Order::Kilo  - DoubleValue::Integer() >= 1e3
    // Order::One   - DoubleValue::Integer() >= 0
    // Order::Milli - DoubleValue::Integer() >= 1e-3
    // Order::Micro - DoubleValue::Integer() >= 1e-6
    Order::E GetOrder() const;

    bool operator<(const DoubleValue &);
    bool operator>(const DoubleValue &);
    bool operator<=(const DoubleValue &);
    bool operator>=(const DoubleValue &);
    bool operator==(const DoubleValue &);

private:

    uint64 value;       // �������� ��������� � �������� ��������� "����". ������������� � "1" ������� ��� ��������, ��� ����� �������������
};
