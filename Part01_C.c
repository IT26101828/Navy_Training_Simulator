#include <stdio.h>
#include <stdlib.h>
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


/* =========================================
   RANDOM NUMBER
   ========================================= */

double randomDouble(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


int randomInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}


/* =========================================
   SET E SHIP TYPE PROPERTIES
   ========================================= */

void setEscortProperties(EscortShip *ship)
{
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
           Generate position
        */

        E[i].x = randomDouble(0, D);
        E[i].y = randomDouble(0, D);


        /*
           Generate minimum velocity
        */

        E[i].vMin = randomDouble(10, 50);


        /*
           Generate maximum velocity.
           
           EA can have maximum velocity
           up to 1.2 * B maximum velocity.

           Other E ships must have maximum
           velocity less than B maximum velocity.
        */

        if (E[i].type[1] == 'A')
        {
            E[i].vMax =
                randomDouble(
                    E[i].vMin + 10,
                    1.2 * B.vMax
                );
        }
        else
        {
            double maximum;

            maximum = B.vMax - 1;

            if (maximum <= E[i].vMin)
            {
                E[i].vMin = maximum / 2.0;
            }

            E[i].vMax =
                randomDouble(
                    E[i].vMin + 1,
                    maximum
                );
        }


        /*
           Initial values
        */

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

        printf("Angle         : %.2lf - %.2lf\n",
               E[i].angleMin,
               E[i].angleMax);

        printf("Impact Power  : %.2lf\n",
               E[i].impactPower);

        printf("Damage        : %.2lf%%\n",
               E[i].damage);
    }
}


/* =========================================
   CHECK WHETHER A SHIP CAN HIT A TARGET
   ========================================= */

/*
   Projectile equation used:

   R = (u^2 * sin(2theta)) / g

   For targets at different heights, we test
   different firing angles using:

   y = x tan(theta)
       - [g x^2 / (2 u^2 cos^2(theta))]

   If a valid velocity exists between the
   minimum and maximum velocity, the target
   can be hit.
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
       Try every angle within the
       allowed angle range.
    */

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


            /*
               Check whether the required
               velocity is possible.
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
   SAVE INITIAL CONDITIONS
   ========================================= */

void saveInitialConditions()
{
    FILE *file;

    int i;

    file = fopen(
        "part1C_initial_conditions.txt",
        "w"
    );

    if (file == NULL)
    {
        printf("Error creating file.\n");
        return;
    }


    fprintf(
        file,
        "PART 1-C INITIAL CONDITIONS\n"
    );

    fprintf(
        file,
        "===========================\n\n"
    );


    fprintf(
        file,
        "Canvas Size: %.2lf x %.2lf\n",
        D,
        D
    );


    fprintf(
        file,
        "\nBATTLESHIP\n"
    );

    fprintf(
        file,
        "Type: %c\n",
        B.type
    );

    fprintf(
        file,
        "Position: (%.2lf, %.2lf)\n",
        B.x,
        B.y
    );

    fprintf(
        file,
        "Vmax: %.2lf\n",
        B.vMax
    );


    fprintf(
        file,
        "\nESCORT SHIPS\n"
    );


    for (i = 0; i < N; i++)
    {
        fprintf(
            file,
            "\nE%d\n",
            E[i].id
        );

        fprintf(
            file,
            "Type: %s\n",
            E[i].type
        );

        fprintf(
            file,
            "Position: (%.2lf, %.2lf)\n",
            E[i].x,
            E[i].y
        );

        fprintf(
            file,
            "Vmin: %.2lf\n",
            E[i].vMin
        );

        fprintf(
            file,
            "Vmax: %.2lf\n",
            E[i].vMax
        );

        fprintf(
            file,
            "Minimum Angle: %.2lf\n",
            E[i].angleMin
        );

        fprintf(
            file,
            "Maximum Angle: %.2lf\n",
            E[i].angleMax
        );

        fprintf(
            file,
            "Impact Power: %.2lf\n",
            E[i].impactPower
        );
    }


    fclose(file);

    printf(
        "\nInitial conditions saved.\n"
    );
}


/* =========================================
   SAVE FINAL CONDITIONS
   ========================================= */

void saveFinalConditions()
{
    FILE *file;

    int i;

    file = fopen(
        "part1C_final_conditions.txt",
        "w"
    );

    if (file == NULL)
    {
        printf("Error creating final file.\n");
        return;
    }


    fprintf(
        file,
        "PART 1-C FINAL CONDITIONS\n"
    );

    fprintf(
        file,
        "=========================\n\n"
    );


    fprintf(
        file,
        "Battleship Type: %c\n",
        B.type
    );

    fprintf(
        file,
        "Battleship Position: (%.2lf, %.2lf)\n",
        B.x,
        B.y
    );

    fprintf(
        file,
        "Cumulative Damage: %.2lf%%\n",
        B.damageReceived
    );

    fprintf(
        file,
        "Status: %s\n\n",
        B.destroyed ?
        "DESTROYED" :
        "SURVIVED"
    );


    fprintf(
        file,
        "ESCORT SHIPS\n"
    );


    for (i = 0; i < N; i++)
    {
        fprintf(
            file,
            "E%d | Type: %s | Damage: %.2lf%% | %s\n",
            E[i].id,
            E[i].type,
            E[i].damage,
            E[i].destroyed ?
            "DESTROYED" :
            "ACTIVE"
        );
    }


    fclose(file);

    printf(
        "Final conditions saved.\n"
    );
}


