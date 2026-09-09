#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_ESCORTS 50
#define MAX_POINTS 20

#define PI 3.141592653589793
#define G 9.81


/* =========================================
   STRUCTURES
   ========================================= */

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

    /*
       Time between two consecutive
       E ship firings.
    */
    double TE;

    double damage;

    int destroyed;

    /*
       Used to keep track of whether
       this E ship can currently fire.
    */
    int canFire;

} EscortShip;


typedef struct
{
    char type;

    double x;
    double y;

    double vMax;

    double angleMin;
    double angleMax;

    double damageReceived;

    int destroyed;

} Battleship;


/* =========================================
   GLOBAL VARIABLES
   ========================================= */

EscortShip E[MAX_ESCORTS];

Battleship B;

double pathX[MAX_POINTS];
double pathY[MAX_POINTS];

int N;
int k;

double D;

/*
   Time between B firings.
*/
double TB;


/* =========================================
   RANDOM NUMBER FUNCTIONS
   ========================================= */

double randomDouble(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX)
           * (max - min);
}


int randomInt(int min, int max)
{
    return min +
           rand() % (max - min + 1);
}


/* =========================================
   SET E SHIP PROPERTIES
   ========================================= */

void setEscortProperties(EscortShip *ship)
{
    if (ship->type[1] == 'A')
    {
        /*
           EA
        */
        ship->impactPower = 0.08;

        ship->angleMin = 20;
    }
    else if (ship->type[1] == 'B')
    {
        /*
           EB
        */
        ship->impactPower = 0.06;

        ship->angleMin = 30;
    }
    else if (ship->type[1] == 'C')
    {
        /*
           EC
        */
        ship->impactPower = 0.07;

        ship->angleMin = 25;
    }
    else if (ship->type[1] == 'D')
    {
        /*
           ED
        */
        ship->impactPower = 0.05;

        ship->angleMin = 50;
    }
    else
    {
        /*
           EE
        */
        ship->impactPower = 0.04;

        ship->angleMin = 70;
    }

    ship->angleMax = 90;
}


/* =========================================
   GENERATE ESCORT SHIPS
   ========================================= */

void generateEscortShips()
{
    int i;
    int typeNumber;

    for (i = 0; i < N; i++)
    {
        E[i].id = i + 1;

        /*
           Randomly select E type.
        */

        typeNumber = randomInt(1, 5);

        E[i].type[0] = 'E';
        E[i].type[2] = '\0';

        switch (typeNumber)
        {
            case 1:
                E[i].type[1] = 'A';
                break;

            case 2:
                E[i].type[1] = 'B';
                break;

            case 3:
                E[i].type[1] = 'C';
                break;

            case 4:
                E[i].type[1] = 'D';
                break;

            case 5:
                E[i].type[1] = 'E';
                break;
        }


        setEscortProperties(&E[i]);


        /*
           Random position.
        */

        E[i].x =
            randomDouble(0, D);

        E[i].y =
            randomDouble(0, D);


        /*
           Random minimum velocity.
        */

        E[i].vMin =
            randomDouble(10, 50);


        /*
           Maximum velocity.

           EA may have maximum velocity
           up to 1.2 * B maximum velocity.

           Other E types are below B.
        */

        if (E[i].type[1] == 'A')
        {
            E[i].vMax =
                randomDouble(
                    E[i].vMin + 1,
                    1.2 * B.vMax
                );
        }
        else
        {
            double maxVelocity;

            maxVelocity =
                B.vMax - 1;

            if (maxVelocity <= E[i].vMin)
            {
                E[i].vMin =
                    maxVelocity / 2.0;
            }

            E[i].vMax =
                randomDouble(
                    E[i].vMin + 1,
                    maxVelocity
                );
        }


        /*
           TE will be assigned later.
        */

        E[i].TE = 0;

        E[i].damage = 0;

        E[i].destroyed = 0;

        E[i].canFire = 1;
    }
}


/* =========================================
   SET E FIRING TIMES
   ========================================= */

