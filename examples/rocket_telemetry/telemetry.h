#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>

/* Packed sample, with explicit padding to make size even */
typedef struct __attribute__((packed))
{
    float time_s;
    float altitude_m;
    float velocity_mps;
    float accel_mps2;
    float temperature_c;
    float battery_v;
    uint8_t flight_phase; /* 0=boost,1=coast,2=apogee,3=descent,4=landed */
    uint8_t _pad;         /* explicit padding: ensures sizeof(telemetry_sample_t) is even */
} telemetry_sample_t;

/* Packed frame: total size now even (32 bytes) */
typedef struct __attribute__((packed))
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
