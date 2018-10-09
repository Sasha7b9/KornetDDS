#pragma once
#include "Hardware/Controls.h"

#ifdef OPEN
#include "Hardware/stm746.h"
#else
#include "Hardware/stm429.h"
#endif

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4514)
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RTC_GET_PACKED_TIME() {}
#define RTC_SET_TIME_AND_DATA(d, m, y, h, min, s)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OPEN
class CPU : public STM746
#else
class CPU : public STM429
#endif
{
public:

    static void Init();

    static void Update();

    static float GetFPS();

    //----------------------------------------------------------------------------------------------------------------------------------- Keyboard ---
    class Keyboard
    {
    friend class CPU;

    private:

        static void Init();

    public:
        /// ��������� �������� ��� ����������
        static void SetCallback(void(*func)());

        static void InitInputs(uint16 sl[], char portSL[], int numSL, uint16 rl[], char portRL[], int numRL);
        /// ���������� true, ���� ����� ����
        static bool BufferIsEmpty();
        /// ���������� ��������� ����� ����������, ���� ������� �������
        static Control GetNextControl();
        /// �������-��������. ��� ���������, �� ������� ��� ���������� ���������� � ����� � �������� �� ������
        static void Draw();

        //----------------------------------------------------------------------------------------------------------------------------------- TIM4 ---
        class TIM4_
        {
        public:
            /// ������� ������, ������� ����������� � timeStop ��
            static void Start(uint timeStopMS);

            static void Stop();

            static void ElapsedCallback(void *htim);
        };

    private:

        static void Update();
    };

    //--------------------------------------------------------------------------------------------------------------------------------------- SPI4 ---
    // SPI ��� ����� � �������� �����������
    class SPI4_
    {
    friend class CPU;
    private:
        static void Init();
    public:
        /// ��������� ������ ������
        static void Transmit(uint8 *buffer, uint16 size);
        /// ��������� ������ ������ � ������������� ������ ������� �������
        static bool TransmitReceive(uint8 *trans, uint8 *receiv, uint16 size);
        /// ������� ������ ������
        static void Receive(uint8 *recv, uint16 size, uint timeOut);
        /// ���������� true, ���� ��������� ����� - ��������� �� ����� ���������� �������
        static bool IsBusy();
    };

    //-------------------------------------------------------------------------------------------------------------------------------------- GPIO ----
    class GPIO_
    {
    public:
        static void WritePin(char port, uint16 maskPin, bool state);

        static bool ReadPin(char port, uint16 maskPin);
    };

    //-------------------------------------------------------------------------------------------------------------------------------------- FLASH ---
    class FLASH_
    {
    public:
        static void SaveSettings();
        static void LoadSettings();
    };

    //--------------------------------------------------------------------------------------------------------------------------------------- FSMC ---
    // ���� ��� ����� � ����
    class FSMC
    {
    friend class CPU;

    private:
        static void Init();
    };

private:

    static void SystemClockConfig();
    /// ����� ������ ������ ��� (����� ������ ���������� 1-���������� ������� �������)
    static uint timeStartMeasFPS;
    /// ������� ������� ��������� � ������ ���������� 1-���������� ������� �������
    static int numFrames;

    static float fps;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

    /// ���������� �� ������. Timer.cpp
    void TIM3_IRQHandler();
    /// ���������� �� ����� ����������. CPU_Keyboard.cpp
    void TIM4_IRQHandler();

    void NMI_Handler();
    void HardFault_Handler();
    void MemManage_Handler();
    void BusFault_Handler();
    void UsageFault_Handler();
    void SVC_Handler();
    void DebugMon_Handler();
    void PendSV_Handler();
    void SysTick_Handler();
    
#ifdef __cplusplus
}
#endif


#ifdef WIN32
#pragma warning(pop)
#endif
