#pragma once
#include "Command.h"
#ifdef PANEL
#include "Generator/GeneratorPanel.h"
#else
#include "Generator/GeneratorDevice.h"
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SimpleMessage  // -V690
{
    friend class Transceiver;
public:
    explicit SimpleMessage();
    explicit SimpleMessage(uint size, uint8);
 
    ~SimpleMessage();

    void Create(uint size, uint8);
    /// ���������� ��������� �� ��������� �����
    SimpleMessage *Clone();
    /// ���������� ��������� ����������� ����������.
    void ResetPointer() { taken = 0; };

    bool CreateFromMessage(SimpleMessage *message);
    /// ������� ��������� � ���������� ������
    bool CreateAllocate(uint8 *buffer, uint size);
    /// �������� ����������� ���������� ������
    bool AllocateMemory(uint size);
    /// ����� ����
    uint8 TakeByte();
    /// ����� ���������
    uint16 TakeHalfWord();
    /// ����� �����
    uint TakeWord();

    uint64 TakeDoubleWord();

    float TakeFloat();
    /// ����� ���������� ������. ����� ������ ���� ������� �������� �� ��������� ������ ������
    void TakeRemainigData(uint8 *data);
    /// ���������� ��������� �� ���������� ������. ������ ����� ������� �����
    uint8 *RemainingData() const;
    /// ���������� ������
    void FreeMemory();
    /// ���������� ��������� �� ������
    uint8 *Data(int pos = 0);
    /// ���������� ������ ������
    uint Size() const;
    /// ���������� true, ���� ��� null-����� (������ �� ��������)
    bool IsEmpty() const;
    /// ���������� true, ���� ��������� ���������
    bool IsEquals(const SimpleMessage *message) const;
    /// ���������� ��������� �� pos-� �������. ������ ��� ����������� �����
    char *String(int pos);

    pString Trace();
    /// �������� �����
    void PutWord(uint data);
    
    /// �������� ��������� ����������
    void Transmit();
    /// �������� ������
    void PutData(uint8 *data, uint length);
protected:
    /// �������� ����
    void PutByte(uint8 data);
    /// �������� �������� �����
    void PutHalfWord(int16 data);

    void PutDoubleWord(uint64 data);

    void PutHalfWord(uint16 data);
    /// �������� float
    void PutFloat(float data);
    /// ������ ���������� ������
    uint allocated;
    /// ����� ������ ������, ���� 0, ���� ������ �� ��������
    uint8 *buffer;
    /// ������� ������ ������ ��������� �������. ������������ ��� Put
    uint used;
    /// �� ����� ������� ���������. ������������ ��� Take
    uint taken;

    SimpleMessage(const SimpleMessage &) : allocated(0), buffer(0), used(0), taken(0) {};
};


namespace Message
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ��������� ��������� - ��� ������ ����� ������������
    class RequestData : public SimpleMessage
    {
    public:
        RequestData();
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ��������� � �������
    class Log : public SimpleMessage
    {
    public:
        Log(char *string);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ��������� ������ �������
    class StartMode : public SimpleMessage
    {
    public:
        StartMode(uint8 ch, uint8 mode);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ������ � �������
    class WriteRegister : public SimpleMessage
    {
    public:
        WriteRegister(uint8 reg, uint64 data);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ��������� ����������� ������
    class DebugMode : public SimpleMessage
    {
    public:
        /// mode == 1/0 - ��������/��������� ����� �������
        DebugMode(uint8 mode);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ����� ���������
    class Reset : public SimpleMessage
    {
    public:
        Reset();
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ��������/��������� �����
    class EnableChannel : public SimpleMessage
    {
    public:
        /// ch : 0 - A, 1 - B
        /// enable : 0 - ���������, 1 - ��������
        EnableChannel(uint8 ch, uint8 enable);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class LoadFormDDS : public SimpleMessage
    {
    public:
        LoadFormDDS(uint8 ch, uint16 data[Generator::FPGA_NUM_POINTS]);
    };

    namespace FDrive
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� ��������� ���� � FPGA
        class LoadToFPGA : public SimpleMessage
        {
        public:
            LoadToFPGA(uint8 ch, uint8 numFile, char *directory);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ������ ����� �����
        class FileName : public SimpleMessage
        {
        public:
            FileName(uint8 numFile, char *name);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class FileString : public SimpleMessage
        {
        public:
            FileString(uint numString, char *nameFile);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ����� ��������� � ������ � ������� ��������
        class NumDirsAndFiles : public SimpleMessage
        {
        public:
            NumDirsAndFiles(uint numDirs, uint numFiles);
            NumDirsAndFiles(char *directory);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� � ���������������/�������������� �����
        class Mount : public SimpleMessage
        {
        public:
            /// mount :
            /// 0 - ������ �������������;
            /// 1 - ������ ������ ��������������
            /// 2 - ������ ������������� �� �������
            Mount(uint8 mount);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ������ ������� �����
        class FileSize : public SimpleMessage
        {
        public:
            FileSize(uint8 numFile, uint size);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class PictureDDS : public SimpleMessage
        {
        public:
            /// ������
            PictureDDS(uint8 numFile);
            /// �����
            PictureDDS(uint8 numFile, uint8 *data);
        };
    }

    namespace Set
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� ����� �������
        class FormWave : public SimpleMessage
        {
        public:
            FormWave(uint8 ch, uint8 form);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� �������
        class Frequency : public SimpleMessage
        {
        public:
            Frequency(uint8 ch, uint64 frequency);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� ���������
        class Amplitude : public SimpleMessage
        {
        public:
            Amplitude(uint8 ch, uint64 amplitude);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� �������� ������� �� ������
        class Offset : public SimpleMessage
        {
        public:
            Offset(uint8 ch, uint64 offset);
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ������������� ���������� ��� ������� ���������
        class Parameter : public SimpleMessage
        {
        public:
            Parameter(Command::E param, uint8 ch, uint64 value);
            Parameter(Command::E param, uint8 ch, uint8 choice);
        };
    }

    namespace FreqMeter
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ��������� ��������� �����������
        class Measure : public SimpleMessage
        {
        public:
            Measure(uint frequency);
        };
    }

    namespace SCPI
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ������ SCPI
        class Data : public SimpleMessage
        {
        public:
            Data(uint8 *data, uint length);
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class Calibrate : public SimpleMessage
    {
    public:
        Calibrate(uint8 ch, uint8 koeff);
    };
}