void setEscortFiringTimes()
{
    int i;

    printf(
        "\n========================================\n"
    );

    printf(
        "       E SHIP FIRING INTERVALS\n"
    );

    printf(
        "========================================\n"
    );


    for (i = 0; i < N; i++)
    {
        /*
           Only set the value once for each
           E ship type.

           Each E ship of the same type
           uses the same TE.
        */

        if (E[i].type[1] == 'A')
        {
            E[i].TE = randomDouble(2, 8);
        }
        else if (E[i].type[1] == 'B')
        {
            E[i].TE = randomDouble(2, 8);
        }
        else if (E[i].type[1] == 'C')
        {
            E[i].TE = randomDouble(2, 8);
        }
        else if (E[i].type[1] == 'D')
        {
            E[i].TE = randomDouble(2, 8);
        }
        else
        {
            E[i].TE = randomDouble(2, 8);
        }
    }


    /*
       Make all ships of the same type
       have the same TE.
    */

    for (i = 0; i < N; i++)
    {
        int j;

        for (j = 0; j < i; j++)
        {
            if (E[i].type[1] ==
                E[j].type[1])
            {
                E[i].TE = E[j].TE;

                break;
            }
        }
    }


    /*
       Display TE values.
    */

    for (i = 0; i < N; i++)
    {
        int alreadyDisplayed = 0;
        int j;

        for (j = 0; j < i; j++)
        {
            if (E[i].type[1] ==
                E[j].type[1])
            {
                alreadyDisplayed = 1;

                break;
            }
        }

        if (!alreadyDisplayed)
        {
            printf(
                "Type E%c : TE = %.2lf seconds\n",
                E[i].type[1],
                E[i].TE
            );
        }
    }
}


/* =========================================
   DISPLAY E SHIPS
   ========================================= */

void displayEscortShips()
{
    int i;

    printf(
        "\n========================================\n"
    );

    printf(
        "          ESCORT SHIP INFORMATION\n"
    );

    printf(
        "========================================\n"
    );


    for (i = 0; i < N; i++)
    {
        printf(
            "\nE%d\n",
            E[i].id
        );

        printf(
            "Type          : %s\n",
            E[i].type
        );

        printf(
            "Position      : (%.2lf, %.2lf)\n",
            E[i].x,
            E[i].y
        );

        printf(
            "Vmin          : %.2lf\n",
            E[i].vMin
        );

        printf(
            "Vmax          : %.2lf\n",
            E[i].vMax
        );

        printf(
            "Angle Range   : %.2lf - %.2lf\n",
            E[i].angleMin,
            E[i].angleMax
        );

        printf(
            "Impact Power  : %.2lf\n",
            E[i].impactPower
        );

        printf(
            "TE            : %.2lf seconds\n",
            E[i].TE
        );

        printf(
            "Status        : %s\n",
            E[i].destroyed ?
            "DESTROYED" :
            "ACTIVE"
        );
    }
}


/* =========================================
   PROJECTILE CALCULATION
   ========================================= */

int canHit(
    double x1,
    double y1,

    double x2,
    double y2,

    double vMin,
    double vMax,

    double angleMin,
    double angleMax,

    double *timeToHit
)
{
    double dx;
    double dy;

    double angle;
    double theta;

    double denominator;
    double velocity;


    dx =
        fabs(x2 - x1);

    dy =
        y2 - y1;


    if (dx < 0.000001)
    {
        dx = 0.000001;
    }


    /*
       Try all angles in the allowed
       vertical angle range.
    */

    for (angle = angleMin;
         angle <= angleMax;
         angle += 0.5)
    {
        theta =
            angle * PI / 180.0;


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
                *timeToHit =
                    dx /
                    (velocity * cos(theta));

                return 1;
            }
        }
    }


    return 0;
}


/* =========================================
   B ATTACK STRATEGY
   ========================================= */

/*
   Higher impact power gets higher priority.

   Distance is used as a secondary factor.
*/

double calculatePriority(
    EscortShip *ship
)
{
    double distance;

    distance =
        sqrt(
            (ship->x - B.x) *
            (ship->x - B.x) +

            (ship->y - B.y) *
            (ship->y - B.y)
        );


    return
        (ship->impactPower * 1000.0) +
        (100.0 / (distance + 1.0));
}


/* =========================================
   FIND B ATTACK TARGETS
   ========================================= */

int findAttackTargets(
    int targets[]
)
{
    int i;

    int count = 0;

    double timeToHit;


    for (i = 0; i < N; i++)
    {
        if (E[i].destroyed)
        {
            continue;
        }


        if (canHit(
            B.x,
            B.y,

            E[i].x,
            E[i].y,

            0,
            B.vMax,

            B.angleMin,
            B.angleMax,

            &timeToHit
        ))
        {
            targets[count] = i;

            count++;
        }
    }


    return count;
}


/* =========================================
   SORT B ATTACK TARGETS
   ========================================= */