/* =========================================
   RESET SHIPS
   ========================================= */

void resetShips()
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
   PART 1-C SIMULATION
   BASED ON PART 1-A
   ========================================= */

void simulation1C()
{
    int i;

    int hitCount;

    double timeToHit;

    FILE *file;


    file = fopen(
        "part1C_simulation.txt",
        "w"
    );


    if (file == NULL)
    {
        printf("Error creating simulation file.\n");
        return;
    }


    printf("\n========================================\n");
    printf("       PART 1-C SIMULATION\n");
    printf("========================================\n");


    fprintf(
        file,
        "PART 1-C SIMULATION\n"
    );

    fprintf(
        file,
        "==================\n\n"
    );


    /*
       E ships attack B.

       IMPORTANT:
       Each E ship can attack only ONCE.
    */

    for (i = 0; i < N; i++)
    {
        if (E[i].destroyed)
        {
            continue;
        }


        /*
           Check whether E can hit B.
        */

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
               Apply impact power.

               Example:

               EA = 0.08

               Damage = 8%
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
                "Damage caused = %.2lf%%\n",
                E[i].impactPower * 100
            );


            printf(
                "Cumulative B damage = %.2lf%%\n",
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
                "Time to hit: %.2lf seconds\n",
                timeToHit
            );


            fprintf(
                file,
                "Impact Power: %.2lf\n",
                E[i].impactPower
            );


            fprintf(
                file,
                "Damage caused: %.2lf%%\n",
                E[i].impactPower * 100
            );


            fprintf(
                file,
                "Cumulative B damage: %.2lf%%\n\n",
                B.damageReceived
            );


            /*
               If B reaches 100%, it is destroyed.
            */

            if (B.damageReceived >= 100)
            {
                B.destroyed = 1;

                printf(
                    "\nBATTLESHIP DESTROYED!\n"
                );


                fprintf(
                    file,
                    "BATTLESHIP DESTROYED\n"
                );


                break;
            }
        }
        else
        {
            /*
               E cannot hit B.
            */

            E[i].hasFired = 1;


            printf(
                "E%d (%s) cannot hit B.\n",
                E[i].id,
                E[i].type
            );


            fprintf(
                file,
                "E%d (%s) cannot hit B.\n\n",
                E[i].id,
                E[i].type
            );
        }
    }


    /*
       If B survives, B attacks E ships.
    */

    if (!B.destroyed)
    {
        hitCount = 0;


        printf(
            "\nBATTLESHIP SURVIVED.\n"
        );


        printf(
            "Cumulative damage on B = %.2lf%%\n",
            B.damageReceived
        );


        fprintf(
            file,
            "\nBATTLESHIP SURVIVED.\n"
        );


        fprintf(
            file,
            "Cumulative damage on B = %.2lf%%\n\n",
            B.damageReceived
        );


        for (i = 0; i < N; i++)
        {
            if (E[i].destroyed)
            {
                continue;
            }


            /*
               B can destroy E with one hit.
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
                E[i].destroyed = 1;

                hitCount++;


                printf(
                    "B destroyed E%d.\n",
                    E[i].id
                );


                printf(
                    "Time to hit = %.2lf seconds\n",
                    timeToHit
                );


                fprintf(
                    file,
                    "B destroyed E%d\n",
                    E[i].id
                );


                fprintf(
                    file,
                    "Time to hit: %.2lf seconds\n\n",
                    timeToHit
                );
            }
        }


        printf(
            "\nTotal E ships destroyed by B: %d\n",
            hitCount
        );


        fprintf(
            file,
            "\nTotal E ships destroyed by B: %d\n",
            hitCount
        );
    }


    fclose(file);

    saveFinalConditions();
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
        "          BATTLESHIP PATH\n"
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
   PART 1-C VERSION OF PART 1-B
   SIMULATION 1
   ========================================= */

void pathSimulation1C()
{
    int i;
    int j;

    int hitCount;

    double timeToHit;

    FILE *file;


    file = fopen(
        "part1C_path_simulation1.txt",
        "w"
    );


    if (file == NULL)
    {
        printf("Error creating file.\n");
        return;
    }


    printf(
        "\n========================================\n"
    );

    printf(
        "PART 1-C - PATH SIMULATION 1\n"
    );

    printf(
        "========================================\n"
    );


    fprintf(
        file,
        "PART 1-C - PATH SIMULATION 1\n"
    );


    /*
       Move B through all k points.
    */

    for (i = 0; i < k; i++)
    {
        B.x = pathX[i];

        B.y = pathY[i];


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
           E ships that have already fired
           cannot fire again.
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


            /*
               Check whether E can hit B.
            */

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
                /*
                   Apply cumulative damage.
                */

                B.damageReceived +=
                    E[j].impactPower * 100;


                E[j].hasFired = 1;


                printf(
                    "E%d (%s) hit B -> %.2lf%% damage\n",
                    E[j].id,
                    E[j].type,
                    E[j].impactPower * 100
                );


                printf(
                    "Cumulative damage = %.2lf%%\n",
                    B.damageReceived
                );


                fprintf(
                    file,
                    "E%d (%s) hit B\n",
                    E[j].id,
                    E[j].type
                );


                fprintf(
                    file,
                    "Time to hit: %.2lf seconds\n",
                    timeToHit
                );


                fprintf(
                    file,
                    "Damage: %.2lf%%\n",
                    E[j].impactPower * 100
                );


                /*
                   Check whether B is destroyed.
                */

                if (B.damageReceived >= 100)
                {
                    B.destroyed = 1;


                    printf(
                        "\nBATTLESHIP DESTROYED!\n"
                    );


                    fprintf(
                        file,
                        "BATTLESHIP DESTROYED\n"
                    );


                    fclose(file);


                    saveFinalConditions();


                    return;
                }
            }
            else
            {
                /*
                   E tried once and cannot fire again.
                */

                E[j].hasFired = 1;


                printf(
                    "E%d cannot hit B.\n",
                    E[j].id
                );
            }
        }


        /*
           B attacks E ships.
        */

        hitCount = 0;


        if (!B.destroyed)
        {
            for (j = 0; j < N; j++)
            {
                if (E[j].destroyed)
                {
                    continue;
                }


                if (canHit(
                    B.x,
                    B.y,

                    E[j].x,
                    E[j].y,

                    0,
                    B.vMax,

                    B.angleMin,
                    B.angleMax,

                    &timeToHit
                ))
                {
                    /*
                       B destroys E with one hit.
                    */

                    E[j].destroyed = 1;

                    hitCount++;


                    printf(
                        "B destroyed E%d.\n",
                        E[j].id
                    );


                    fprintf(
                        file,
                        "B destroyed E%d\n",
                        E[j].id
                    );


                    fprintf(
                        file,
                        "Time to hit: %.2lf seconds\n",
                        timeToHit
                    );
                }
            }
        }


        fprintf(
            file,
            "E ships destroyed this iteration: %d\n",
            hitCount
        );
    }


    /*
       All k iterations completed.
    */

    printf(
        "\nAll %d iterations completed.\n",
        k
    );


    printf(
        "B cumulative damage = %.2lf%%\n",
        B.damageReceived
    );


    fprintf(
        file,
        "\nAll %d iterations completed.\n",
        k
    );


    fprintf(
        file,
        "B cumulative damage = %.2lf%%\n",
        B.damageReceived
    );


    fclose(file);

    saveFinalConditions();
}


