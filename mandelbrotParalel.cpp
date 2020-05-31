#include <complex>
#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <stdlib.h>

#define WORK_TAG 0
#define DIE_TAG 1
#define GRAO 48

using namespace std;

int main(int argc, char *argv[]){
	int max_row, max_column, max_n, aux, k, begin = 0;
	int envia = 0, recebe = 0, TAM;
	char *mat, *mat_aux;
	int meurank, procs, tag = 10;
	MPI_Status status;

	if (argc != 4){
		printf("Numero de argumentos invalido! Valido: 3 (max_row, max_column e max_n). \n");
		return EXIT_SUCCESS;
	}

	max_row = atoi(argv[1]);
	max_column = atoi(argv[2]);
	max_n = atoi(argv[3]);

	//max_row = 1024; 
	//max_column = 768; 
	//max_n = 18000;	

	TAM = (max_row * max_column);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meurank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);


    if (procs != 2 && procs != 4 && procs != 8 && procs != 16 && procs != 32 && procs != 64) {
        if (meurank == 0)
     		printf("Numero de processos invalido! Validos: 2, 4, 8, 16, 32 e 64.\n");
   		MPI_Finalize();
    	return 0;
   	}

   	//Mestre
    if (meurank == 0){     	
		mat = (char*)malloc(sizeof(char*)*TAM);
		mat_aux = (char*)malloc(sizeof(char*)*GRAO);

		//manda mensagens (distribui para os processos escravos)
		for(k=1; k < procs; k++){
            MPI_Send(&begin, 1 , MPI_INT, k, WORK_TAG, MPI_COMM_WORLD);
            envia++;
            begin += GRAO; // pega o inicio mais um pedaço de 48
	    }    

	    // pega as respostas dos escravos
        while(recebe < envia){
            MPI_Recv(&mat_aux[0], GRAO, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            memcpy(&mat[status.MPI_TAG], &mat_aux[0], GRAO);
            recebe++;
            if(begin < TAM){
                MPI_Send(&begin, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
                envia++;
                begin += GRAO;
             }
        }

        //sair
        for(k=1; k < procs; k++){
            MPI_Send(&k, 1, MPI_INT, k, DIE_TAG, MPI_COMM_WORLD);
        }

        aux = 0;

		for(int r = 0; r < max_row; ++r){
			for(int c = 0; c < max_column; ++c) {
				cout << mat[aux++];
			}
			cout << '\n';
		}

    } 
    //Escravos
    else{
    	mat = (char *)malloc(sizeof(char*)*GRAO);
    	status.MPI_TAG = WORK_TAG;	

    	while(status.MPI_TAG != DIE_TAG){ // recebe a tarefa
            MPI_Recv(&k, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if(status.MPI_TAG == WORK_TAG){
			    aux = 0;

			    for(int i = k; i < (GRAO + k); ++i){ // realiza a tarefa
					int r = i / max_column;
					int c = i % max_column;
					complex<float> z;
					int n = 0;

					while(abs(z) < 2 && ++n < max_n)
						z = pow(z, 2) + decltype(z)(
							(float)c * 2 / max_column - 1.5,
							(float)r * 2 / max_row - 1
						);
					mat[aux++] = (n == max_n ? '#' : '.');
				}

				MPI_Send(&mat[0], GRAO, MPI_CHAR, 0, k, MPI_COMM_WORLD); // envia a tarefa 
            }
        }
    }

	MPI_Finalize();
}