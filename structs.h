//
// Created by Guilherme Martins on 21/11/2026.
//

#ifndef SCORPYSCBCBALLISTICCALCULATOR_STRUCTS_H
#define SCORPYSCBCBALLISTICCALCULATOR_STRUCTS_H

// a set of 3 doubles, which define a Minecraft coordinate.
// x is east-west, y is up down, z is south-north (positive-negative)
typedef struct {
    double x;
    double y;
    double z;
} coord;

// a set of the low-angle and high-angle pitches of a firing solution.
typedef struct {
    double low;
    double high;
} PitchDouble;

// a set of the low and high pitches, yaw, initial velocity, cannon length, and cannon and target coordinates.
typedef struct {
    double pitchLow;
    double pitchHigh;
    double yaw;
    double initialVelocity;
    double length;
    coord cannonCoords;
    coord targetCoords;
} FiringSolution;

typedef struct {
    double cannonX;
    double cannonY;
    double cannonZ;
    double targetX;
    double targetY;
    double targetZ;
    double initialVelocity;
    double length;
} SolutionParams;

#endif //SCORPYSCBCBALLISTICCALCULATOR_STRUCTS_H
