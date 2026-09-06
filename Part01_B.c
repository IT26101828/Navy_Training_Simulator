#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_ESCORTS 50
#define MAX_POINTS 20

#define PI 3.141592653589793
#define G 9.81

/* =========================
   STRUCTURES
   ========================= */

typedef struct
{
    int id;
    char type[3];

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;

    int destroyed;

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double vMax;

    double angleMin;
    double angleMax;

    int destroyed;

} Battleship;


/* =========================
   GLOBAL VARIABLES
   ========================= */

EscortShip E[MAX_ESCORTS];
Battleship B;

double pathX[MAX_POINTS];
double pathY[MAX_POINTS];

int N;
int k;

double canvasSize;


/* =========================
   RANDOM NUMBER FUNCTIONS
   ========================= */

double randomDouble(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}


/* =========================
   SET ESCORT PROPERTIES
   ========================= */

void setEscortProperties(EscortShip *ship)
{
    if (strcmp(ship->type, "EA") == 0)
    {
        ship->angleMin = 20;
        ship->impactPower = 0.08;
    }
    else if (strcmp(ship->type, "EB") == 0)
    {
        ship->angleMin = 30;
        ship->impactPower = 0.06;
    }
    else if (strcmp(ship->type, "EC") == 0)
    {
        ship->angleMin = 25;
        ship->impactPower = 0.07;
    }
    else if (strcmp(ship->type, "ED") == 0)
    {
        ship->angleMin = 50;
        ship->impactPower = 0.05;
    }
    else
    {
        strcpy(ship->type, "EE");

        ship->angleMin = 70;
        ship->impactPower = 0.04;
    }

    ship->angleMax = 90;
}


/* =========================
   GENERATE ESCORT SHIPS
   ========================= */

void generateEscortShips()
{
    int i;
    int typeNumber;

    for (i = 0; i < N; i++)
    {
        E[i].id = i + 1;

        typeNumber = randomInt(1, 5);

        switch (typeNumber)
        {
            case 1:
                strcpy(E[i].type, "EA");
                break;

            case 2:
                strcpy(E[i].type, "EB");
                break;

            case 3:
                strcpy(E[i].type, "EC");
                break;

            case 4:
                strcpy(E[i].type, "ED");
                break;

            case 5:
                strcpy(E[i].type, "EE");
                break;
        }

        setEscortProperties(&E[i]);

        E[i].x = randomDouble(0, canvasSize);
        E[i].y = randomDouble(0, canvasSize);

        E[i].vMin = randomDouble(20, 50);

        E[i].vMax = randomDouble(
            E[i].vMin + 10,
            B.vMax
        );

        E[i].destroyed = 0;
    }
}


/* =========================
   DISPLAY ESCORT SHIPS
   ========================= */

void displayEscortShips()
{
    int i;

    printf("\n========== ESCORT SHIPS ==========\n");

    for (i = 0; i < N; i++)
    {
        printf("\nE%d\n", E[i].id);

        printf("Type          : %s\n", E[i].type);

        printf("Position      : (%.2lf, %.2lf)\n",
               E[i].x,
               E[i].y);

        printf("Minimum V     : %.2lf\n",
               E[i].vMin);

        printf("Maximum V     : %.2lf\n",
               E[i].vMax);

        printf("Angle Range   : %.2lf - %.2lf\n",
               E[i].angleMin,
               E[i].angleMax);

        printf("Status        : %s\n",
               E[i].destroyed ?
               "DESTROYED" : "ACTIVE");
    }
}


/* =========================
   PROJECTILE CALCULATION
   ========================= */

/*
   Checks whether a projectile can travel
   from one point to another.

   Projectile equation:

   y = x tan(theta)
       - [g x^2 / (2 V^2 cos^2(theta))]

   We test angles from angleMin to angleMax.

*/

