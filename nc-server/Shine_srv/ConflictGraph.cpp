/** @file ConflictGraph.cpp
 *  \brief File to generate the confict graph to perform the algorithm
 *    */
#include "ConflictGraph.h"

/* Variables declaration */
int n_utenti;
int m_files;
int *b_chuncks;
int n_nodi;
int m_archi;

int ***Ind = NULL;
int *Q = NULL;
int **Q_chuncks = NULL;

int **Matrix_Adj = NULL;
nodo *nodes = NULL;

/******************************************************************************************************************************************/
void computeNumberOfNodes(){
    int i, k, id_file;
    n_nodi = 0;

    /*For each user ...*/
    for (i=0; i<n_utenti; i++){
    	/*The file that the user 'i' request*/
    	id_file = Q[i];
    	/*For each chunk ...*/
    	for (k=0; k<b_chuncks[id_file]; k++){
    		/*Checvk if user 'i' have in cache the chunk 'k' related to files 'id_file'*/
    		if (Ind[i][id_file][k] == 1){
    			Q_chuncks[i][k] = 0;
            }else{
                Q_chuncks[i][k] = 1;
                n_nodi++;
            }
        }
    }
}

/*This is a function that provide to make a nodes of the conflict information graph*/
void makeNodes(){
    int i, j, k, id;

    nodes = (nodo *) malloc (n_nodi * sizeof(nodo));
    if (!nodes){
        printf("\nError: Allocation Nodes.\n");
		exit(0);
    }

    Matrix_Adj = (int **) malloc (n_nodi * sizeof(int *));
    check_memory_allocation_2D(Matrix_Adj, "Allocation 2D Adj Matrix.");

    for (i=0; i<n_nodi; i++){
        Matrix_Adj[i] = (int *) malloc (n_nodi * sizeof(int));
        check_memory_allocation_1D(Matrix_Adj[i], "Allocation 1D Adj Matrix.");
    }

    for (i=0; i<n_nodi; i++){
        Matrix_Adj[i][i] = 0;
    }

    id =0;
    /*For each user and for each chunk related to requested file that the user not have in cache will be created a node*/
    for (i=0; i<n_utenti; i++){
        j = Q[i];
        for (k=0; k<b_chuncks[j]; k++){
            if (Q_chuncks[i][k] == 1){
                nodes[id].id = id;
                nodes[id].degree = 0;
                nodes[id].id_utente = i;
                nodes[id].id_chunck = k;
                nodes[id].id_file = j;
                id++;
            }
        }
    }

    if (id != n_nodi){
        printf("\nError: Create Nodes Number Dismatch With The Aspected Nodes Number.\n");
		exit(0);
    }
}

/*This is a function that provide to make a edges of the conflict information graph*/
void makeEdges(){
    int i_1, j_1, k_1, i_2, j_2, k_2, id1, id2;

    m_archi = 0;

    /*For each pair of nodes of the conflict information graph ...*/
    for (id1=0; id1<n_nodi-1; id1++){
        for (id2=(id1+1); id2<n_nodi; id2++){
            i_1 = nodes[id1].id_utente;
            j_1 = nodes[id1].id_file;
            k_1 = nodes[id1].id_chunck;

            i_2 = nodes[id2].id_utente;
            j_2 = nodes[id2].id_file;
            k_2 = nodes[id2].id_chunck;

		 //cout << "j_1: " << j_1 << ", k_1: " << k_1 << ", j_2: " << j_2 << ", j_2: " << k_2 << endl;

            /*If the files requests by user 'i_1' and user 'i_2' are different or, the chunk requests by user 'i_1' and user 'i_2' are different and
            the user 'i_2' not have in cache the chunk requests by user 'i_1' or the user 'i_1' not have in cache the chunk requests by user 'i_2' then will be created an edges between the two nodes
            of the conflict information graph*/
//            if ( ( ( k_1 != k_2 ) || ( j_1 != j_2) ) && ( ( Ind[i_2][j_1][k_1] == 0 ) || ( Ind[i_1][j_2][k_2] == 0 ) ) ){
			if ( ( ( Ind[i_2][j_1][k_1] == 0 ) || ( Ind[i_1][j_2][k_2] == 0 ) ) ){
		           Matrix_Adj[id1][id2] = 1;
		           Matrix_Adj[id2][id1] = 1;
		           nodes[id1].degree++;
		           nodes[id2].degree++;
		           m_archi += 2;
			}else{
                Matrix_Adj[id1][id2] = 0;
                Matrix_Adj[id2][id1] = 0;
            }
        }
    }
}

/******************************************************************************************************************************************/
void _dealloc(){
    int i;

    free(Q);
    Q = NULL;

    for (i=0; i<n_utenti; i++){
        free(Q_chuncks[i]);
        Q_chuncks[i] = NULL;
    }

    free(Q_chuncks);
    Q_chuncks = NULL;
}

/******************************************************************************************************************************************/

/*This is a main function*/
cf_data conflictGraphGenerator(data_matrix data){
    m_files = data.m_files;
    b_chuncks = data.b_chunks;
    n_utenti = data.n_utenti;
    Ind = data.Ind;
    Q = data.Q;
    Q_chuncks = data.Q_chuncks;

//ALLOCAZIONE DI MATRIX_ADJ QUI E NODES

    cf_data output;

    if (Ind != NULL && Q != NULL && Q_chuncks != NULL){
        /*Bulding a conflict information graph*/
        computeNumberOfNodes();

        if (n_nodi > 0){
            makeNodes();
            makeEdges();
        }else{
            m_archi = 0;
        }

        output.Matrix_Adj = Matrix_Adj;
        output.n_nodi = n_nodi;
        output.nodes = nodes;
        output.Ind = Ind;

        //_dealloc();
    }else{
        output.n_nodi = 0;
    }

    return output;
}
