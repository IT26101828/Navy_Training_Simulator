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

    double damage;

    int destroyed;
    int hasFired;

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
   Time between two consecutive B shots.
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
   SET ESCORT SHIP PROPERTIES
   ========================================= */

void setEscortProperties(EscortShip *ship)
{
    /*
       Impact power and minimum angle
       from Table 1.
    */

    if (ship->type[1] == 'A')
    {
        /* EA */

        ship->impactPower = 0.08;

        ship->angleMin = 20;
    }
    else if (ship->type[1] == 'B')
    {
        /* EB */

        ship->impactPower = 0.06;

        ship->angleMin = 30;
    }
    else if (ship->type[1] == 'C')
    {
        /* EC */

        ship->impactPower = 0.07;

        ship->angleMin = 25;
    }
    else if (ship->type[1] == 'D')
    {
        /* ED */

        ship->impactPower = 0.05;

        ship->angleMin = 50;
    }
    else
    {
        /* EE */

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
           Randomly select one of five
           escort ship types.
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
           Random position inside canvas.
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
           Random maximum velocity.

           EA can have a maximum velocity
           up to 1.2 * B maximum velocity.

           Other types must be below B
           maximum velocity.
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
            double maximumVelocity;

            maximumVelocity =
                B.vMax - 1;

            if (maximumVelocity <= E[i].vMin)
            {
                E[i].vMin =
                    maximumVelocity / 2.0;
            }

            E[i].vMax =
                randomDouble(
                    E[i].vMin + 1,
                    maximumVelocity
                );
        }


        E[i].damage = 0;

        E[i].destroyed = 0;

        E[i].hasFired = 0;
    }
}


/* =========================================
   DISPLAY ESCORT SHIPS
   ========================================= */

void displayEscortShips()
{
    int i;

    printf("\n========================================\n");
    printf("          ESCORT SHIP INFORMATION\n");
    printf("========================================\n");

    for (i = 0; i < N; i++)
    {
        printf("\nE%d\n", E[i].id);

        printf("Type          : %s\n",
               E[i].type);

        printf("Position      : (%.2lf, %.2lf)\n",
               E[i].x,
               E[i].y);

        printf("Vmin          : %.2lf\n",
               E[i].vMin);

        printf("Vmax          : %.2lf\n",
               E[i].vMax);

        printf("Angle Range   : %.2lf - %.2lf\n",
               E[i].angleMin,
               E[i].angleMax);

        printf("Impact Power  : %.2lf\n",
               E[i].impactPower);

        printf("Status        : %s\n",
               E[i].destroyed ?
               "DESTROYED" :
               "ACTIVE");
    }
}


/* =========================================
   PROJECTILE CALCULATION
   ========================================= */

/*
   Checks whether a ship can hit another ship.

   The target is checked for all possible
   angles within the attacker's allowed
   angle range.

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
    double *timeToHit
)
{
    double dx;
    double dy;

    double angle;
    double theta;

    double denominator;
    double velocity;


    dx = fabs(x2 - x1);

    dy = y2 - y1;


    if (dx < 0.000001)
    {
        dx = 0.000001;
    }


    /*
       Test possible firing angles.
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


            /*
               Check velocity range.
            */

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
   CALCULATE STRATEGY PRIORITY
   ========================================= */

/*
   CUSTOM STRATEGY

   Higher impact power = higher priority.

   If ships have similar impact powers,
   the closer ship gets higher priority.

   Therefore B tries to destroy the most
   dangerous E ships first.
*/

double calculatePriority(EscortShip *ship)
{
    double distance;

    double priority;


    distance =
        sqrt(
            (ship->x - B.x) *
            (ship->x - B.x) +

            (ship->y - B.y) *
            (ship->y - B.y)
        );


    /*
       Impact power is the main factor.

       Distance is used as a secondary factor.
    */

    priority =
        (ship->impactPower * 1000.0) +
        (100.0 / (distance + 1.0));


    return priority;
}


