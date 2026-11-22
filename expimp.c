//
// Created by Guilherme Martins on 21/11/2026.
//

#include "expimp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BIN_FILE_LENGTH
#define BIN_FILE_LENGTH 50
#endif



int hasExtension(const char* fileName, const char* extension)
{
    const char* end = strrchr(fileName, '.');
    if (end == NULL) return 0;
    return (strcmp(end, extension) == 0);
}

int exportParamsBin(const coord* cannonCoords, const coord* targetCoords,
                    const double initialVelocity, double length, const char filePath[]) {
    if (!hasExtension(filePath, ".dat")) return -2;

    FILE* file = fopen(filePath, "wb");

    if (file == NULL) {
        printf("ERROR with exporting parameters as a file.");
        return -1;
    }

    // cannonCoords       targetCoords
    // (64 + 64 + 64)  +  (64 + 64 + 64)
    // [truncated](unsinged char)(initialVel/40)   [truncated](unsigned char)(length)
    //  + 5                                          + 11
    // = 400 bits (/8 = 50 bytes)
    unsigned short truncPowder = (unsigned short)(initialVelocity / 40);
    unsigned short truncLength = (unsigned short)(length);

    fwrite(&cannonCoords->x, sizeof(double), 1, file);
    fwrite(&cannonCoords->y, sizeof(double), 1, file);
    fwrite(&cannonCoords->z, sizeof(double), 1, file);

    fwrite(&targetCoords->x, sizeof(double), 1, file);
    fwrite(&targetCoords->y, sizeof(double), 1, file);
    fwrite(&targetCoords->z, sizeof(double), 1, file);

    // to align everything, we're gonna do some nifty bit-shifting
    unsigned short powderAndLengthToWrite = (unsigned short)(((truncPowder << 11) & 0xF800u) | (truncLength & 0x07FFu));
    fwrite(&powderAndLengthToWrite, sizeof(powderAndLengthToWrite), 1, file);

    fclose(file);

    return 0;
}

SolutionParams* importParamsBin(const char filePath[]) {
    if (!hasExtension(filePath, ".dat")) return NULL;

    FILE* file = fopen(filePath, "rb");

    if (file == NULL) {
        printf("ERROR with importing parameters from a file.");
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    const long sz = ftell(file);
    if (sz != BIN_FILE_LENGTH) {
        fclose(file);
        return NULL;
    }

    rewind(file);

    SolutionParams* result = (SolutionParams*)malloc(sizeof(SolutionParams));
    if (result == NULL) {
        fclose(file);
        return NULL;
    }
    unsigned short unprocessedPowderAndLength;

    size_t readOk = 0;
    readOk += fread(&result->cannonX, sizeof(double), 1, file);
    readOk += fread(&result->cannonY, sizeof(double), 1, file);
    readOk += fread(&result->cannonZ, sizeof(double), 1, file);
    readOk += fread(&result->targetX, sizeof(double), 1, file);
    readOk += fread(&result->targetY, sizeof(double), 1, file);
    readOk += fread(&result->targetZ, sizeof(double), 1, file);
    readOk += fread(&unprocessedPowderAndLength, sizeof(unprocessedPowderAndLength), 1, file);

    fclose(file);

    if (readOk != 7) {
        free(result);
        return NULL;
    }

    // bit-shifting wizardry to save a whopping 1 byte

    result->initialVelocity = (double)((unprocessedPowderAndLength >> 11 & 0x1Fu) * 40u);
    result->length = (double)(unprocessedPowderAndLength & 0x7FFu);
    return result;
}