#include "stdafx.h"
#ifndef WIN32
#include "defines.h"
#include "log.h"
#include "Packet.h"
#include "Hardware/CPU.h"
#include <string.h>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint Packet::transcievedPackets = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Packet::Packet()
{
    
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::Create(uint8 *buffer, uint size)
{
    if (size <= MAX_SIZE_DATA_FIELD)
    {
        std::memcpy(DataField(), buffer, size);
        FillServiceField(size);
    }
    else
    {
        LOG_ERROR("������ �� ���������� � �����");
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *Packet::DataField()
{
    return (uint8 *)&data;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::FillServiceField(uint sizeData)
{
    WriteNumberPacket();
    WriteNumberSubPacket(0);
    WriteSizeData(sizeData);
    WriteCRC();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Packet::IsEquals(Packet *rhs) const
{
    return std::memcmp((void *)&data, &rhs->data, sizeof(Data)) == 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::WriteNumberPacket()
{
    uint *address = (uint *)(FromBegin(SIZE_FIELD_CRC));

    *address = transcievedPackets++;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::WriteNumberSubPacket(uint number)
{
    uint *address = (uint *)(FromBegin(SIZE_FIELD_CRC + SIZE_FIELD_NUMBER_PACKET));

    *address = number;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::WriteSizeData(uint size)
{
    uint8 *address = (uint8 *)(FromBegin(SIZE_FIELD_CRC + SIZE_FIELD_NUMBER_PACKET + SIZE_FIELD_NUMBER_SUBPACKET));

    *address = (uint8)size;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Packet::WriteCRC()
{
    uint crc32 = CPU::CRC32::Calculate(FromBegin(SIZE_FIELD_CRC), MAX_SIZE_DATA_FIELD);

    uint *address = (uint *)(FromBegin(0));

    *address = crc32;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *Packet::FromBegin(uint offset)
{
    return Begin() + offset;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8 *Packet::Begin()
{
    return data.data;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint Packet::Size() const
{
    return SIZE;
}
