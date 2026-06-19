#include <stdio.h>
#include "telemetry.h"
#include "flight_sim.h"

int main(void)
{
    telemetry_sample_t s;
    telemetry_frame_t f;
    uint16_t seq = 0;

    for (int i = 0; i < 200; i++)
    {
        simulate_flight_step(0.05F, &s);
        telemetry_encode(&s, &f, seq++);

        printf("SEQ=%u ALT=%.1f VEL=%.1f ACC=%.1f PHASE=%u CRC=0x%04X\n", f.seq,
               f.sample.altitude_m, f.sample.velocity_mps, f.sample.accel_mps2,
               f.sample.flight_phase, f.crc);
    }

    return 0;
}
