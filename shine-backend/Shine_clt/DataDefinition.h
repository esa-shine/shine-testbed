#ifndef DATADEFINITION_H
#define DATADEFINITION_H

#include <limits>
#include <vector>

using namespace std;

const int INF = numeric_limits<int>::max();

typedef struct NODE_LIST_STRUCT{
    int id;
    int degree;
    int id_utente;
    int id_file;
    int id_chunck;
    struct NODE_LIST_STRUCT *next;
}NODE_LIST;

typedef struct LIST_STRUCT{
    NODE_LIST *nodes_list;
}LIST;

typedef struct NODE_SUBSET_STRUCT{
    int id;
    int cardinality;
	int *label;
	int size_label;
    struct NODE_SUBSET_STRUCT *next;
}NODE_SUBSET;

typedef struct SUBSET_STRUCT{
    NODE_SUBSET *pt_end;
    NODE_SUBSET *nodes_list;
}SUBSET;

typedef struct NODO{
    int id;
    int degree;
    int id_utente;
    int id_file;
    int id_chunck;
}nodo;

typedef struct ARC_STRUCT{
    int adj;
}ARC;

typedef struct NODE_STRUCT{
    int id;
    int degree;
    struct ARC_STRUCT *arcList;
}NODE;

typedef struct RCL_LIST_STRUCT{
    int i;
}RCL_LIST;

typedef struct RANDOM_LIST{
    int idx;
    struct RANDOM_LIST *next;
}random_list;

typedef struct DATA_MATRIX{
    int n_utenti;
    int m_files;
    int b_chunks;
    int ***Ind;
    int *Q;
    int **Q_chuncks;
}data_matrix;

typedef struct CF_DATA{
    int **Matrix_Adj;
    nodo *nodes;
    int n_nodi;
    int ***Ind;
}cf_data;
/** \struct
 * this struct contains the header of the protocol.
 * It's made of */
typedef struct HEADER_TRANSMISSION{
	/**\var padding
	 * \brief (1 byte) is used to identify the last chunk for requested file.*/
	int padding;
	/**\var num_chunks
	 * \brief define how many chunks are send together.*/
	int num_chunks;
	/**\var id_utenti
	 * \brief a list of users identifiers.*/
    vector<int> id_utenti;
    /**\var id_files
     * \brief a list of files identifiers.*/
    vector<int> id_files;
    /**\var id_chunks
     * \brief a list of chunks identifiers.*/
    vector<int> id_chunks;
    /**\var size_package
     * \brief a list of the sizes of every chunk send. This list is useful when padding is 1.*/
    vector<int> size_package;
    /**\var sizeTot
     * \brief Total package size.*/
    int sizeTot;
}header_transmission;

#endif // DATADEFINITION_H