int canHit(
    double x1,
    double y1,
    double x2,
    double y2,
    double vMin,
    double vMax,
    double angleMin,
    double angleMax,
    double *time
)
{
    double dx;
    double dy;

    double angle;
    double theta;

    double denominator;
    double velocity;

    dx = x2 - x1;
    dy = y2 - y1;

    /*
       We use horizontal distance.
    */

    dx = fabs(dx);

    /*
       If the ships are directly above each other,
       avoid division problems.
    */

    if (dx < 0.000001)
    {
        dx = 0.000001;
    }

    for (angle = angleMin;
         angle <= angleMax;
         angle += 0.5)
    {
        theta = angle * PI / 180.0;

        denominator =
            2.0 *
            cos(theta) *
            cos(theta) *
            (dx * tan(theta) - dy);

        if (denominator > 0)
        {
            velocity =
                sqrt(
                    (G * dx * dx) /
                    denominator
                );

            if (velocity >= vMin &&
                velocity <= vMax)
            {
                *time =
                    dx /
                    (velocity * cos(theta));

                return 1;
            }
        }
    }

    return 0;
}


/* =========================
   GENERATE BATTLESHIP PATH
   ========================= */

void generatePath()
{
    int i;

    printf("\n========== BATTLESHIP PATH ==========\n");

    for (i = 0; i < k; i++)
    {
        pathX[i] = randomDouble(0, canvasSize);
        pathY[i] = randomDouble(0, canvasSize);

        printf("Point %d : (%.2lf, %.2lf)\n",
               i + 1,
               pathX[i],
               pathY[i]);
    }
}


/* =========================
   SAVE SIMULATION RESULT
   ========================= */

void saveResult(
    FILE *file,
    int iteration,
    double bx,
    double by,
    int destroyedByE,
    int destroyedECount
)
{
    fprintf(file,
            "\n----------------------------------\n");

    fprintf(file,
            "Iteration: %d\n",
            iteration);

    fprintf(file,
            "Battleship Position: (%.2lf, %.2lf)\n",
            bx,
            by);

    if (destroyedByE != -1)
    {
        fprintf(file,
                "Battleship destroyed by E%d\n",
                destroyedByE);
    }
    else
    {
        fprintf(file,
                "Escort ships destroyed by B: %d\n",
                destroyedECount);
    }
}


/* =========================
   SIMULATION 1
   ========================= */

void simulation1()
{
    int i;
    int j;

    int destroyedECount;

    double timeToHit;

    FILE *file;

    file = fopen("part1B_simulation1.txt", "w");

    if (file == NULL)
    {
        printf("Error opening result file.\n");
        return;
    }

    printf("\n====================================\n");
    printf("PART 1-B - SIMULATION 1\n");
    printf("====================================\n");

    fprintf(file,
            "PART 1-B - SIMULATION 1\n");

    fprintf(file,
            "========================\n");

    /*
       Start from the first point.
    */

    for (i = 0; i < k; i++)
    {
        B.x = pathX[i];
        B.y = pathY[i];

        destroyedECount = 0;

        printf("\nIteration %d\n", i + 1);

        printf("B position = (%.2lf, %.2lf)\n",
               B.x,
               B.y);

        /*
           First check whether an Escort Ship
           can hit the Battleship.
        */

        for (j = 0; j < N; j++)
        {
            if (E[j].destroyed)
                continue;

            if (canHit(
                E[j].x,
                E[j].y,
                B.x,
                B.y,
                E[j].vMin,
                E[j].vMax,
                E[j].angleMin,
                E[j].angleMax,
                &timeToHit))
            {
                B.destroyed = 1;

                printf("B was destroyed by E%d.\n",
                       E[j].id);

                saveResult(
                    file,
                    i + 1,
                    B.x,
                    B.y,
                    E[j].id,
                    0
                );

                fclose(file);

                return;
            }
        }

        /*
           If B survived, B attacks the Escort Ships.
        */

        for (j = 0; j < N; j++)
        {
            if (E[j].destroyed)
                continue;

            if (canHit(
                B.x,
                B.y,
                E[j].x,
                E[j].y,
                0,
                B.vMax,
                B.angleMin,
                B.angleMax,
                &timeToHit))
            {
                E[j].destroyed = 1;

                destroyedECount++;

                printf("B destroyed E%d.\n",
                       E[j].id);

                printf("Time to hit = %.2lf seconds\n",
                       timeToHit);
            }
        }

        saveResult(
            file,
            i + 1,
            B.x,
            B.y,
            -1,
            destroyedECount
        );
    }

    printf("\nSimulation 1 completed.\n");

    fprintf(file,
            "\nSimulation 1 completed.\n");

    fclose(file);
}


