//
// Created by Guilherme Martins on 21/11/2026.
//

#include "calculations.h"

#include <math.h>
#include <stdlib.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

static double heightError(const double theta, const double dR, const double dy,
                        const double initialVelocity, const double length) {
    const double TERMINAL_V = 4.95;                  // (0.99 * 0.05) / (1.0 - 0.99)
    const double LIN_DRAG = -0.01005033585350144;     // ln(0.99)
    double c = cos(theta);
    double r = dR - length * c;          // horizontal distance from barrel tip to target
    double h = dy - length * sin(theta); // target height above barrel tip
    return r * tan(theta) + (TERMINAL_V * r) / (initialVelocity * c)
         - TERMINAL_V * (log(1.0 - r / (100.0 * initialVelocity * c)) / LIN_DRAG) - h;
}

double computeYaw(const coord* cannonCoords, const coord* targetCoords) {
    double deltaX = targetCoords->x - cannonCoords->x;
    double deltaZ = targetCoords->z - cannonCoords->z;
    // in degrees: 0 = south, 90 = west, 180 = north, 270 = east
    double theta = atan2(-deltaX, deltaZ) * (180.0/PI);
    return fmod(theta + 360.0, 360.0);
}

PitchDouble* computePitch(const coord* cannonCoords, const coord* targetCoords,
    const double initialVelocity, const double length) {
    PitchDouble* result = malloc(sizeof(PitchDouble));
    if (result == NULL) {
        return NULL;
    }
    result->low = NAN;
    result->high = NAN;

    const double v0 = initialVelocity / 20.0;        // convert blocks/s to blocks/tick

    double dx = targetCoords->x - cannonCoords->x;
    double dy = targetCoords->y - cannonCoords->y;
    double dz = targetCoords->z - cannonCoords->z;
    double dR = sqrt(dx * dx + dz * dz);

    // check if target is beyond maximum possible horizontal range
    double ratio = dR / (100.0 * v0 + length);
    if (ratio >= 1.0) {
        return result;
    }

    double min_theta = -M_PI / 2.0 + 1e-4;
    double max_theta = acos(ratio) - 1e-6;

    // find peak elevation angle that maximizes height at distance dR
    const double inv_phi = 0.6180339887498949;
    double a = min_theta, b = max_theta;
    double c = b - (b - a) * inv_phi;
    double d = a + (b - a) * inv_phi;

    while (fabs(b - a) > 1e-6) {
        double y_c = heightError(c, dR, dy, v0, length);
        double y_d = heightError(d, dR, dy, v0, length);
        if (y_c > y_d) {
            b = d;
        } else {
            a = c;
        }
        c = b - (b - a) * inv_phi;
        d = a + (b - a) * inv_phi;
    }
    double peak_theta = 0.5 * (a + b);

    // check if the highest possible shot still falls below the target
    if (heightError(peak_theta, dR, dy, v0, length) < 0.0) {
        return result;
    }

    // bisection search on [min_theta, peak_theta] for low arc
    double low = min_theta, high = peak_theta;
    for (int i = 0; i < 60; i++) {
        double mid = 0.5 * (low + high);
        double err = heightError(mid, dR, dy, v0, length);

        if (fabs(err) < 1e-6 || (high - low) < 1e-8) {
            low = high = mid;
            break;
        }
        if (err < 0.0) {
            low = mid;
        } else {
            high = mid;
        }
    }
    result->low = 0.5 * (low + high) * (180.0 / M_PI);

    // bisection search on [peak_theta, max_theta] for high arc
    low = peak_theta;
    high = max_theta;
    for (int i = 0; i < 60; i++) {
        double mid = 0.5 * (low + high);
        double err = heightError(mid, dR, dy, v0, length);

        if (fabs(err) < 1e-6 || (high - low) < 1e-8) {
            low = high = mid;
            break;
        }

        if (err > 0.0) {
            low = mid;
        } else {
            high = mid;
        }
    }
    result->high = 0.5 * (low + high) * (180.0 / M_PI);

    return result;
}

FiringSolution* computeFiringSolution(const coord* cannonCoords, const coord* targetCoords,
    const double initialVelocity, double length) {
    FiringSolution* result = malloc(sizeof(FiringSolution));
    if (result == NULL) {
        return NULL;
    }
    result->cannonCoords = *cannonCoords;
    result->targetCoords = *targetCoords;
    result->initialVelocity = initialVelocity;
    result->length = length;
    result->yaw = computeYaw(cannonCoords, targetCoords);
    PitchDouble* pitchDouble = computePitch(cannonCoords, targetCoords, initialVelocity, length);
    if (pitchDouble == NULL) {
        free(result);
        return NULL;
    }
    result->pitchLow = pitchDouble->low;
    result->pitchHigh = pitchDouble->high;
    free(pitchDouble);

    return result;
}