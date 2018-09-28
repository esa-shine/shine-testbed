/** @file hgcc.cpp
 *  \brief A file to perform the hgcc algorithm
 *    */
#include "hgcc.h"

/* Global Variables */
int n_nodi_hgcc, m_archi_hgcc;
int fostar;
int n_utenti_hgcc, m_files_hgcc;
int *b_chuncks_hgcc;

/*Graph Structure  fo Local Search*/
NODE *nodes_hgcc = NULL;

/*Best Solution*/
int *solution_star = NULL;

/*Cache Binary Matrix*/
int ***Ind_hgcc = NULL;

LIST *_list = NULL;

/*Requests Binary Matrix*/
int **Q_hgcc = NULL;

/*Adjacent Matrix*/
int **adj = NULL;

/*Subsets structure*/
SUBSET *subsets_array = NULL;

/****************************************************** READING TXT FILES ********************************************/
void createGraphForHGCC(cf_data outputForColoring){
    int *idx = NULL;
    int **matrixADJ = outputForColoring.Matrix_Adj;
    NODE_LIST *temp_node = NULL;

    Q_hgcc = (int **) malloc (n_utenti_hgcc * sizeof(int*));
    if(!Q_hgcc){
        printf("MEMORY EXCEPTION: Q allocation.\n");
        exit(0);
    }

    for (int i=0; i<n_utenti_hgcc; i++){
        Q_hgcc[i] = (int *) malloc (m_files_hgcc * sizeof(int));
        if (!Q_hgcc[i]){
            printf("MEMORY EXCEPTION: Q allocation.\n");
            exit(0);
        }
    }

    for (int i=0; i<n_utenti_hgcc; i++){
        for (int j=0; j<m_files_hgcc; j++){
            Q_hgcc[i][j] = 0;
        }
    }

    n_nodi_hgcc = outputForColoring.n_nodi;
    m_archi_hgcc = 0;

	if (n_nodi_hgcc > 0){
		_list = (LIST *) malloc (1 * sizeof(LIST));
		if (!_list){
			printf("MEMORY EXCEPTION: head nodes list.\n");
			exit(0);
		}
		_list->nodes_list = NULL;

		nodes_hgcc = (NODE *) malloc (n_nodi_hgcc * sizeof(NODE));
		idx = (int *) malloc (n_nodi_hgcc * sizeof(int));
		if (!nodes_hgcc || !idx) {
			printf("MEMORY EXCEPTION: write graph's nodes - idx array.\n");
			exit(0);
		}

        /*Make requests matrix and list structure*/
		for(int i=0; i<n_nodi_hgcc; i++){
			nodes_hgcc[i].id = outputForColoring.nodes[i].id;
			nodes_hgcc[i].degree = outputForColoring.nodes[i].degree;

			int id_utente = outputForColoring.nodes[i].id_utente;
			int id_file = outputForColoring.nodes[i].id_file;
			int id_chunk = outputForColoring.nodes[i].id_chunck;

			if (nodes_hgcc[i].degree > 0){
				nodes_hgcc[i].arcList = (ARC *) malloc (nodes_hgcc[i].degree * sizeof(ARC));
				idx[i] = 0;
				if (!nodes_hgcc[i].arcList) {
					printf("MEMORY EXCEPTION: write graph's arcs.\n");
					exit(0);
				}
			}else{
				idx[i] = 0;
				nodes_hgcc[i].arcList = NULL;
			}

			if (_list->nodes_list == NULL){
				_list->nodes_list = (NODE_LIST *) malloc (1 * sizeof(NODE_LIST));
				if (!_list->nodes_list){
					printf("MEMORY EXCEPTION: nodes list.\n");
					exit(0);
				}
				_list->nodes_list->id = i;
				_list->nodes_list->degree = nodes_hgcc[i].degree;
				_list->nodes_list->id_chunck = id_chunk;
				_list->nodes_list->id_file = id_file;
				_list->nodes_list->id_utente = id_utente;
				_list->nodes_list->next = NULL;

				Q_hgcc[id_utente][id_file] = 1;
			}else{
				temp_node = (NODE_LIST *) malloc (1 * sizeof(NODE_LIST));
				if (!temp_node){
					printf("MEMORY EXCEPTION: nodes list.\n");
					exit(0);
				}
				temp_node->id = i;
				temp_node->degree = nodes_hgcc[i].degree;
				temp_node->id_chunck = id_chunk;
				temp_node->id_file = id_file;
				temp_node->id_utente = id_utente;
				temp_node->next = _list->nodes_list;
				_list->nodes_list = temp_node;

				Q_hgcc[id_utente][id_file] = 1;
			}
		}

		for(int i=0; i<n_nodi_hgcc; i++){
			for(int k=i+1; k<n_nodi_hgcc; k++){
                if (matrixADJ[i][k] == 1){
                    nodes_hgcc[i].arcList[idx[i]].adj = k;
                    nodes_hgcc[k].arcList[idx[k]].adj = i;

                    idx[i]++;
                    idx[k]++;

                    m_archi_hgcc++;
                }
			}
		}

		adj = matrixADJ;

		free(idx);
		idx = NULL;
	}
}

