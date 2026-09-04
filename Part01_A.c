#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

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
}

EscortShip;

typedef struct
{
	char type;

	double x;
	double y;

	double vMax;
}
Battleship;

int main()
{
	Battleship B;
	EscortShip E[100];

	double D;
	int N;

	printf("Enter canvas size D: ");
	scanf("%lf",&D);

	printf("Enter number of escort ships: ");
	scanf("%d",&N);

	printf("\nCanvas: (0,0) to (%.2f,%.2f)\n",D,D);
	printf("Number of escort ships: %d\n",N);

	printf("\nBattleship Types:\n");
	printf("U - USS Iowa\n");
	printf("M - MS King George V\n");
        printf("R - Richelieu\n");
	printf("S - Sovetsky Soyuz-class\n");

	printf("Enter Battleship type: ");
        scanf(" %c", &B.type);


	printf("Enter Battleship X position: ");
	scanf("%lf",&B.x);

	printf("Enter Battleship Y position: ");
        scanf("%lf",&B.y);

	printf("\nBattleship position: (%.2f,%.2f)\n",B.x,B.y);



	srand(time(NULL));

	for (int i = 0;i<N;i++)
	{
		E[i].x = ((double)rand()/RAND_MAX)*D;
		E[i].y = ((double)rand()/RAND_MAX)*D;
	}

	for(int i=0;i<N;i++)
	{
		printf("Escort %d: (%.2f,%.2f)\n",i+1,E[i].x,E[i].y);
	
	}


	for (int i=0;i<N;i++)
	{
		int type = rand() %5;

		if (type==0)
		{
			E[i].type[0] = 'E';
			E[i].type[1] = 'A';
			E[i].type[2] = '\0';
		}

		else if (type==1)
		{
			E[i].type[0] = 'E';
			E[i].type[1] = 'B';
			E[i].type[2] = '\0';
		}

		else if (type==2)
		{
			E[i].type[0] = 'E';
			E[i].type[1] = 'C';
			E[i].type[2] = '\0';
		}

		else if (type==3)
		{
			E[i].type[0] = 'E';
			E[i].type[1] = 'D';
			E[i].type[2] = '\0';
		}

		else 
		{
			E[i].type[0] = 'E';
			E[i].type[1] = 'E';
			E[i].type[2] = '\0';
		}
        }

		for (int i=0;i<N;i++)
		{
			printf("Escort %d: Type = %s\n",i+1,E[i].type);
		}

/*Assign values*/
        for (int i=0;i<N;i++)
	{
		if (E[i].type[1]=='A')
		{
			E[i].impactPower = 0.08;
			E[i].angleMax = 20;
		}

		else if (E[i].type[1]=='B')
		{
			E[i].impactPower = 0.06;
			E[i].angleMax = 30;
		}

		else if (E[i].type[1]=='C')
		{
			E[i].impactPower = 0.07;
			E[i].angleMax = 25;
		}

		else if (E[i].type[1]=='D')
		{
			E[i].impactPower = 0.05;
			E[i].angleMax = 50;
		}

		else if (E[i].type[1]=='E')
		{
			E[i].impactPower = 0.04;
			E[i].angleMax = 70;
		}
	}


	/* Velocity and angle properties */

         printf("\nEnter Battleship maximum shell velocity: ");
         scanf("%lf", &B.vMax);

         for (int i = 0; i < N; i++)
         {
		 E[i].vMin = 10 + rand() % 11;
                 E[i].vMax = 50 + rand() % 31;

                 E[i].angleMin = 0;
	 }



            
        /*Display properties*/
	for (int i=0;i<N;i++)
	{
		printf("\nEscort %d\n",i+1);
		printf("Type: %s\n",E[i].type);
		printf("Impact Power: %.2f\n",E[i].impactPower);
		printf("Angle Range: %.2f degrees\n",E[i].angleMax);
	}

/*save to file*/

	FILE *file;

	file = fopen("initial_conditions.txt","w");

	if(file==NULL)
	{
		printf("Error opening file.\n");
		return 1;
	}

	fprintf(file, "NAVAL BATTLE SIMULATOR\n");
	fprintf(file, "======================\n\n");

	fprintf(file, "Canvas Size: %.2f\n",D);
	fprintf(file, "Number of Escort Ships: %d\n\n",N);

	fprintf(file, "BATTLESHIP\n");
	fprintf(file, "Type: %c\n",B.type);
	fprintf(file, "Position: (%.2f,%.2f)\n\n",B.x,B.y);
	fprintf(file, "Maximum Velocity: %.2f\n\n", B.vMax);

	fprintf(file, "ESCORT SHIPS\n");

	for (int i=0;i<N;i++)
	{
		fprintf(file, "\nEscort %d\n",i+1);
		fprintf(file, "Type: %s\n",E[i].type);
		fprintf(file, "Position: (%.2f,%.2f)\n",E[i].x,E[i].y);
		fprintf(file, "Impact Power: %.2f\n",E[i].impactPower);
		fprintf(file, "Minimum Velocity: %.2f\n",E[i].vMin);
                fprintf(file, "Maximum Velocity: %.2f\n",E[i].vMax);
                fprintf(file, "Minimum Angle: %.2f degrees\n",E[i].angleMin);
                fprintf(file, "Maximum Angle: %.2f degrees\n",E[i].angleMax);
		
	}

	fclose(file);

	printf("\nInitial conditions saved to initial_conditions.txt\n");


/*display data from saved file*/

	FILE *readFile;
	char line[200];

	readFile = fopen("initial_conditions.txt","r");

	if (readFile==NULL)
	{
		printf("Error opening initial_conditions.txt\n");
		return 1;
	}

	printf("\n=========================================\n");
	printf("INITIAL CONDITIONS FROM FILE\n");
	printf("===========================================\n\n");

	while (fgets(line,sizeof(line),readFile) !=NULL)
	{
		printf("%s",line);
	}

	fclose(readFile);

	/*basic simulation*/


	double distance;
	double attackRange = D/2.0;

	int battleshipSunk = 0;
	int hitCount = 0;
	int sinkingShip = -1;

	printf("BASIC ATTACK SIMULATION\n");
	printf("-------------------------\n");

	for (int i = 0;i<N;i++)
	{

		distance = sqrt((E[i].x-B.x)*(E[i].x-B.x)+(E[i].y-B.y)*(E[i].y-B.y));
		printf("\nDistance to Escort %d = %.2f\n",i+1,distance);

		if (distance<=attackRange)
		{
			printf("Escort %d is in attack range.\n",i+1);

			/*E attacks B*/

			battleshipSunk = 1;
			sinkingShip = i+1;

			break;

		}
		else
		{
			printf("Escort %d is out of attack range.\n",i+1);
		}
	}

	if (battleshipSunk)
	{
		
		printf("Escort ship %d sank the Battleship.\n",sinkingShip);
	}

	else
	{
		printf("\nBattleship is not sunk.\n");
	}


	    if (battleshipSunk == 1)
    {
        printf("\nBattleship is sunk!\n");
        printf("Escort ship %d sank the Battleship.\n",sinkingShip);
    }
    else
    {
        printf("\nBattleship is not sunk.\n");

        for (int i = 0;i<N;i++)
        {
            distance = sqrt((E[i].x-B.x)*(E[i].x-B.x)+(E[i].y-B.y)*(E[i].y-B.y));

            if (distance<=attackRange)
            {
                hitCount++;

                printf("Battleship hit Escort %d\n", i+1);
            }
        }

        printf("\nTotal E ships hit by B: %d\n", hitCount);
    }

    /*save battle result*/
    
   if (battleshipSunk == 0)
   {
	  double battleTime = 0.0;

	 printf("\nBattle completed in %.2f seconds.\n",battleTime);
	 
	FILE*resultFile;
       resultFile = fopen("battle_results.txt","w");

       if (resultFile == NULL)
       {
	       printf("Error creating battle results file.\n");
	       return 1;
       }

       fprintf(resultFile,"BATTLE RESULTS\n");
       fprintf(resultFile,"---------------\n\n");

       fprintf(resultFile,"Battleship Type: %c\n",B.type);
       fprintf(resultFile,"Battleship Position: (%.2f,%.2f)\n\n",B.x,B.y);
       
       fprintf(resultFile,"Number of E ships hit by B: %d\n",hitCount);
       fprintf(resultFile,"Battle Time: %.2f seconds\n\n",battleTime);
       fprintf(resultFile,"HIT ESCORT SHIPS\n");

       for(int i=0;i<N;i++)
       {
	       distance = sqrt((E[i].x-B.x)*(E[i].x-B.x)+(E[i].y-B.y)*(E[i].y-B.y));

	       if (distance<=attackRange)
	       {
		       fprintf(resultFile,"Escort %d - Time to hit: %2f seconds\n",i+1,battleTime);
	       }
       
       fclose(resultFile);

       printf("Battle results saved to battle_results.txt\n");
       }
   }


   /*save final battlefield*/

   FILE *finalFile;

   finalFile = fopen("final_battlefield.txt","w");

   if (finalFile == NULL)
   {
	   printf("Error creating final battlefield file.\n");
	   return 1;
   }

   fprintf(finalFile,"FINAL BATTLEFIELD\n");
   fprintf(finalFile,"------------------\n\n");

   fprintf(finalFile,"Canvas Size: %.2f\n",D);

   fprintf(finalFile,"Battleship\n");
   fprintf(finalFile,"Type: %c\n",B.type);
   fprintf(finalFile,"Position:(%.2f,%.2f)\n\n",B.x,B.y);

   fprintf(finalFile,"Escort Ships\n");

    for(int i=0;i<N;i++)
    {
	fprintf(finalFile,"\nEscort %d\n",i+1);
        fprintf(finalFile,"Type: %s\n",E[i].type);
        fprintf(finalFile,"Position:(%.2f,%.2f)\n",E[i].x,E[i].y);
	fprintf(finalFile,"Status: %s\n",battleshipSunk && sinkingShip == i+1 ? "Sank Battleship": "Alive");
    }

    fclose(finalFile);

    printf("Final battlefield saved to final_battlefield.txt\n");

}