/* =========================
   RESET FOR SIMULATION 2
   ========================= */

void resetShips()
{
    int i;

    B.destroyed = 0;

    /*
       Restore all Escort Ships because
       Simulation 2 must use the same
       initial conditions as Simulation 1.
    */

    for (i = 0; i < N; i++)
    {
        E[i].destroyed = 0;
    }
}


/* =========================
   SIMULATION 2
   ========================= */

void simulation2()
{
    int i;
    int j;

    int jamIteration;

    int destroyedECount;

    double thetaMin;
    double timeToHit;

    FILE *file;

    printf("\n====================================\n");
    printf("PART 1-B - SIMULATION 2\n");
    printf("====================================\n");

    /*
       User chooses when the B gun jams.
    */

    printf("\nAfter how many iterations should the gun jam? ");
    scanf("%d", &jamIteration);

    if (jamIteration < 1)
        jamIteration = 1;

    if (jamIteration >= k)
        jamIteration = k - 1;

    /*
       Assignment says:
       0 < thetaMin < 30
    */

    thetaMin = randomDouble(1, 29);

    printf("B gun minimum angle after jamming = %.2lf degrees\n",
           thetaMin);

    file = fopen("part1B_simulation2.txt", "w");

    if (file == NULL)
    {
        printf("Error opening result file.\n");
        return;
    }

    fprintf(file,
            "PART 1-B - SIMULATION 2\n");

    fprintf(file,
            "========================\n");

    fprintf(file,
            "Gun jam iteration: %d\n",
            jamIteration);

    fprintf(file,
            "New minimum angle: %.2lf degrees\n",
            thetaMin);

    /*
       Reset everything so Simulation 2
       starts with the SAME initial state.
    */

    resetShips();

    for (i = 0; i < k; i++)
    {
        B.x = pathX[i];
        B.y = pathY[i];

        destroyedECount = 0;

        printf("\nIteration %d\n",
               i + 1);

        printf("B position = (%.2lf, %.2lf)\n",
               B.x,
               B.y);

        /*
           Before the jam:
           B can fire from 0 to 90 degrees.
        */

        if (i < jamIteration)
        {
            B.angleMin = 0;
            B.angleMax = 90;
        }
        else
        {
            /*
               After the gun jams:
               B can fire only from
               thetaMin to 90 degrees.
            */

            B.angleMin = thetaMin;
            B.angleMax = 90;

            printf("B gun is JAMMED.\n");
            printf("New angle range: %.2lf - 90 degrees\n",
                   B.angleMin);
        }

        /*
           E ships attack B.
        */

        for (j = 0; j < N; j++)
        {
            if (E[j].destroyed)
                continue;

            if (canHit(
                E[j].x,
                E[j].y,
                B.x,
                B.y,
                E[j].vMin,
                E[j].vMax,
                E[j].angleMin,
                E[j].angleMax,
                &timeToHit))
            {
                B.destroyed = 1;

                printf("B was destroyed by E%d.\n",
                       E[j].id);

                fprintf(file,
                        "\nIteration %d\n",
                        i + 1);

                fprintf(file,
                        "B position: (%.2lf, %.2lf)\n",
                        B.x,
                        B.y);

                fprintf(file,
                        "B destroyed by E%d\n",
                        E[j].id);

                fclose(file);

                return;
            }
        }

        /*
           B attacks E ships.
        */

        for (j = 0; j < N; j++)
        {
            if (E[j].destroyed)
                continue;

            if (canHit(
                B.x,
                B.y,
                E[j].x,
                E[j].y,
                0,
                B.vMax,
                B.angleMin,
                B.angleMax,
                &timeToHit))
            {
                E[j].destroyed = 1;

                destroyedECount++;

                printf("B destroyed E%d.\n",
                       E[j].id);

                printf("Time to hit = %.2lf seconds\n",
                       timeToHit);
            }
        }

        fprintf(file,
                "\n----------------------------------\n");

        fprintf(file,
                "Iteration: %d\n",
                i + 1);

        fprintf(file,
                "B Position: (%.2lf, %.2lf)\n",
                B.x,
                B.y);

        fprintf(file,
                "B Angle Range: %.2lf - %.2lf\n",
                B.angleMin,
                B.angleMax);

        fprintf(file,
                "E ships destroyed: %d\n",
                destroyedECount);
    }

    printf("\nSimulation 2 completed.\n");

    fprintf(file,
            "\nSimulation 2 completed.\n");

    fclose(file);
}


