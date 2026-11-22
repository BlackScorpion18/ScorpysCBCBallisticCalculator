#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "calculations.h"
#include "expimp.h"

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

    char buff[256];

    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                   "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

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
                   "-> exp: export firing solution to a .dat file\n"
                   "-> imp: import firing solution from a .dat file\n"
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

        if (strcmp(buff, "exp") == 0) {
            printf("Specify a file name (enter nothing to use the default name - SCBCBCparam.dat):\n");
            if (fgets(buff, sizeof(buff), stdin) == NULL) {
                break;                  // EOF: input closed
            }
            buff[strcspn(buff, "\n")] = '\0';

            int err;
            if (buff[0] == '\0') {
                err = exportParamsBin(cannonCoords, targetCoords, initialVelocity, cannonLength, "SCBCBCparam.dat");
            } else err = exportParamsBin(cannonCoords, targetCoords, initialVelocity, cannonLength, buff);

            switch (err) {
                case 0:
                    printf("File exported successfully!\n");
                    break;
                case -2:
                    printf("ERROR! Specified file is not a .dat file.\n");
                    break;
                default:
                    printf("ERROR! File exporting failed due to an unspecified error.\n");
                    break;
            }

            continue;
        }

        if (strcmp(buff, "imp") == 0) {
            printf("Specify a file name (enter nothing to use the default name - SCBCBCparam.dat):\n");
            if (fgets(buff, sizeof(buff), stdin) == NULL) {
                break;                  // EOF: input closed
            }
            buff[strcspn(buff, "\n")] = '\0';

            SolutionParams* params;
            if (buff[0] == '\0') {
                params = importParamsBin("SCBCBCparam.dat");
            } else params = importParamsBin(buff);

            if (params == NULL) {
                printf("ERROR! Failed to import the file. Verify the file's existence and extension and try again.\n");
            } else {
                cannonCoords->x = params->cannonX;
                cannonCoords->y = params->cannonY;
                cannonCoords->z = params->cannonZ;
                targetCoords->x = params->targetX;
                targetCoords->y = params->targetY;
                targetCoords->z = params->targetZ;
                initialVelocity = params->initialVelocity;
                cannonLength = params->length;
                printf("Parameters successfully imported from file!\n");
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