void sortAttackTargets(
    int targets[],
    int count
)
{
    int i;
    int j;

    int temp;

    double p1;
    double p2;


    for (i = 0; i < count - 1; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            p1 =
                calculatePriority(
                    &E[targets[i]]
                );

            p2 =
                calculatePriority(
                    &E[targets[j]]
                );


            if (p2 > p1)
            {
                temp =
                    targets[i];

                targets[i] =
                    targets[j];

                targets[j] =
                    temp;
            }
        }
    }
}


/* =========================================
   DISPLAY B ATTACK ORDER
   ========================================= */

void displayAttackOrder(
    int targets[],
    int count
)
{
    int i;

    if (count == 0)
    {
        printf(
            "No E ships are in B's attack range.\n"
        );

        return;
    }


    for (i = 0; i < count; i++)
    {
        printf(
            "%d. E%d (%s) - Priority %.2lf\n",

            i + 1,

            E[targets[i]].id,

            E[targets[i]].type,

            calculatePriority(
                &E[targets[i]]
            )
        );
    }
}


/* =========================================
   GENERATE B PATH
   ========================================= */

void generatePath()
{
    int i;

    printf(
        "\n========================================\n"
    );

    printf(
        "             BATTLESHIP PATH\n"
    );

    printf(
        "========================================\n"
    );


    for (i = 0; i < k; i++)
    {
        pathX[i] =
            randomDouble(0, D);

        pathY[i] =
            randomDouble(0, D);


        printf(
            "Point %d = (%.2lf, %.2lf)\n",

            i + 1,

            pathX[i],

            pathY[i]
        );
    }
}


/* =========================================
   RESET SIMULATION
   ========================================= */

void resetSimulation()
{
    int i;

    B.destroyed = 0;

    B.damageReceived = 0;


    for (i = 0; i < N; i++)
    {
        E[i].destroyed = 0;

        E[i].damage = 0;

        E[i].canFire = 1;
    }
}


/* =========================================
   E SHIPS ATTACK B CONTINUOUSLY
   ========================================= */

/*
   This function calculates how many times
   each E ship can fire during the given
   simulation time.

   An E ship can fire at:

       0
       TE
       2*TE
       3*TE
       ...

   until the specified end time.
*/

void continuousEAttack(
    double simulationTime,
    FILE *file
)
{
    int i;

    double time;

    double numberOfShots;

    int shots;

    double timeToHit;


    for (i = 0; i < N; i++)
    {
        if (E[i].destroyed)
        {
            continue;
        }


        /*
           Check whether E can hit B.
        */

        if (!canHit(
            E[i].x,
            E[i].y,

            B.x,
            B.y,

            E[i].vMin,
            E[i].vMax,

            E[i].angleMin,
            E[i].angleMax,

            &timeToHit
        ))
        {
            /*
               E cannot hit B from its
               current position.
            */

            continue;
        }


        /*
           Number of shots during
           simulation time.

           +1 includes the first shot
           at time 0.
        */

        numberOfShots =
            floor(
                simulationTime /
                E[i].TE
            ) + 1;


        shots =
            (int)numberOfShots;


        /*
           Fire continuously.
        */

        for (int shot = 0;
             shot < shots;
             shot++)
        {
            time =
                shot * E[i].TE;


            /*
               If B is already destroyed,
               stop.
            */

            if (B.destroyed)
            {
                return;
            }


            B.damageReceived +=
                E[i].impactPower * 100;


            printf(
                "Time %.2lf s: E%d fired at B -> %.2lf%% damage\n",

                time,

                E[i].id,

                E[i].impactPower * 100
            );


            printf(
                "B cumulative damage = %.2lf%%\n",

                B.damageReceived
            );


            if (file != NULL)
            {
                fprintf(
                    file,

                    "Time %.2lf s: E%d fired at B -> %.2lf%% damage\n",

                    time,

                    E[i].id,

                    E[i].impactPower * 100
                );


                fprintf(
                    file,

                    "B cumulative damage = %.2lf%%\n",

                    B.damageReceived
                );
            }


            /*
               Check if B is destroyed.
            */

            if (B.damageReceived >= 100)
            {
                B.destroyed = 1;


                printf(
                    "\nBATTLESHIP DESTROYED!\n"
                );


                if (file != NULL)
                {
                    fprintf(
                        file,

                        "\nBATTLESHIP DESTROYED!\n"
                    );
                }


                return;
            }
        }
    }
}


/* =========================================
   SIMULATION 1-A
   ========================================= */

