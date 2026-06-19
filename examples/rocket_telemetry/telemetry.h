#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

typedef struct
{
    float time_s;
    float altitude_m;
    float velocity_mps;
    float accel_mps2;
    float temperature_c;
    float battery_v;
    uint8_t flight_phase; /* 0=boost,1=coast,2=apogee,3=descent,4=landed */
} telemetry_sample_t;

typedef struct
{
    uint8_t sync1;
    uint8_t sync2;
    uint16_t seq;
    telemetry_sample_t sample;
    uint16_t crc;
} telemetry_frame_t;

uint16_t telemetry_crc16(const uint8_t* data, uint32_t len);
void telemetry_encode(const telemetry_sample_t* s, telemetry_frame_t* f, uint16_t seq);

#endif