/* =========================
   DISPLAY FINAL RESULTS
   ========================= */

void displayFinalResults()
{
    int i;

    printf("\n====================================\n");
    printf("FINAL ESCORT SHIP STATUS\n");
    printf("====================================\n");

    for (i = 0; i < N; i++)
    {
        printf("E%d (%s) : %s\n",
               E[i].id,
               E[i].type,
               E[i].destroyed ?
               "DESTROYED" : "ACTIVE");
    }
}


/* =========================
   MAIN
   ========================= */

int main()
{
    int choice;

    srand((unsigned int)time(NULL));

    printf("============================================\n");
    printf("       ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("              PART 1-B\n");
    printf("============================================\n");

    /*
       Canvas size
    */

    printf("\nEnter canvas size D: ");
    scanf("%lf", &canvasSize);

    if (canvasSize <= 0)
    {
        printf("Invalid canvas size.\n");
        return 0;
    }

    /*
       Battleship type
    */

    printf("\nEnter Battleship type:\n");
    printf("U - USS Iowa\n");
    printf("M - MS King George V\n");
    printf("R - Richelieu\n");
    printf("S - Sovetsky Soyuz\n");

    printf("Enter type: ");
    scanf(" %c", &B.type);

    /*
       Battleship velocity
    */

    printf("\nEnter maximum velocity of B shell: ");
    scanf("%lf", &B.vMax);

    /*
       Starting position
    */

    printf("\nEnter initial B X coordinate: ");
    scanf("%lf", &B.x);

    printf("Enter initial B Y coordinate: ");
    scanf("%lf", &B.y);

    /*
       B can normally fire from
       0 to 90 degrees.
    */

    B.angleMin = 0;
    B.angleMax = 90;

    B.destroyed = 0;

    /*
       Number of Escort Ships
    */

    printf("\nEnter number of Escort Ships: ");
    scanf("%d", &N);

    if (N < 1)
        N = 1;

    if (N > MAX_ESCORTS)
        N = MAX_ESCORTS;

    /*
       Number of path points
    */

    printf("\nEnter number of path points k: ");
    scanf("%d", &k);

    if (k < 2)
        k = 2;

    if (k > MAX_POINTS)
        k = MAX_POINTS;

    /*
       Generate E ships.
    */

    generateEscortShips();

    /*
       Display initial battlefield.
    */

    displayEscortShips();

    /*
       Generate B path.
    */

    generatePath();

    /*
       Menu
    */

    do
    {
        printf("\n====================================\n");
        printf("             PART 1-B MENU\n");
        printf("====================================\n");

        printf("1. Run Simulation 1\n");
        printf("2. Run Simulation 2\n");
        printf("3. Display Escort Ships\n");
        printf("4. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:

                /*
                   Reset before Simulation 1.
                */

                resetShips();

                B.angleMin = 0;
                B.angleMax = 90;

                simulation1();

                break;

            case 2:

                /*
                   Simulation 2 uses the same
                   initial positions/path.
                */

                simulation2();

                break;

            case 3:

                displayEscortShips();

                break;

            case 4:

                printf("\nExiting...\n");

                break;

            default:

                printf("Invalid choice.\n");
        }

    } while (choice != 4);

    return 0;
}