void simulate1A()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;

    int destroyedCount = 0;

    double currentTime = 0;

    double timeToHit;

    FILE *file;


    file =
        fopen(
            "Part2B_Simulation_1A.txt",
            "w"
        );


    printf(
        "\n========================================\n"
    );

    printf(
        "      PART 2-B - SIMULATION 1-A\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 2-B - SIMULATION 1-A\n"
    );


    fprintf(
        file,
        "B firing interval TB = %.2lf\n\n",
        TB
    );


    /*
       E ships continuously attack B.

       For this stationary simulation,
       we first determine a reasonable
       battle period from B's attacks.
    */


    count =
        findAttackTargets(
            targets
        );


    sortAttackTargets(
        targets,
        count
    );


    printf(
        "\nB ATTACK ORDER:\n"
    );


    displayAttackOrder(
        targets,
        count
    );


    fprintf(
        file,
        "B ATTACK ORDER:\n"
    );


    for (i = 0; i < count; i++)
    {
        fprintf(
            file,

            "%d. E%d (%s)\n",

            i + 1,

            E[targets[i]].id,

            E[targets[i]].type
        );
    }


    /*
       B attacks in strategy order.

       E ships are continuously firing
       between B attacks.
    */

    for (i = 0; i < count; i++)
    {
        int index =
            targets[i];


        /*
           E ships get to fire during
           the current period.
        */

        continuousEAttack(
            currentTime,
            file
        );


        if (B.destroyed)
        {
            break;
        }


        /*
           B attacks E.
        */

        printf(
            "\nTime %.2lf s: B attacks E%d\n",

            currentTime,

            E[index].id
        );


        fprintf(
            file,

            "\nTime %.2lf s: B attacks E%d\n",

            currentTime,

            E[index].id
        );


        E[index].destroyed = 1;

        destroyedCount++;


        printf(
            "E%d DESTROYED.\n",

            E[index].id
        );


        fprintf(
            file,

            "E%d DESTROYED.\n",

            E[index].id
        );


        /*
           Time until next B shot.
        */

        currentTime += TB;
    }


    /*
       Allow E ships to fire until
       the final B attack time.
    */

    if (!B.destroyed)
    {
        continuousEAttack(
            currentTime,
            file
        );
    }


    printf(
        "\nE ships destroyed = %d\n",
        destroyedCount
    );


    printf(
        "B cumulative damage = %.2lf%%\n",
        B.damageReceived
    );


    fprintf(
        file,

        "\nE ships destroyed = %d\n",

        destroyedCount
    );


    fprintf(
        file,

        "B cumulative damage = %.2lf%%\n",

        B.damageReceived
    );


    fclose(file);
}


/* =========================================
   SIMULATION 1-B
   ========================================= */

void simulate1B()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;
    int j;

    double currentTime = 0;

    FILE *file;


    file =
        fopen(
            "Part2B_Simulation_1B.txt",
            "w"
        );


    printf(
        "\n========================================\n"
    );

    printf(
        "      PART 2-B - SIMULATION 1-B\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 2-B - SIMULATION 1-B\n"
    );


    fprintf(
        file,
        "TB = %.2lf seconds\n\n",
        TB
    );


    /*
       Move B through all k path points.

       This follows the Part 1-B approach
       where the simulation is repeated
       at each generated point.
    */

    for (i = 0; i < k; i++)
    {
        B.x =
            pathX[i];

        B.y =
            pathY[i];


        printf(
            "\n========== ITERATION %d ==========\n",
            i + 1
        );


        printf(
            "B position = (%.2lf, %.2lf)\n",

            B.x,

            B.y
        );


        fprintf(
            file,

            "\n========== ITERATION %d ==========\n",

            i + 1
        );


        fprintf(
            file,

            "B position = (%.2lf, %.2lf)\n",

            B.x,

            B.y
        );


        /*
           E ships continuously attack B
           during the current simulated time.
        */

        continuousEAttack(
            currentTime,
            file
        );


        if (B.destroyed)
        {
            printf(
                "\nB was destroyed at iteration %d.\n",
                i + 1
            );

            break;
        }


        /*
           Find E ships B can attack.
        */

        count =
            findAttackTargets(
                targets
            );


        sortAttackTargets(
            targets,
            count
        );


        printf(
            "\nB ATTACK ORDER:\n"
        );


        displayAttackOrder(
            targets,
            count
        );


        fprintf(
            file,

            "\nB ATTACK ORDER:\n"
        );


        for (j = 0; j < count; j++)
        {
            fprintf(
                file,

                "%d. E%d (%s)\n",

                j + 1,

                E[targets[j]].id,

                E[targets[j]].type
            );
        }


        /*
           B fires at the first available
           target according to its strategy.
        */

        if (count > 0)
        {
            int index =
                targets[0];


            printf(
                "Time %.2lf s: B destroyed E%d\n",

                currentTime,

                E[index].id
            );


            fprintf(
                file,

                "Time %.2lf s: B destroyed E%d\n",

                currentTime,

                E[index].id
            );


            E[index].destroyed = 1;
        }


        /*
           Move simulation time forward
           by B's firing interval.
        */

        currentTime += TB;
    }


    /*
       Final continuous E attack.
    */

    if (!B.destroyed)
    {
        continuousEAttack(
            currentTime,
            file
        );
    }


    printf(
        "\nFinal B damage = %.2lf%%\n",

        B.damageReceived
    );


    fprintf(
        file,

        "\nFinal B damage = %.2lf%%\n",

        B.damageReceived
    );


    fclose(file);
}


