/** @file DecodingInput.cpp
 *  \brief A file to decoding the input informations such as, probability, pre-filled cache...
 *    */
#include "DecodingInput.h"

int ***Ind_cache = NULL;
int *Q_demand = NULL;
int **Q_matrix = NULL;

/**This is a function that provide a compute the Ind cache matrix, this matrix is a binary matrix, Ind[i,j,k] = 0 if the user 'i' not have the chunk 'k' associated to file 'j'*/
void computeIndMatrix(vector<int> input){
    int index = input.size() - 1;
    int id_user = 0;

    while (index >=0){
        int n_o_r = input.at(index);
        index--;

        for (int i = 0; i < n_o_r; i++){
            Q_demand[id_user] = input.at(index);
            index--;
        }

        int m_user = input.at(index);
        index--;

        for (int i = 0; i < m_user; i++){
            int id_file = input.at(index);
            index --;
            int id_chunk = input.at(index);
            index --;
            Ind_cache[id_user][id_file][id_chunk] = 1;
        }

        id_user++;
    }
}

/**This is a function that provide a compute the Q matrix, this matrix is a binary matrix, Q[i,k] = 0 if the user 'i' have in cache the chunk 'k' related to files that he request*/
void computeQMatrix(int n_utenti, int* b_chuncks){
    int i, k, id_file;

    /*For each user ...*/
    for (i=0; i<n_utenti; i++){
    	/*The file that the user 'i' request*/
    	id_file = Q_demand[i];
    	/*For each chunk ...*/
    	for (k=0; k<b_chuncks[id_file]; k++){
    		/*Check if user 'i' have in cache the chunk 'k' related to files 'id_file'*/
    		if (Ind_cache[i][id_file][k] == 1){
    			Q_matrix[i][k] = 0;
    		}else{
    			Q_matrix[i][k] = 1;
    		}
    	}
    }
}

void _allocArrays(int n_utenti, int *b_chuncks, int m_files){
    int i, j ,k;
    Ind_cache = (int ***) malloc (n_utenti * sizeof(int **));
    check_memory_allocation_3D(Ind_cache, "Allocation 3D Ind Matrix.");

    for (int i = 0; i < n_utenti; i++){
        Ind_cache[i] = (int **) malloc (m_files * sizeof(int *));
        check_memory_allocation_2D(Ind_cache[i], "Allocation 2D Ind Matrix.");
        for (int j = 0; j < m_files; j++){
            Ind_cache[i][j] = (int *) malloc (b_chuncks[j] * sizeof(int));
            check_memory_allocation_1D(Ind_cache[i][j], "Allocation 1D Ind Matrix.");
        }
    }

    /*Initialization of binary matrix, Ind[i,j,k] = 0 if the user 'i' not have the chunk 'k' associated to file 'j'*/
    for (i=0; i<n_utenti; i++){
        for (j=0; j<m_files; j++){
            for (k=0; k<b_chuncks[j]; k++){
                Ind_cache[i][j][k] = 0;
            }
        }
    }

	//BUG FIX: searching the maximum b_chuncks...
	int b_chuncks_max = 0;
	for (int u = 0; u < m_files; u++){
		if (b_chuncks[u] > b_chuncks_max)
			b_chuncks_max = b_chuncks[u];
		}

    Q_matrix = (int **) malloc (n_utenti * sizeof(int *));
    check_memory_allocation_2D(Q_matrix, "Allocation 2D Q Matrix.");
    for (int i = 0; i < n_utenti; i++){
		Q_matrix[i] = (int *) malloc (b_chuncks_max * sizeof(int));
//        Q_matrix[i] = (int *) malloc (b_chuncks[4] * sizeof(int));	//BUG ENORME
        check_memory_allocation_1D(Q_matrix[i], "Allocation 1D Q Matrix.");
    }


    Q_demand = (int *) malloc (n_utenti * sizeof(int));
    check_memory_allocation_1D(Q_demand, "Allocation 1D Q Demand.");
}

data_matrix fromArrayToMatrix(int m_files, int* b_chuncks, vector<int> input){
    data_matrix data;

    int n_utenti = 0;
    int index = input.size() - 1;

    while (index >=0){
        n_utenti++;
        int n_o_r = input.at(index);
        index -= (n_o_r + 1);
        int m_user = input.at(index);
        index -= ((2 * m_user) + 1);
    }

    printf("Il numero di utenti e' pari a: %d\n\n", n_utenti);

    _allocArrays(n_utenti, b_chuncks, m_files);

    /**Compute the cache binary matrix*/
    computeIndMatrix(input);

    /**Compute the binary matrix Q*/
    computeQMatrix(n_utenti, b_chuncks);
    data.n_utenti = n_utenti;
    data.b_chunks = b_chuncks;
    data.m_files = m_files;
    data.Q = Q_demand;
    data.Q_chuncks = Q_matrix;
    data.Ind = Ind_cache;


    return data;
}
