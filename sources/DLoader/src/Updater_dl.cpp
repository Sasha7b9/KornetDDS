#include "defines.h"
#include "common/Command.h"
#include "common/Messages_pl.h"
#include "Updater_dl.h"
#include "FDrive/FDrive_dl.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Interface/Interface_dl.h"
#include <cstring>


/*

    ���������� ���������� ��������� �������.
    ��� ������� DLoader ������� ������� RequestUpdate. ���� ��� ������� � ������� ���������� �������, ����������� ������� ����������.
    ���� ������� ���������� ���.
    1. ������������, ���������� �� ������. ���� �� ���������� - �����.
    2. ����� �� ������ ����� G6-49-D.bat. ���� ���� ���������, ���������� ��������� �������.
    2.1 ������������, ������� �������� ����� �������.
    2.2 ���������� ��������� � ������ � ���, ��� ����� ������� �������� ��������.
    2.3 ����� ��������� �������� ���������� ��������� � ���, ��� �������� ���������.
    2.4 � ������ ���������� ��������� � ������ ������
    2.5 � ������ ������� ����������� �������� ���� � ����������� � ������������ � ������. ����� ������ ������ � ������ ���������� ��������� � ���, ����� ����� ���������� ���������.
    2.6 �� ��������� ������ � ������ ���������� ��������� � ���, ��� ���������� �������� Device ���������.

*/


#define FILE_NAME_DEVICE "G6-49-D.bin"
#define FILE_NAME_PANEL  "G6-49-P.bin"

static bool needUpgrade = false;

// ������ ���������� ���������
static void E(SimpleMessage *);

// ���������� ������� �� ����������
static void OnRequestUpdate(SimpleMessage *);

// ������� ��������� � ������� ��������� ���������� device
static void SendMessageAboutDevicePortion(int size, int fullSize);

// ������� ��������� � ������� ��������� ���������� panel. num - ����� "������" ������, data - "������" ������ �������� SIZE_CHUNK
static void SendMessageAboutPanelPortion(int num, uint8 *data, int size, int fullSize);

static int CalculatePortion(int size, int fullSize);


static void TransmitMessage(SimpleMessage *message);


void Updater::Handler(SimpleMessage *message)
{
    typedef void(*pFunc)(SimpleMessage *);

    static const pFunc funcs[Command::Count] =
    {
        /* RequestData               */ E,
        /* EnableChannel             */ E,
        /* SetFormWave               */ E,
        /* SetFrequency              */ E,
        /* SetAmplitude              */ E,
        /* SetOffset                 */ E,
        /* SetDuration               */ E,
        /* SetDutyRatio              */ E,
        /* SetPhase                  */ E,
        /* ModeDebug                 */ E,
        /* SetDelay                  */ E,
        /* WriteRegister             */ E,
        /* SetDurationRise           */ E,
        /* SetDurationFall           */ E,
        /* SetDurationStady          */ E,
        /* SetDutyFactor             */ E,
        /* SetManipulation           */ E,
        /* SetManipulationDuration   */ E,
        /* SetManipulationPeriod     */ E,
        /* SetPacketPeriod           */ E,
        /* SetPacketNumber           */ E,
        /* SetStartMode              */ E,
        /* SetPeriod                 */ E,
        /* SetPolarity               */ E,
        /* SetManipulationMode       */ E,
        /* LoadFromDDS               */ E,
        /* FreqMeasure               */ E,
        /* Log                       */ E,
        /* FDrive_NumDirsAndFiles    */ E,
        /* FDrive_Mount              */ E,
        /* FDrive_RequestDir         */ E,
        /* FDrive_RequestFile        */ E,
        /* Test                      */ E,
        /* SetKoeffCalibration       */ E,
        /* GetKoeffCalibration       */ E,
        /* FDrive_RequestFileSize    */ E,
        /* FDrive_RequestFileString  */ E,
        /* FDrive_LoadFromExtStorage */ E,
        /* FDrive_GetPictureDDS      */ E,
        /* SCPI_RecvData             */ E,
        /* PortCPU                   */ E,
        /* CalibrationLoad           */ E,
        /* CalibrationSet            */ E,
        /* StartApplication          */ E,
        /* RequestUpdate             */ OnRequestUpdate,
        /* PortionUpdateDevice       */ E,
        /* PortionUpgradePanel       */ E,
        /* EraseSectors              */ E
    };

    message->ResetPointer();

    uint8 com = message->TakeUINT8();

    if(com < Command::Count)
    {
        pFunc func = funcs[com];

        func(message);
    }
}


static void E(SimpleMessage *)
{

}


static void OnRequestUpdate(SimpleMessage *)
{
    needUpgrade = true;
}


void Updater::UpgradeDevice()
{
    const int fullSize = DLDrive::File::Open(FILE_NAME_DEVICE);

    if(fullSize != -1)
    {
        int numSectors = fullSize / (128 * 1024) + 1;

        HAL_EEPROM::EraseSectors(numSectors);

        uint address = MAIN_PROGRAM_START_ADDRESS;

        static uint8 buffer[SIZE_CHUNK];

        int size = fullSize;

        while(size > 0)
        {
            int readed = (size < SIZE_CHUNK) ? size : SIZE_CHUNK;
            size -= readed;

            DLDrive::File::Read(readed, buffer);

            HAL_EEPROM::WriteBuffer(address, buffer, readed);

            address += readed;

            SendMessageAboutDevicePortion(size, fullSize);
        }

        DLDrive::File::Close();
    }
}


void Updater::UpgradePanel()
{
    const int fullSize = DLDrive::File::Open(FILE_NAME_PANEL);

    if(fullSize != -1)
    {
        int numSectors = fullSize / (128 * 1024) + 1;

        Message::EraseSectors message(numSectors);
        TransmitMessage(&message);

        static uint8 buffer[SIZE_CHUNK];

        int size = fullSize;

        int numPortion = 0;

        while(size > 0)
        {
            int readed = (size < SIZE_CHUNK) ? size : SIZE_CHUNK;
            size -= readed;

            DLDrive::File::Read(readed, buffer);

            SendMessageAboutPanelPortion(numPortion++, buffer, size, fullSize);
        }

        DLDrive::File::Close();
    }
}


static void SendMessageAboutDevicePortion(int size, int fullSize)
{
    static int prevPortion = -1;        // ����� ����� ���������� ��� ���������. ���������� �� 0 �� 100. ���������� ������ ����� ����������

    int portion = CalculatePortion(size, fullSize);

    if(portion != prevPortion)
    {
        prevPortion = portion;

        Message::PortionUpgradeDevice message(portion);

        TransmitMessage(&message);
    }
}


static void SendMessageAboutPanelPortion(int num, uint8 *data, int size, int fullSize)
{
    int portion = CalculatePortion(size, fullSize);

    Message::PortionUpgradePanel message(num, portion, data, SIZE_CHUNK);

    TransmitMessage(&message);
}


static int CalculatePortion(int size, int fullSize)
{
    return static_cast<int>((1.0F - static_cast<float>(size) / fullSize) * 100);
}


bool Updater::NeedUpgrade()
{
    return needUpgrade;
}


static void TransmitMessage(SimpleMessage *message)
{
    message->Transmit();

    while(DInterface::GetOutbox().Size())
    {
        DInterface::Update();
    }
}
