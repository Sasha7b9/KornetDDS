#pragma once


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


struct FloatValue
{
    explicit FloatValue(float v);

    // ���� �������� �� ���������� �������������. ��� ���� ������ �������� ����� ���� ���� ("+" ��� "-"), ������� ����� ���������� �� ����� ������ ("."),
    // � order ��������, �� ������� ����� ��������� �������� ����� (3 - �������� �� 1000, -3 - ��������� �� 1000)
    explicit FloatValue(const char *const buffer, int order = 0);

    void FromUnits(int units, uint mUnits, uint uUnits, uint nUnits, int sign);

    void FromFloat(float v);

    void FromUINT64(uint64 v) { value = v; }

    static FloatValue FromNanoUnits(int units, uint64 nUnits);

    void FromString(const char * const buffer, int order);

    float ToFloat() const;

    uint64 ToUINT64() const { return value; }

    int Sign() const;

    uint64 Abs() const;

    void Divide(uint div);

    void Multiplie(uint mul);
    
    // ��������� �� �������� ��������
    void Add(float v);

    void Add(FloatValue value);

    void SetSign(int sign);
    
    // ���������� ����� ����� �����
    int Integer() const;
    
    // ���������� ������� ����� �����. numDigits - ��������� ���������� ��������� ����� �������
    int Fract(int numDigits) const;
    
    // ���������� ���������� ���������� � ������� �����
    int FractNano() const;

    bool operator<(const FloatValue &);
    bool operator>(const FloatValue &);
    bool operator<=(const FloatValue &);
    bool operator>=(const FloatValue &);

private:

    uint64 value;       // �������� ��������� � �������� ��������� "����". ������������� � "1" ������� ��� ��������, ��� ����� �������������
};
