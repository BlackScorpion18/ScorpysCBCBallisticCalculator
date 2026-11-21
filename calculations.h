//
// Created by Guilherme Martins on 21/11/2026.
//

#ifndef SCORPYSCBCBALLISTICCALCULATOR_CALCULATIONS_H
#define SCORPYSCBCBALLISTICCALCULATOR_CALCULATIONS_H


/**
 * computes the yaw needed for a firing solution.
 * @param cannonCoords a pointer to a struct containing the coordinates of the cannon.
 * @param targetCoords a pointer to a struct containing the coordinates of the target.
 * @return the yaw angle, in degrees.
 */
double computeYaw(const coord* cannonCoords, const coord* targetCoords)

/**
 * computes the height error, needed for the pitch calculations.
 * @param theta the angle the projectile is fired at.
 * @param dR the horizontal (x and z) distance between the cannon mount and the target.
 * @param dy the vertical (y) distance between the cannon mount and the target.
 * @param initialVelocity the velocity of the projectile as it leaves the cannon's barrel.
 * @param length the length of the cannon barrel, starting at and including the block the cannon mount's hinge.
 * @return height of the projectile above the target once it has covered the horizontal distance to it.
 */
static double heightError(const double theta, const double dR, const double dy,
                        const double initialVelocity, const double length);

/**
 * computes the low-angle and high-angle pitch values for a firing solution.
 * @param cannonCoords a pointer to a struct containing the coordinates of the cannon.
 * @param targetCoords a pointer to a struct containing the coordinates of the target.
 * @param initialVelocity the velocity of the projectile as it leaves the cannon's barrel.
 * @param length the length of the cannon barrel, starting at and including the block the cannon mount's hinge.
 * @return a pointer to a PitchDouble struct, containing both the low-angle and high-angle pitch values .
 */
PitchDouble* computePitch(const coord* cannonCoords, const coord* targetCoords,
                          const double initialVelocity, const double length) ;

/**
 * computes a firing solution by calling the computeYaw() and computePitch() functions.
 * @param cannonCoords a pointer to a struct containing the coordinates of the cannon.
 * @param targetCoords a pointer to a struct containing the coordinates of the target.
 * @param initialVelocity the velocity of the projectile as it leaves the cannon's barrel.
 * @param length the length of the cannon barrel, starting at and including the block the cannon mount's hinge.
 * @return a pointer to a FiringSolution struct.
 */
FiringSolution* computeFiringSolution(const coord* cannonCoords, const coord* targetCoords,
    const double initialVelocity, double length);

#endif //SCORPYSCBCBALLISTICCALCULATOR_CALCULATIONS_H
