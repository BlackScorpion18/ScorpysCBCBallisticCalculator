#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct {
    double x;
    double y;
    double z;
} coord;

typedef struct {
    double low;
    double high;
} PitchDouble;

typedef struct {
    double pitchLow;
    double pitchHigh;
    double yaw;
    double initialVelocity;
    double length;
    coord cannonCoords;
    coord targetCoords;
} FiringSolution;

// height of the shell above the target once it has covered the horizontal distance to it,
// fired at angle theta from the tip of a barrel of the given length pivoting on the mount
static double heightError(double theta, double dR, double dy, double v0, double length) {
    const double TERMINAL_V = 4.95;                  // (0.99 * 0.05) / (1.0 - 0.99)
    const double LN_DRAG = -0.01005033585350144;     // ln(0.99)
    double c = cos(theta);
    double r = dR - length * c;          // horizontal distance from barrel tip to target
    double h = dy - length * sin(theta); // target height above barrel tip
    return r * tan(theta) + (TERMINAL_V * r) / (v0 * c)
         - TERMINAL_V * (log(1.0 - r / (100.0 * v0 * c)) / LN_DRAG) - h;
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

void printFiringSolution(const FiringSolution* firingSolution) {
    if (firingSolution == NULL) {
        printf("ERROR: FIRING SOLUTION IS NULL\n");
        return;
    }

    printf("FIRING SOLUTION for coordinates:\n"
           "   CANNON: (%.1lf, %.1lf, %.1lf)\n"
           "   TARGET: (%.1lf, %.1lf, %.1lf)\n"
           "   LENGTH: %.0lf\n"
           "with a powder level of %.0lf (initial velocity = %.1lf m/s)\n"
           ">>> YAW: %.2lf\n",
           firingSolution->cannonCoords.x, firingSolution->cannonCoords.y, firingSolution->cannonCoords.z,
           firingSolution->targetCoords.x, firingSolution->targetCoords.y, firingSolution->targetCoords.z,
           firingSolution->length,
           (firingSolution->initialVelocity / 40), firingSolution->initialVelocity,
           firingSolution->yaw);

    if (isnan(firingSolution->pitchLow) && isnan(firingSolution->pitchHigh)) {
        printf(">>> TARGET OUT OF RANGE with the current powder level\n");
    } else {
        printf(">>> PITCH LOW-ANGLE: ");
        if (isnan(firingSolution->pitchLow)) {
            printf("NAN\n");
        } else printf("%.2lf\n", firingSolution->pitchLow);
        printf(">>> PITCH HIGH-ANGLE: ");
        if (isnan(firingSolution->pitchLow)) {
            printf("NAN\n");
        } else printf("%.2lf\n", firingSolution->pitchHigh);
    }
}

int main(int argc, char *argv[]) {
    coord* cannonCoords = malloc(sizeof(coord));
    coord* targetCoords = malloc(sizeof(coord));
    if (cannonCoords == NULL || targetCoords == NULL) {
        printf("ERROR: out of memory\n");
        return 1;
    }
    cannonCoords->x = 0;
    cannonCoords->y = 0;
    cannonCoords->z = 0;
    targetCoords->x = 100;
    targetCoords->y = 100;
    targetCoords->z = 100;
    int powderCharges = 1;
    double cannonLength = 1;
    double initialVelocity = 40;

    FiringSolution* firingSolution = NULL;

    char buff[128];
    printf("Welcome to Scorpy's CBC Ballistic Calculator!\n"
           "Type 'help' for help.\n");

    while (1) {
        printf("\nInput a command...\n");
        if (fgets(buff, sizeof(buff), stdin) == NULL) {
            break;                  // EOF: input closed
        }
        buff[strcspn(buff, "\n")] = '\0';

        if (buff[0] == '\0') {
            continue;               // ignore empty lines
        }

        if (strcmp(buff, "exit") == 0) {
            printf("Exiting...");
            break;
        }

        if (strcmp(buff, "clr") == 0) {
            printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
            continue;
        }

        if (strcmp(buff, "help") == 0) {
            printf("Scorpy's CBC Ballistic Calculator - Help\n\n"
                   "-> help: displays this menu\n"
                   "-> exit: exits the program\n"
                   "-> clr: clear screen\n"
                   "-> can: input new cannon coordinates\n"
                   "-> trg: input new target coordinates\n"
                   "-> pwd: input powder charge/big cartridge level (positive integer)\n"
                   "-> len: input cannon length (mount block to tip, both included)\n"
                   "-> run: compute firing solution (pitch and yaw)\n"
                   "-> disp: display last calculated firing solution\n"
                   "\n"
                   "Coordinates are Minecraft's (x,y,z) coordinates.\n"
                   "Values for the coordinates are initialized as (0,0,0) for the cannon, "
                   "and (100,100,100) for the target.\n");
            continue;
        }

        if (strcmp(buff, "can") == 0) {
            printf("CANNON - Input X:\n");
            scanf("%lf", &cannonCoords->x);
            printf("CANNON X is now %lf\n", cannonCoords->x);

            printf("CANNON - Input Y:\n");
            scanf("%lf", &cannonCoords->y);
            printf("CANNON Y is now %lf\n", cannonCoords->y);

            printf("CANNON - Input Z:\n");
            scanf("%lf", &cannonCoords->z);
            printf("CANNON Z is now %lf\n", cannonCoords->z);

            printf("CANNON coords set: (%lf, %lf, %lf)\n", cannonCoords->x, cannonCoords->y, cannonCoords->z);

            // clearing buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            continue;
        }

        if (strcmp(buff, "trg") == 0) {
            printf("TARGET - Input X:\n");
            scanf("%lf", &targetCoords->x);
            printf("TARGET X is now %lf\n", targetCoords->x);

            printf("TARGET - Input Y:\n");
            scanf("%lf", &targetCoords->y);
            printf("TARGET Y is now %lf\n", targetCoords->y);

            printf("TARGET - Input Z:\n");
            scanf("%lf", &targetCoords->z);
            printf("TARGET Z is now %lf\n", targetCoords->z);

            printf("TARGET coords set: (%lf, %lf, %lf)\n", targetCoords->x, targetCoords->y, targetCoords->z);

            // clearing buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            continue;
        }

        if (strcmp(buff, "pwd") == 0) {
            printf("POWDER - input charge number / cartridge powder level:\n");
            int ch;
            int r;
            while ((r = scanf("%d", &powderCharges)) != 1 || powderCharges <= 0) {
                if (r == EOF) {
                    break;
                }
                while ((ch = getchar()) != '\n' && ch != EOF) { }
                printf("ERROR: powder level must be a positive integer; try inputting again.\n");
            }
            if (r == EOF) {
                break;
            }

            initialVelocity = (powderCharges) * 40;
            printf("POWDER set to %d; initial projectile velocity will be %lf.\n", powderCharges, initialVelocity);

            // clearing buffer
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            continue;
        }

        if (strcmp(buff, "len") == 0) {
            printf("LENGTH - input cannon length (mount block to tip, both included):\n");
            double newLength;
            if (scanf("%lf", &newLength) == 1 && newLength >= 1) {
                cannonLength = newLength;
                printf("LENGTH set to %.0lf\n", cannonLength);
            } else {
                printf("ERROR: length must be a number of at least 1.\n");
            }

            // clearing buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            continue;
        }

        if (strcmp(buff, "run") == 0) {
            printf("COMPUTING FIRING SOLUTION...\n");
            FiringSolution* newSolution = computeFiringSolution(cannonCoords, targetCoords, initialVelocity, cannonLength);
            if (newSolution == NULL) {
                printf("ERROR WHILE COMPUTING A FIRING SOLUTION. Recheck inputted values and rerun.\n");
                continue;
            }
            free(firingSolution);
            firingSolution = newSolution;
            printf("COMPUTATIONS DONE\n");
            printFiringSolution(firingSolution);
            continue;
        }

        if (strcmp(buff, "disp") == 0) {
            if (firingSolution == NULL) {
                printf("No firing solution computed yet. Use 'run' first.\n");
            } else {
                printFiringSolution(firingSolution);
            }
            continue;
        }

        // default
        printf("Invalid input; please try again. If needed, type 'help' for help.\n");
    }

    free(firingSolution);
    free(cannonCoords);
    free(targetCoords);
    return 0;
}