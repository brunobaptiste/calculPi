#include <stdio.h>
#include <mpi.h>

#define MASTER 0

double f(double x){
  return 4.0/(1.0+x*x); 
}

int main(int argc, char *argv[])
{
    int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
	rc,             /* return code */
    i;

    MPI_Status status;
    MPI_Request req;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);

    int nbblocs = 2000000000; // 2000000000
    double R, P, S, Ssum;

    S = 0;
    P = 1.0/nbblocs;

    // /* Boucle for exec par tous (trop long)*/
    // for (i=0; i<=nbblocs-1 ;i++)
    // {

    //     if(i%numtasks == taskid){
    //         R = P*f(i*P);
    //         S += R;
    //         //printf("S%d = %f\n", i, S);
    //     }
    // }
    // printf("rank : %d mon pi = %.10f\n", taskid, S);

    /* Boucle par partie de nbblocs */
    for(i = taskid*(nbblocs/numtasks); i < (taskid+1)*(nbblocs/numtasks); i++){
        R = P*f(i*P);
        S += R;
    }
    printf("rank : %d mon S = %.10f\n", taskid, S);

    // /* Boucle for exec pour tous */
    // for (i=0; i<=nbblocs-1 ;i++)
    // {
    //     R = P*f(i*P);
    //     S += R;
    //     //printf("S%d = %f\n", i, S);        
    // }
    // printf("rank : %d mon pi = %.10f\n", taskid, S);


    rc = MPI_Ireduce(&S, &Ssum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD, &req);

    if(taskid==MASTER){
	int ready;
	MPI_Test(&req, &ready, &status);
	if(ready==1){
        	printf("%.26f < PI < %.26f\n", (Ssum-P*f(0)+P*f(1)), (Ssum));
        	printf("Milieu intervalle : PI = %.26f\n", (((Ssum-P*f(0)+P*f(1)) + (Ssum))/2));
        	printf("Vraie valeur de PI: PI = 3.14159265358979323846264338 \n");
		}
	else{printf("not ready");}
    }

    MPI_Finalize();
    return 0;
}


