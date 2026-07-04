#include <stdio.h>
#include "telemetry.h"
#include "flight_sim.h"
#include "blackbox.h"

int main(void)
{
    telemetry_sample_t s;
    telemetry_frame_t f;
    uint16_t seq = 0;

    printf("=== Flight (live telemetry) ===\n");
    blackbox_erase();

    for (int i = 0; i < 200; i++)
    {
        simulate_flight_step(0.05F, &s);
        telemetry_encode(&s, &f, seq++);

        printf("SEQ=%u ALT=%.1f VEL=%.1f ACC=%.1f PHASE=%u CRC=0x%04X\n", f.seq,
               f.sample.altitude_m, f.sample.velocity_mps, f.sample.accel_mps2,
               f.sample.flight_phase, f.crc);

        blackbox_status_t st = blackbox_write_frame(&f);
        if (st != BLACKBOX_OK)
        {
            printf("!! black box write failed at seq %u (status %d)\n", f.seq, st);
            break;
        }
    }

    printf("\n=== Recovery pass (reading black box back from flash) ===\n");

    uint32_t recovered = 0;
    uint32_t corrupt = 0;
    uint32_t total = blackbox_frame_count();

    for (uint32_t i = 0; i < total; i++)
    {
        telemetry_frame_t out;
        blackbox_status_t st = blackbox_read_frame(i, &out);

        if (st == BLACKBOX_OK)
        {
            recovered++;
        }
        else
        {
            corrupt++;
            printf("!! frame %u failed recovery (status %d)\n", i, st);
        }
    }

    printf("Recovered %u/%u frames (%u corrupt)\n", recovered, total, corrupt);

    return (corrupt == 0) ? 0 : 1;
}