void createCacheMatrix(cf_data outputForColoring){
    Ind_hgcc = outputForColoring.Ind;
}
/*********************************************************************************************************************/

/************************************************* CHECK CONSTRAINTS *************************************************/
void checkConstraintsHGCC(int *sol){
    int i,j;

    for(i=0;i<n_nodi_hgcc;i++){
        if(sol[nodes_hgcc[i].id]<=0){
            printf("\nCONSTRAINTS VIOLATED: one or more nodes not present in the solution.\n");
            exit(0);
        }

        for(j=0;j<nodes_hgcc[i].degree;j++){
            if(sol[nodes_hgcc[i].id]==sol[nodes_hgcc[i].arcList[j].adj]){
                printf("\nCONSTRAINTS VIOLATED: equal color.\n");
                exit(0);
            }
        }
    }
}
/*********************************************************************************************************************/

/************************************************ IGCC COLORING ALGORITHM ********************************************/
/*Thi function provide to create a list of subsets*/
void computeSubsetsList(){
    int id, id_file, id_chunck, cardinality, i, idx_arr, c;
    NODE_LIST *head = NULL;

    subsets_array = (SUBSET *) malloc (n_utenti_hgcc * sizeof(SUBSET));
    if (!subsets_array){
        printf("MEMORY EXCEPTION: subsets array.\n");
        exit(0);
    }

    for (i=0; i<n_utenti_hgcc; i++){
        subsets_array[i].nodes_list = NULL;
        subsets_array[i].pt_end = NULL;
    }

	/**********************/
	int size_label;
	int *label = NULL;
	/**********************/

    head = _list->nodes_list;
    c = 0;

    /*Each node will be analyzed*/
    while (head != NULL){
        /*Node id*/
        id = head->id;

        /*File id*/
        id_file = head->id_file;

        /*Chunk id*/
        id_chunck = head->id_chunck;

        /*Cardinality of subset*/
        cardinality = 0;

		/**********************/
		/*Size of the label*/
		size_label = 0;
		label = (int *) malloc (n_utenti_hgcc * sizeof(int));
		if (!label){
			printf("MEMORY EXCEPTION: label array.\n");
			exit(0);
		}
		/**********************/

        /*For each user will be analyzed his cache*/
        for (i=0; i<n_utenti_hgcc; i++){
            /*If the user 'i' have in cache the chunk of the file associated to the node analyzed or the user 'i' request the chunk then the user 'i' will be added to the label*/
            if ( Ind_hgcc[i][id_file][id_chunck] == 1 || Q_hgcc[i][id_file] == 1){
                cardinality++;
				label[size_label] = i;
				size_label++;
            }
        }

        idx_arr = cardinality - 1;
        /*The node will be added to the subset*/
        if (subsets_array[idx_arr].nodes_list == NULL){
            subsets_array[idx_arr].nodes_list = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
            if (!subsets_array[idx_arr].nodes_list){
                printf("MEMORY EXCEPTION: nodes list.\n");
                exit(0);
            }

			/*************************/
			subsets_array[idx_arr].nodes_list->label = label;
			subsets_array[idx_arr].nodes_list->size_label = size_label;
			/*************************/
            subsets_array[idx_arr].nodes_list->cardinality = cardinality;
            subsets_array[idx_arr].nodes_list->id = id;
            subsets_array[idx_arr].nodes_list->next = NULL;
            subsets_array[idx_arr].pt_end = subsets_array[idx_arr].nodes_list;
        }else{
            NODE_SUBSET *new_node = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
            if (!new_node){
                printf("MEMORY EXCEPTION: nodes list [new_node].\n");
                exit(0);
            }

			/*************************/
			new_node->label = label;
			new_node->size_label = size_label;
			/*************************/
            new_node->cardinality = cardinality;
            new_node->id = id;
            new_node->next = subsets_array[idx_arr].nodes_list;
            subsets_array[idx_arr].nodes_list = new_node;
        }

        head = head->next;
        c++;
    }

    if (c != n_nodi_hgcc){
        printf("ERROR: dismatch c and number of nodes.\n");
        exit(0);
    }
}