/* =========================================
   FIND E SHIPS IN B ATTACK RANGE
   ========================================= */

int findAttackTargets(int targets[])
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


        /*
           Check whether B can hit E.
        */

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
   SORT ATTACK TARGETS
   ========================================= */

void sortAttackTargets(int targets[], int count)
{
    int i;
    int j;

    int temp;

    double priorityI;
    double priorityJ;


    /*
       Selection-style sorting.

       Highest priority first.
    */

    for (i = 0; i < count - 1; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            priorityI =
                calculatePriority(
                    &E[targets[i]]
                );

            priorityJ =
                calculatePriority(
                    &E[targets[j]]
                );


            if (priorityJ > priorityI)
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
   DISPLAY ATTACK ORDER
   ========================================= */

void displayAttackOrder(
    int targets[],
    int count
)
{
    int i;

    printf(
        "\nB ATTACK ORDER\n"
    );

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
   GENERATE BATTLESHIP PATH
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

        E[i].hasFired = 0;
    }
}


/* =========================================
   SAVE ATTACK ORDER
   ========================================= */

void saveAttackOrder(
    FILE *file,
    int targets[],
    int count
)
{
    int i;


    fprintf(
        file,
        "B ATTACK ORDER\n"
    );


    if (count == 0)
    {
        fprintf(
            file,
            "No E ships in attack range.\n"
        );

        return;
    }


    for (i = 0; i < count; i++)
    {
        fprintf(
            file,
            "%d. E%d (%s) Priority = %.2lf\n",

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
   PART 2-A SIMULATION OF PART 1-A
   ========================================= */

void simulate1A()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;

    int destroyedCount = 0;

    double timeToHit;

    double currentTime = 0;

    FILE *file;


    file = fopen(
        "Part2A_Simulation_1A.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error creating file.\n"
        );

        return;
    }


    printf(
        "\n========================================\n"
    );

    printf(
        "       PART 2-A - SIMULATION 1-A\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 2-A - SIMULATION 1-A\n"
    );


    fprintf(
        file,
        "=========================\n\n"
    );


    fprintf(
        file,
        "B firing interval TB = %.2lf seconds\n\n",
        TB
    );


    /*
       First E ships attack B.

       Each E can attack only once.
    */

    printf(
        "\nE ships attacking B...\n"
    );


    for (i = 0; i < N; i++)
    {
        if (E[i].destroyed)
        {
            continue;
        }


        if (canHit(
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
            B.damageReceived +=
                E[i].impactPower * 100;


            E[i].hasFired = 1;


            printf(
                "E%d hit B -> %.2lf%% damage\n",

                E[i].id,

                E[i].impactPower * 100
            );


            printf(
                "B cumulative damage = %.2lf%%\n",

                B.damageReceived
            );


            fprintf(
                file,
                "E%d (%s) hit B\n",

                E[i].id,

                E[i].type
            );


            fprintf(
                file,
                "Damage = %.2lf%%\n",

                E[i].impactPower * 100
            );


            /*
               B is destroyed at 100%.
            */

            if (B.damageReceived >= 100)
            {
                B.destroyed = 1;


                printf(
                    "\nBATTLESHIP DESTROYED!\n"
                );


                fprintf(
                    file,
                    "\nBATTLESHIP DESTROYED.\n"
                );


                fclose(file);

                return;
            }
        }
        else
        {
            /*
               E has used its one attack.
            */

            E[i].hasFired = 1;


            printf(
                "E%d cannot hit B.\n",
                E[i].id
            );
        }
    }


    /*
       B survived.
       Now determine which E ships are
       inside B's attack range.
    */

    count =
        findAttackTargets(targets);


    /*
       Apply custom strategy.
    */

    sortAttackTargets(
        targets,
        count
    );


    displayAttackOrder(
        targets,
        count
    );


    saveAttackOrder(
        file,
        targets,
        count
    );


    /*
       B fires at one E every TB seconds.
    */

    for (i = 0; i < count; i++)
    {
        int index;

        index =
            targets[i];


        if (E[index].destroyed)
        {
            continue;
        }


        /*
           First shot occurs at currentTime.

           Subsequent shots are separated
           by TB seconds.
        */

        if (i > 0)
        {
            currentTime += TB;
        }


        E[index].destroyed = 1;

        destroyedCount++;


        B.damageReceived =
            B.damageReceived;


        printf(
            "\nTime %.2lf s: B attacks E%d\n",

            currentTime,

            E[index].id
        );


        printf(
            "E%d DESTROYED.\n",
            E[index].id
        );


        fprintf(
            file,
            "\nTime %.2lf seconds: B attacks E%d\n",

            currentTime,

            E[index].id
        );


        fprintf(
            file,
            "E%d DESTROYED.\n",

            E[index].id
        );
    }


    /*
       Display final result.
    */

    printf(
        "\n========================================\n"
    );

    printf(
        "SIMULATION 1-A COMPLETED\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "E ships destroyed = %d\n",
        destroyedCount
    );


    printf(
        "Battle completion time = %.2lf seconds\n",
        currentTime
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
        "E ships destroyed = %d\n",
        destroyedCount
    );


    fprintf(
        file,
        "Battle completion time = %.2lf seconds\n",
        currentTime
    );


    fclose(file);
}


/* =========================================
   PART 2-A SIMULATION OF PART 1-B
   ========================================= */

void simulate1B()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;
    int j;

    int destroyedCount;

    double timeToHit;

    double currentTime = 0;

    FILE *file;


    file = fopen(
        "Part2A_Simulation_1B.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error creating file.\n"
        );

        return;
    }


    printf(
        "\n========================================\n"
    );

    printf(
        "       PART 2-A - SIMULATION 1-B\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 2-A - SIMULATION 1-B\n"
    );


    fprintf(
        file,
        "=========================\n"
    );


    fprintf(
        file,
        "B firing interval TB = %.2lf seconds\n\n",
        TB
    );


    /*
       B moves through all k path points.
    */

    for (i = 0; i < k; i++)
    {
        /*
           Move B to next path point.
        */

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
           E ships attack B.

           Each E can attack only once.
        */

        for (j = 0; j < N; j++)
        {
            if (E[j].destroyed)
            {
                continue;
            }


            if (E[j].hasFired)
            {
                continue;
            }


            if (canHit(
                E[j].x,
                E[j].y,

                B.x,
                B.y,

                E[j].vMin,
                E[j].vMax,

                E[j].angleMin,
                E[j].angleMax,

                &timeToHit
            ))
            {
                B.damageReceived +=
                    E[j].impactPower * 100;


                E[j].hasFired = 1;


                printf(
                    "E%d hit B -> %.2lf%% damage\n",

                    E[j].id,

                    E[j].impactPower * 100
                );


                fprintf(
                    file,
                    "E%d hit B -> %.2lf%% damage\n",

                    E[j].id,

                    E[j].impactPower * 100
                );


                /*
                   Check B destruction.
                */

                if (B.damageReceived >= 100)
                {
                    B.destroyed = 1;


                    printf(
                        "\nBATTLESHIP DESTROYED!\n"
                    );


                    fprintf(
                        file,
                        "\nBATTLESHIP DESTROYED.\n"
                    );


                    fclose(file);

                    return;
                }
            }
            else
            {
                /*
                   E uses its one opportunity.
                */

                E[j].hasFired = 1;
            }
        }


        /*
           Find E ships currently in B's
           attack range.
        */

        count =
            findAttackTargets(
                targets
            );


        /*
           Apply our strategy.
        */

        sortAttackTargets(
            targets,
            count
        );


        /*
           Show attack order for this iteration.
        */

        printf(
            "\nAttack order for iteration %d:\n",
            i + 1
        );


        displayAttackOrder(
            targets,
            count
        );


        fprintf(
            file,
            "\nAttack order:\n"
        );


        saveAttackOrder(
            file,
            targets,
            count
        );


        destroyedCount = 0;


        /*
           B attacks E ships in strategic order.
        */

        for (j = 0; j < count; j++)
        {
            int index;

            index =
                targets[j];


            if (E[index].destroyed)
            {
                continue;
            }


            /*
               B fires at the target.
            */

            if (j > 0)
            {
                currentTime += TB;
            }


            E[index].destroyed = 1;

            destroyedCount++;


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
        }


        fprintf(
            file,
            "E ships destroyed this iteration: %d\n",

            destroyedCount
        );
    }


    /*
       All k iterations completed.
    */

    printf(
        "\nAll %d path iterations completed.\n",
        k
    );


    printf(
        "B cumulative damage = %.2lf%%\n",
        B.damageReceived
    );


    fprintf(
        file,
        "\nAll %d path iterations completed.\n",
        k
    );


    fprintf(
        file,
        "B cumulative damage = %.2lf%%\n",
        B.damageReceived
    );


    fclose(file);
}


/* =========================================
   PART 2-A SIMULATION OF PART 1-C
   ========================================= */

void simulate1C()
{
    int targets[MAX_ESCORTS];

    int count;

    int i;

    int destroyedCount = 0;

    double timeToHit;

    double currentTime = 0;

    FILE *file;


    file = fopen(
        "Part2A_Simulation_1C.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error creating file.\n"
        );

        return;
    }


    printf(
        "\n========================================\n"
    );

    printf(
        "       PART 2-A - SIMULATION 1-C\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 2-A - SIMULATION 1-C\n"
    );


    fprintf(
        file,
        "=========================\n\n"
    );


    fprintf(
        file,
        "B firing interval TB = %.2lf seconds\n\n",
        TB
    );


    /*
       E ships attack B.

       Part 1-C rule:
       each E ship can attack only once
       and causes damage according to its
       impact power.
    */

    for (i = 0; i < N; i++)
    {
        if (E[i].destroyed)
        {
            continue;
        }


        if (E[i].hasFired)
        {
            continue;
        }


        if (canHit(
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
               Add cumulative impact.
            */

            B.damageReceived +=
                E[i].impactPower * 100;


            E[i].hasFired = 1;


            printf(
                "E%d (%s) hit B.\n",

                E[i].id,

                E[i].type
            );


            printf(
                "Damage = %.2lf%%\n",

                E[i].impactPower * 100
            );


            printf(
                "Cumulative damage = %.2lf%%\n",

                B.damageReceived
            );


            fprintf(
                file,
                "E%d (%s) hit B\n",

                E[i].id,

                E[i].type
            );


            fprintf(
                file,
                "Damage = %.2lf%%\n",

                E[i].impactPower * 100
            );


            fprintf(
                file,
                "Cumulative damage = %.2lf%%\n\n",

                B.damageReceived
            );


            /*
               B destroyed at 100%.
            */

            if (B.damageReceived >= 100)
            {
                B.destroyed = 1;


                printf(
                    "\nBATTLESHIP DESTROYED!\n"
                );


                fprintf(
                    file,
                    "BATTLESHIP DESTROYED.\n"
                );


                fclose(file);

                return;
            }
        }
        else
        {
            /*
               E has used its one attack.
            */

            E[i].hasFired = 1;
        }
    }


    /*
       B survived.

       Now find E ships within B's
       attack range.
    */

    count =
        findAttackTargets(
            targets
        );


    /*
       Apply custom strategy.
    */

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


    saveAttackOrder(
        file,
        targets,
        count
    );


    /*
       B attacks in the selected order.
    */

    for (i = 0; i < count; i++)
    {
        int index;

        index =
            targets[i];


        if (E[index].destroyed)
        {
            continue;
        }


        /*
           B fires one shot every TB seconds.
        */

        if (i > 0)
        {
            currentTime += TB;
        }


        E[index].destroyed = 1;

        destroyedCount++;


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
    }


    /*
       Final results.
    */

    printf(
        "\n========================================\n"
    );

    printf(
        "SIMULATION 1-C COMPLETED\n"
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


    printf(
        "Battle time = %.2lf seconds\n",

        currentTime
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


    fprintf(
        file,
        "Battle time = %.2lf seconds\n",

        currentTime
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

    unsigned int seed;


    /*
       Random seed.
    */

    srand(
        (unsigned int)time(NULL)
    );


    printf(
        "============================================\n"
    );

    printf(
        "       ADVANCED NAVAL BATTLE SIMULATOR\n"
    );

    printf(
        "                    PART 2-A\n"
    );

    printf(
        "============================================\n"
    );


    /*
       Canvas size.
    */

    printf(
        "\nEnter canvas size D: "
    );

    scanf(
        "%lf",
        &D
    );


    if (D <= 0)
    {
        printf(
            "Invalid canvas size.\n"
        );

        return 0;
    }


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
       Battleship maximum velocity.
    */

    printf(
        "\nEnter B maximum shell velocity: "
    );

    scanf(
        "%lf",
        &B.vMax
    );


    /*
       Battleship starting position.
    */

    printf(
        "\nEnter B starting X coordinate: "
    );

    scanf(
        "%lf",
        &B.x
    );


    printf(
        "Enter B starting Y coordinate: "
    );

    scanf(
        "%lf",
        &B.y
    );


    /*
       B can normally fire from 0 to 90.
    */

    B.angleMin = 0;

    B.angleMax = 90;


    B.destroyed = 0;

    B.damageReceived = 0;


    /*
       Number of E ships.
    */

    printf(
        "\nEnter number of Escort Ships N: "
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
       Display initial conditions.
    */

    displayEscortShips();


    /*
       Generate path.
    */

    generatePath();


    /*
       =====================================
       B FIRING TIME
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
        "\n1. Enter TB manually\n"
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
            "\nEnter TB (seconds): "
        );

        scanf(
            "%lf",
            &TB
        );
    }
    else
    {
        /*
           Random firing interval.
        */

        TB =
            randomDouble(2, 10);

        printf(
            "\nRandomly generated TB = %.2lf seconds\n",
            TB
        );
    }


    if (TB <= 0)
    {
        TB = 5;

        printf(
            "Invalid TB. Default TB = 5 seconds.\n"
        );
    }


    /*
       Seed option.
    */

    printf(
        "\nEnter a seed value for reproducibility: "
    );

    scanf(
        "%u",
        &seed
    );


    srand(seed);


    /*
       =====================================
       MENU
       =====================================
    */

    do
    {
        printf(
            "\n========================================\n"
        );

        printf(
            "             PART 2-A MENU\n"
        );

        printf(
            "========================================\n"
        );


        printf(
            "1. Run Part 2-A Simulation of Part 1-A\n"
        );

        printf(
            "2. Run Part 2-A Simulation of Part 1-B\n"
        );

        printf(
            "3. Run Part 2-A Simulation of Part 1-C\n"
        );

        printf(
            "4. Display Escort Ships\n"
        );

        printf(
            "5. Display TB\n"
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

                /*
                   Reset before simulation.
                */

                resetSimulation();


                simulate1A();

                break;


            case 2:

                /*
                   Reset before simulation.
                */

                resetSimulation();


                /*
                   Same path generated earlier
                   is reused.
                */

                simulate1B();

                break;


            case 3:

                /*
                   Reset before simulation.
                */

                resetSimulation();


                simulate1C();

                break;


            case 4:

                displayEscortShips();

                break;


            case 5:

                printf(
                    "\nB firing interval TB = %.2lf seconds\n",
                    TB
                );

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
