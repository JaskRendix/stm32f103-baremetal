#include "flight_sim.h"
#include <math.h>

static float t = 0.0F;
static float altitude = 0.0F;
static float velocity = 0.0F;
static float accel = 0.0F;

void simulate_flight_step(float dt, telemetry_sample_t* out)
{
    t += dt;

    /* Simple 1D rocket model */
    if (t < 2.0F)
    {
        /* Boost phase */
        accel = 25.0F; /* m/s² */
        out->flight_phase = 0;
    }
    else if (t < 6.0F)
    {
        /* Coast */
        accel = -9.81F;
        out->flight_phase = 1;
    }
    else if (velocity > 0.0F)
    {
        /* Approaching apogee */
        accel = -9.81F;
        out->flight_phase = 2;
    }
    else if (altitude > 0.0F)
    {
        /* Descent */
        accel = -9.81F * 0.3F; /* parachute drag */
        out->flight_phase = 3;
    }
    else
    {
        /* Landed */
        accel = 0.0F;
        velocity = 0.0F;
        altitude = 0.0F;
        out->flight_phase = 4;
    }

    velocity += accel * dt;
    altitude += velocity * dt;

    out->time_s = t;
    out->altitude_m = altitude;
    out->velocity_mps = velocity;
    out->accel_mps2 = accel;
    out->temperature_c = 20.0F + 0.01F * altitude;
    out->battery_v = 3.7F - 0.0001F * t;
}
