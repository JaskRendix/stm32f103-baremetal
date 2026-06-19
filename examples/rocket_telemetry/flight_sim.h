#ifndef FLIGHT_SIM_H
#define FLIGHT_SIM_H

#include "telemetry.h"

void simulate_flight_step(float dt, telemetry_sample_t* out);

#endif