void runProcess(){
    int i, esci;
    int completeUndirect, completeDirect;

    completeDirect = n_nodi_hgcc * (n_nodi_hgcc - 1);
    completeUndirect = completeDirect / 2;

    esci = 0;
    /*Check if graph is completly connect or completly disconnect*/
    if (m_archi_hgcc == 0 || m_archi_hgcc == completeDirect || m_archi_hgcc == completeUndirect){
        solution_star = (int *) malloc (n_nodi_hgcc * sizeof(int));
        if(!solution_star){
            printf("MEMORY EXCEPTION: star solution allocation.\n");
            exit(0);
        }
        for (i=0; i<n_nodi_hgcc; i++){
            /*Graph completly disconnect*/
            if (m_archi_hgcc == 0){
                fostar = 1;
                solution_star[i] = 1;
                esci = 1;
            /*Graph completly connect*/
            }else if (m_archi_hgcc == completeDirect || m_archi_hgcc == completeUndirect){
                fostar = n_nodi_hgcc;
                solution_star[i] = i + 1;
                esci = 1;
            }
        }
    }

    if (!esci){
        /*Core of algorithm*/
        IGCC_ColoringProcess();
    }
}

void IGCC_ColoringProcess(){
    SUBSET *I = NULL;
    NODE_SUBSET *head_I = NULL, *new_I = NULL, *temp_I = NULL;
    NODE_SUBSET *Gi = NULL, *head_Gi1 = NULL, *head_Gi2 = NULL, *new_node = NULL, *prec = NULL, *del = NULL;
    int i, cardinality_I, noEdges, j, inc;

    solution_star = (int *) malloc (n_nodi_hgcc * sizeof(int));
    if(!solution_star){
       printf("MEMORY EXCEPTION: star solution allocation.\n");
       exit(0);
    }

    for (i=0; i<n_nodi_hgcc; i++){
        solution_star[i] = -1;
    }

	/*********************************/
	int size_label;
	int *label = NULL;
	int i_label, i_perm, j_perm;
	/*********************************/

    /*Number of colors associated to the solution*/
    fostar = 0;

    /*For each subsets ...*/
    for (i=(n_utenti_hgcc-1); i>=0; i--){
        Gi = subsets_array[i].nodes_list;
        if (Gi != NULL){
            /*************************************** 1� FOR **********************************************************/
            head_Gi1 = Gi;

            /*For each node in subset 'i' ...*/
            while (head_Gi1 != NULL){
                if (head_Gi1->cardinality == (i+1) && solution_star[head_Gi1->id] == -1){
                    /*Make a subset I*/
                    I = (SUBSET *) malloc (1 * sizeof(SUBSET));
                    if (!I){
                        printf("MEMORY EXCEPTION: I.\n");
                        exit(0);
                    }
                    I->nodes_list = NULL;
                    I->pt_end = NULL;

                    if (I->nodes_list == NULL){
                        I->nodes_list = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                        if (!I->nodes_list){
                            printf("MEMORY EXCEPTION: I.\n");
                            exit(0);
                        }

						/******************************/
						label = head_Gi1->label;
						size_label = head_Gi1->size_label;

						I->nodes_list->label = NULL;
						I->nodes_list->size_label = -1;
						/******************************/

                        I->nodes_list->cardinality = head_Gi1->cardinality;
                        I->nodes_list->id = head_Gi1->id;
                        I->nodes_list->next = NULL;
                        I->pt_end = I->nodes_list;
                    }

                    /*************************************** 2� FOR **************************************************/
                    cardinality_I = 1;
                    head_Gi2 = Gi;
                    /*For each node in subset 'i' ...*/
                    while (head_Gi2 != NULL){
                       if (head_Gi2->cardinality == (i+1) && solution_star[head_Gi2->id] == -1 && head_Gi2->id != head_Gi1->id){

                            /********************************************************************************/
                            /*Check if label of first node is equal to label or permutation of label of the nodes in the same subset*/
                            if (head_Gi2->size_label == size_label){
                                i_label = 0;
                                for (i_perm = 0; i_perm < size_label; i_perm++){
                                    for (j_perm = 0; j_perm < head_Gi2->size_label; j_perm++){
                                        if (label[i_perm] == head_Gi2->label[j_perm]){
                                            i_label++;
                                        }
                                    }
                                }

                                /*If they are the same label the second node will be added to the subset I*/
                                if (i_label == size_label){
                            /********************************************************************************/
                                    noEdges = 1;
                                    head_I = I->nodes_list;
                                    while (head_I != NULL && noEdges){
                                        if (adj[head_I->id][head_Gi2->id] == 1){
                                            noEdges = 0;
                                        }
                                        head_I = head_I->next;
                                    }

                                    if (noEdges){
                                        new_I = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                                        if (!new_I){
                                            printf("MEMORY EXCEPTION: I.\n");
                                            exit(0);
                                        }

                                        /************************************/
                                        new_I->label = NULL;
                                        new_I->size_label = -1;
                                        /************************************/

                                        new_I->cardinality = head_Gi2->cardinality;
                                        new_I->id = head_Gi2->id;
                                        new_I->next = NULL;
                                        I->pt_end->next = new_I;
                                        I->pt_end = new_I;

                                        cardinality_I++;
                                    }
                               }
                            }
                       }

                       head_Gi2 = head_Gi2->next;
                    }
                    /*************************************************************************************************/

                    /*Coloring the nodes in subset I*/
                    if (cardinality_I == (i+1)){
                        fostar++;
                        head_I = I->nodes_list;
                        while(head_I != NULL){
                            solution_star[head_I->id] = fostar;
                            head_I = head_I->next;
                        }
                    }

                    head_I = I->nodes_list;
                    while(head_I != NULL){
                        temp_I = head_I;
                        head_I = head_I->next;
                        free(temp_I);
                        temp_I = NULL;
                    }
                    I->pt_end = NULL;
                    free(I);
                    I = NULL;
                }

                head_Gi1 = head_Gi1->next;
            }
            /*********************************************************************************************************/

            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/
            /*********************************************************************************************************/

            /****************************************** 3� FOR *******************************************************/
            head_Gi1 = Gi;
            prec = Gi;
            /*For each node in subset 'i' ...*/
            while (head_Gi1 != NULL){
                inc = 1;
                /*If the node not have the color yet*/
                if (solution_star[head_Gi1->id] == -1){
                    /*Make a subset I*/
                    I = (SUBSET *) malloc (1 * sizeof(SUBSET));
                    if (!I){
                        printf("MEMORY EXCEPTION: I.\n");
                        exit(0);
                    }
                    I->nodes_list = NULL;
                    I->pt_end = NULL;

                    if (I->nodes_list == NULL){
                        I->nodes_list = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                        if (!I->nodes_list){
                            printf("MEMORY EXCEPTION: I.\n");
                            exit(0);
                        }

                        /******************************/
						label = head_Gi1->label;
						size_label = head_Gi1->size_label;

						I->nodes_list->label = NULL;
						I->nodes_list->size_label = -1;
						/******************************/

                        I->nodes_list->cardinality = head_Gi1->cardinality;
                        I->nodes_list->id = head_Gi1->id;
                        I->nodes_list->next = NULL;
                        I->pt_end = I->nodes_list;
                    }

                    /*************************************** 4� FOR **************************************************/
                    cardinality_I = 1;
                    head_Gi2 = Gi;
                    /*For each node in subset 'i' ...*/
                    while (head_Gi2 != NULL){
                       if (solution_star[head_Gi2->id] == -1 && head_Gi2->id != head_Gi1->id){

                            /********************************************************************************/
                            i_label = 0;
                            /*Check if label of first node is equal to label or permutation of label of the nodes in the same subset*/
                            for (i_perm = 0; i_perm < size_label; i_perm++){
                                for (j_perm = 0; j_perm < head_Gi2->size_label; j_perm++){
                                    if (label[i_perm] == head_Gi2->label[j_perm]){
                                        i_label++;
                                    }
                                }
                            }

                            /*If they are the same label the second node will be added to the subset I*/
                            if (i_label == size_label){
                            /********************************************************************************/
                                noEdges = 1;
                                head_I = I->nodes_list;
                                while (head_I != NULL && noEdges){
                                    if (adj[head_I->id][head_Gi2->id] == 1){
                                        noEdges = 0;
                                    }
                                    head_I = head_I->next;
                                }

                                if (noEdges){
                                    new_I = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                                    if (!new_I){
                                        printf("MEMORY EXCEPTION: I.\n");
                                        exit(0);
                                    }

                                    /************************************/
                                    new_I->label = NULL;
                                    new_I->size_label = -1;
                                    /************************************/
                                    new_I->cardinality = head_Gi2->cardinality;
                                    new_I->id = head_Gi2->id;
                                    new_I->next = NULL;
                                    I->pt_end->next = new_I;
                                    I->pt_end = new_I;

                                    cardinality_I++;
                                }
                            }
                       }

                       head_Gi2 = head_Gi2->next;
                    }
                    /*************************************************************************************************/

                    /*Coloring the nodes in subset I*/
                    if (cardinality_I >= (i+1)){
                        fostar++;
                        head_I = I->nodes_list;
                        while(head_I != NULL){
                            solution_star[head_I->id] = fostar;
                            head_I = head_I->next;
                        }
                    }else{
                        /*Move the nodes without color to the next level*/
                        j = i-1;

                        /*If next level is equal at the first level*/
                        if (j >= 0){
                            /*If the level 'j' have no nodes then the nodes without color will be added to it*/
                            if (subsets_array[j].nodes_list == NULL){
                                new_node = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                                if (!new_node){
                                    printf("MEMORY EXCEPTION: new node.\n");
                                    exit(0);
                                }

                                /*************************************/
                                int indice;
                                new_node->label = (int *) malloc (n_utenti_hgcc * sizeof(int));
								if (!new_node->label){
									printf("MEMORY EXCEPTION: label array.\n");
									exit(0);
								}

                                for (indice = 0; indice < head_Gi1->size_label; indice++){
                                    new_node->label[indice] = head_Gi1->label[indice];
                                }
                                new_node->size_label = head_Gi1->size_label;
                                /*************************************/

                                new_node->cardinality = head_Gi1->cardinality;
                                new_node->id = head_Gi1->id;
                                new_node->next = NULL;

                                subsets_array[j].nodes_list = new_node;
                                subsets_array[j].pt_end = new_node;
                            }else{
                            /*If the level 'j' already have nodes then the nodes without color will be added to the tail of the nodes list*/
                                new_node = (NODE_SUBSET *) malloc (1 * sizeof(NODE_SUBSET));
                                if (!new_node){
                                    printf("MEMORY EXCEPTION: new node.\n");
                                    exit(0);
                                }

                                /*************************************/
                                int indice;
                                new_node->label = (int *) malloc (n_utenti_hgcc * sizeof(int));
								if (!new_node->label){
									printf("MEMORY EXCEPTION: label array.\n");
									exit(0);
								}

                                for (indice = 0; indice < head_Gi1->size_label; indice++){
                                    new_node->label[indice] = head_Gi1->label[indice];
                                }
                                new_node->size_label = head_Gi1->size_label;
                                /*************************************/

                                new_node->cardinality = head_Gi1->cardinality;
                                new_node->id = head_Gi1->id;
                                new_node->next = NULL;

                                subsets_array[j].pt_end->next = new_node;
                                subsets_array[j].pt_end = new_node;
                            }
                        }


                        /*Remove colored node*/
                        if (head_Gi1->id == subsets_array[i].nodes_list->id){ //head_Gi1 == prec
                            del = head_Gi1;
                            prec = prec->next;
                            subsets_array[i].nodes_list = prec;

                            if (subsets_array[i].pt_end == head_Gi1){
                                subsets_array[i].pt_end = NULL;
                            }

                            /*Freeing all variables*/
                            free(del->label);
                            del->label = NULL;
                            free(del);
                            del = NULL;
                            Gi = head_Gi1 = prec = subsets_array[i].nodes_list;
                            inc = 0;
                        }else{
                            del = head_Gi1;
                            prec->next = head_Gi1->next;

                            if (subsets_array[i].pt_end == head_Gi1){
                                subsets_array[i].pt_end = prec;
                            }

                            /*Freeing all variables*/
                            free(del->label);
                            del->label = NULL;
                            free(del);
                            del = NULL;

                            head_Gi1 = prec;
                        }
                    }

                    /*Freeing all variables*/
                    head_I = I->nodes_list;
                    while(head_I != NULL){
                        temp_I = head_I;
                        head_I = head_I->next;
                        free(temp_I);
                        temp_I = NULL;
                    }
                    I->pt_end = NULL;
                    free(I);
                    I = NULL;
                }

                if (inc){
                    prec = head_Gi1;
                    head_Gi1 = head_Gi1->next;
                }
            }
            /*********************************************************************************************************/
        }
    }
}
/*********************************************************************************************************************/