/* =========================================
   SIMULATION 1-C
   ========================================= */

void simulate1C()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;

    int destroyedCount = 0;

    double currentTime = 0;

    FILE *file;


    file =
        fopen(
            "Part2B_Simulation_1C.txt",
            "w"
        );


    printf(
        "\n========================================\n"
    );

    printf(
        "      PART 2-B - SIMULATION 1-C\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,

        "PART 2-B - SIMULATION 1-C\n"
    );


    fprintf(
        file,

        "TB = %.2lf seconds\n\n",

        TB
    );


    /*
       First find the E ships that can
       attack B.
    */

    continuousEAttack(
        currentTime,
        file
    );


    if (B.destroyed)
    {
        fclose(file);

        return;
    }


    /*
       Find B targets.
    */

    count =
        findAttackTargets(
            targets
        );


    sortAttackTargets(
        targets,
        count
    );


    printf(
        "\nB ATTACK ORDER:\n"
    );


    displayAttackOrder(
        targets,
        count
    );


    fprintf(
        file,

        "\nB ATTACK ORDER:\n"
    );


    for (i = 0; i < count; i++)
    {
        fprintf(
            file,

            "%d. E%d (%s)\n",

            i + 1,

            E[targets[i]].id,

            E[targets[i]].type
        );
    }


    /*
       B destroys E ships one by one.

       Unlike Part 1-C, E ships can now
       fire repeatedly.
    */

    for (i = 0; i < count; i++)
    {
        int index =
            targets[i];


        /*
           E ships fire during the time
           before B's next shot.
        */

        continuousEAttack(
            currentTime,
            file
        );


        if (B.destroyed)
        {
            break;
        }


        /*
           B destroys target E.
        */

        E[index].destroyed = 1;

        destroyedCount++;


        printf(
            "\nTime %.2lf s: B destroyed E%d\n",

            currentTime,

            E[index].id
        );


        fprintf(
            file,

            "\nTime %.2lf s: B destroyed E%d\n",

            currentTime,

            E[index].id
        );


        currentTime += TB;
    }


    /*
       Final E firing period.
    */

    if (!B.destroyed)
    {
        continuousEAttack(
            currentTime,
            file
        );
    }


    printf(
        "\n========================================\n"
    );

    printf(
        "FINAL RESULTS\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "B cumulative damage = %.2lf%%\n",

        B.damageReceived
    );


    printf(
        "E ships destroyed = %d\n",

        destroyedCount
    );


    fprintf(
        file,

        "\n========================================\n"
    );


    fprintf(
        file,

        "FINAL RESULTS\n"
    );


    fprintf(
        file,

        "B cumulative damage = %.2lf%%\n",

        B.damageReceived
    );


    fprintf(
        file,

        "E ships destroyed = %d\n",

        destroyedCount
    );


    fclose(file);
}


/* =========================================
   MAIN
   ========================================= */

