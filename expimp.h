//
// Created by Guilherme Martins on 21/11/2026.
//

#ifndef SCORPYSCBCBALLISTICCALCULATOR_EXPIMP_H
#define SCORPYSCBCBALLISTICCALCULATOR_EXPIMP_H

#include <stddef.h>

#include "structs.h"

/**
 * determines if a filename ends with a certain extension.
 * @param fileName the file name.
 * @param extension the file extension to be checked.
 * @return 1 if a filename ends with extension, 0 if not
 */
int hasExtension(const char* fileName, const char* extension);

/**
 * exports the parameters required for computing a firing solution to a .dat file.
 * @param cannonCoords a pointer to a struct containing the coordinates of the cannon.
 * @param targetCoords a pointer to a struct containing the coordinates of the target.
 * @param initialVelocity the velocity of the projectile as it leaves the cannon's barrel.
 * @param length the length of the cannon barrel, starting at and including the block the cannon mount's hinge.
 * @param filePath a string (char array) containing the file name and its path. file must be in the .dat extension.
 * @return 0 if the file is exported successfully, -1 if there is an error with fopen(), -2 if the file is not a .dat
 */
int exportParamsBin(const coord* cannonCoords, const coord* targetCoords,
               const double initialVelocity, double length, const char filePath[]);

/**
 * imports the parameters required for computing a firing solution from a .dat file.
 * @param filePath a string (char array) containing the file name and its path. file must be in the .dat extension.
 * @return the parameters needed for computing a solution, if the import is successful, NULL otherwise
 */
SolutionParams* importParamsBin(const char filePath[]);

#endif //SCORPYSCBCBALLISTICCALCULATOR_EXPIMP_H