/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
/*This function try to improve the current solution. This procedure try to decrese the number of color utilized*/
int *localSearch(int *fo){
    int i, pos_col, j, k, adj_id, x, col, esci;
    int n_col = (*fo);
    int new_col = -1;

    int *temp_sol = NULL;

    int *colors = NULL;
    colors = (int *) malloc (n_col * sizeof(int));
    if (!colors){
        printf("MEMORY EXCEPTION: colors allocation.\n");
        exit(0);
    }

    for (x=0; x<n_col; x++){
        colors[x] = -1;
    }

    /*For each color, the function try to remove it*/
    for (k=0; k<n_col; k++){
        /*'col' is the color that the function try to remove*/
        col = k + 1;

        temp_sol = (int *) malloc (n_nodi_hgcc * sizeof(int));
        if (!temp_sol){
            printf("MEMORY EXCEPTION: temporany solution allocation.\n");
            exit(0);
        }

         /*Make a copy of the current solution*/
        for (x=0; x<n_nodi_hgcc; x++){
            temp_sol[x] = solution_star[x];
        }

        i = 0;
        esci = 0;
        /*Local search procedure*/
        /*For each node will be analysed his neighborhood*/
        while (i<n_nodi_hgcc && !esci){
            if (temp_sol[nodes_hgcc[i].id] == col){
                for (j=0; j<nodes_hgcc[i].degree; j++){
                    adj_id = nodes_hgcc[i].arcList[j].adj;
                    pos_col = temp_sol[adj_id] - 1;
                    colors[pos_col] = 1;
                }

                new_col = -1;
                x = 0;
                while (x<n_col && new_col == -1){
                    /*If there is a color not utilized in the node's neighborhood, this color will replace the old*/
                    if (colors[x] == -1 && x != k){
                        new_col = x + 1;
                    }
                    x++;
                }

                /*Color replacement*/
                if (new_col != -1){
                    temp_sol[nodes_hgcc[i].id] = new_col;
                }else esci = 1;

                for (x=0; x<n_col; x++){
                    if (colors[x] != 2){
                        colors[x] = -1;
                    }
                }
            }

            i++;
        }

        /*If all nodes associated to the color 'col', will be associated to a new color, 'col' will be removed*/
        if (i == n_nodi_hgcc && !esci){
            free(solution_star);
            solution_star = temp_sol;
            (*fo)--;
            colors[k] = 2;
        }else{
            free(temp_sol);
            temp_sol = NULL;
        }
    }

    free(colors);
    colors = NULL;

    return solution_star;
}
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/
/****************************************************************************************************************************************/