int main()
{
    int choice;

    int timeChoice;

    int seed;


    /*
       Initial random seed.
    */

    srand(
        (unsigned int)time(NULL)
    );


    printf(
        "============================================\n"
    );

    printf(
        "        ADVANCED NAVAL BATTLE SIMULATOR\n"
    );

    printf(
        "                   PART 2-B\n"
    );

    printf(
        "============================================\n"
    );


    /*
       Canvas.
    */

    printf(
        "\nEnter canvas size D: "
    );

    scanf(
        "%lf",
        &D
    );


    /*
       Battleship type.
    */

    printf(
        "\nEnter Battleship type:\n"
    );

    printf(
        "U - USS Iowa\n"
    );

    printf(
        "M - MS King George V\n"
    );

    printf(
        "R - Richelieu\n"
    );

    printf(
        "S - Sovetsky Soyuz\n"
    );


    printf(
        "Enter type: "
    );

    scanf(
        " %c",
        &B.type
    );


    /*
       Battleship shell maximum velocity.
    */

    printf(
        "\nEnter B maximum shell velocity: "
    );

    scanf(
        "%lf",
        &B.vMax
    );


    /*
       B starting position.
    */

    printf(
        "\nEnter B starting X: "
    );

    scanf(
        "%lf",
        &B.x
    );


    printf(
        "Enter B starting Y: "
    );

    scanf(
        "%lf",
        &B.y
    );


    B.angleMin = 0;

    B.angleMax = 90;

    B.damageReceived = 0;

    B.destroyed = 0;


    /*
       Number of E ships.
    */

    printf(
        "\nEnter number of E ships N: "
    );

    scanf(
        "%d",
        &N
    );


    if (N < 1)
    {
        N = 1;
    }


    if (N > MAX_ESCORTS)
    {
        N = MAX_ESCORTS;
    }


    /*
       Number of path points.
    */

    printf(
        "\nEnter number of path points k: "
    );

    scanf(
        "%d",
        &k
    );


    if (k < 2)
    {
        k = 2;
    }


    if (k > MAX_POINTS)
    {
        k = MAX_POINTS;
    }


    /*
       Generate E ships.
    */

    generateEscortShips();


    /*
       Generate E firing intervals.
    */

    setEscortFiringTimes();


    /*
       Generate B path.
    */

    generatePath();


    /*
       =====================================
       B FIRING INTERVAL
       =====================================
    */

    printf(
        "\n========================================\n"
    );

    printf(
        "          B FIRING INTERVAL\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "1. Enter TB manually\n"
    );

    printf(
        "2. Generate TB randomly\n"
    );


    printf(
        "Enter choice: "
    );

    scanf(
        "%d",
        &timeChoice
    );


    if (timeChoice == 1)
    {
        printf(
            "Enter TB in seconds: "
        );

        scanf(
            "%lf",
            &TB
        );
    }
    else
    {
        TB =
            randomDouble(2, 10);

        printf(
            "Random TB = %.2lf seconds\n",
            TB
        );
    }


    /*
       Random seed.
    */

    printf(
        "\nEnter seed value: "
    );

    scanf(
        "%d",
        &seed
    );


    srand(
        seed
    );


    /*
       =====================================
       MAIN MENU
       =====================================
    */

    do
    {
        printf(
            "\n========================================\n"
        );

        printf(
            "              PART 2-B MENU\n"
        );

        printf(
            "========================================\n"
        );


        printf(
            "1. Simulate Part 1-A\n"
        );

        printf(
            "2. Simulate Part 1-B\n"
        );

        printf(
            "3. Simulate Part 1-C\n"
        );

        printf(
            "4. Display E ships\n"
        );

        printf(
            "5. Display firing intervals\n"
        );

        printf(
            "6. Exit\n"
        );


        printf(
            "\nEnter choice: "
        );

        scanf(
            "%d",
            &choice
        );


        switch (choice)
        {
            case 1:

                resetSimulation();

                simulate1A();

                break;


            case 2:

                resetSimulation();

                simulate1B();

                break;


            case 3:

                resetSimulation();

                simulate1C();

                break;


            case 4:

                displayEscortShips();

                break;


            case 5:

                printf(
                    "\nB firing interval:\n"
                );

                printf(
                    "TB = %.2lf seconds\n",
                    TB
                );


                printf(
                    "\nE firing intervals:\n"
                );


                {
                    int i;
                    int j;

                    for (i = 0; i < N; i++)
                    {
                        int alreadyDisplayed = 0;

                        for (j = 0; j < i; j++)
                        {
                            if (E[i].type[1] ==
                                E[j].type[1])
                            {
                                alreadyDisplayed = 1;

                                break;
                            }
                        }

                        if (!alreadyDisplayed)
                        {
                            printf(
                                "E%c = %.2lf seconds\n",

                                E[i].type[1],

                                E[i].TE
                            );
                        }
                    }
                }

                break;


            case 6:

                printf(
                    "\nProgram ended.\n"
                );

                break;


            default:

                printf(
                    "\nInvalid choice.\n"
                );
        }

    } while (choice != 6);


    return 0;
}
