#include "telemetry.h"
#include <string.h>

uint16_t telemetry_crc16(const uint8_t* data, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void telemetry_encode(const telemetry_sample_t* s, telemetry_frame_t* f, uint16_t seq)
{
    f->sync1 = 0xAA;
    f->sync2 = 0x55;
    f->seq = seq;
    f->sample = *s;

    uint8_t* raw = (uint8_t*)f;
    uint32_t len = sizeof(telemetry_frame_t) - sizeof(uint16_t);
    f->crc = telemetry_crc16(raw, len);
}