/* =========================================
   RESET FOR PATH SIMULATION
   ========================================= */

void resetForPathSimulation()
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
   MAIN
   ========================================= */

int main()
{
    int choice;

    int i;


    srand(
        (unsigned int)time(NULL)
    );


    printf(
        "============================================\n"
    );

    printf(
        "          ADVANCED NAVAL BATTLE\n"
    );

    printf(
        "                 PART 1-C\n"
    );

    printf(
        "============================================\n"
    );


    /*
       Canvas
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
       Battleship
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
       Battleship maximum shell velocity
    */

    printf(
        "\nEnter B maximum shell velocity: "
    );

    scanf(
        "%lf",
        &B.vMax
    );


    /*
       Battleship starting position
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
       Battleship angle range
    */

    B.angleMin = 0;

    B.angleMax = 90;


    /*
       Initial status
    */

    B.destroyed = 0;

    B.damageReceived = 0;


    /*
       Number of Escort Ships
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
       Number of path points
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
       Save initial conditions.
    */

    saveInitialConditions();


    /*
       Display initial ships.
    */

    displayEscortShips();


    /*
       Generate path once.

       The same path can then be reused
       for the Part 1-B simulation.
    */

    generatePath();


    /*
       Main menu
    */

    do
    {
        printf(
            "\n========================================\n"
        );

        printf(
            "             PART 1-C MENU\n"
        );

        printf(
            "========================================\n"
        );

        printf(
            "1. Run Part 1-C basic simulation\n"
        );

        printf(
            "2. Run Part 1-C path simulation\n"
        );

        printf(
            "3. Display Escort Ships\n"
        );

        printf(
            "4. Exit\n"
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

                resetShips();


                /*
                   Run Part 1-A again,
                   but with cumulative damage.
                */

                simulation1C();

                break;


            case 2:

                /*
                   Reset before path simulation.
                */

                resetForPathSimulation();


                /*
                   Run Part 1-B again,
                   but with cumulative damage.
                */

                pathSimulation1C();

                break;


            case 3:

                displayEscortShips();

                break;


            case 4:

                printf(
                    "\nProgram ended.\n"
                );

                break;


            default:

                printf(
                    "\nInvalid choice.\n"
                );
        }


    } while (choice != 4);


    return 0;
}