int *hGCC(int *n_col, data_matrix data, cf_data outputForColoring){
    NODE_SUBSET *head = NULL, *temp = NULL;

    n_utenti_hgcc = data.n_utenti;
    m_files_hgcc = data.m_files;
    b_chuncks_hgcc = data.b_chunks;

    /*Reading file text that contains a single graph instance*/
    createGraphForHGCC(outputForColoring);

    /*Reading file text that contains a matrix which represents the file cache*/
    createCacheMatrix(outputForColoring);

    if (n_nodi_hgcc > 0){
        /*First step: Create the subsets*/
        computeSubsetsList();

        /*Second step: Run the HGCC Algorithm*/
        runProcess();

        /*Check if the solution is admissible*/
        checkConstraintsHGCC(solution_star);

        /*Third step: Local search to improve the solution*/
        solution_star = localSearch(&fostar);

        /*Check if the solution is admissible*/
        checkConstraintsHGCC(solution_star);

        /*Freeing all variables*/
        for(int i=0; i<n_nodi_hgcc; i++){
            if (nodes_hgcc[i].arcList != NULL){
                free(nodes_hgcc[i].arcList);
                nodes_hgcc[i].arcList = NULL;
            }
        }

        free(nodes_hgcc);
        nodes_hgcc = NULL;

        for (int k=0; k<n_utenti_hgcc; k++){
            head = subsets_array[k].nodes_list;
            while (head != NULL){
                temp = head;
                head = head->next;
                free(temp);
                temp = NULL;
            }
            subsets_array[k].pt_end = NULL;
        }

        free(subsets_array);
        subsets_array = NULL;
    }else{
        fostar = 0;
        solution_star = (int *) malloc (1 * sizeof(int));
        solution_star[0] = -1;
        n_nodi_hgcc = 1;
    }

    (*n_col) = fostar;

    return solution_star;
}
