/** @file grasp.cpp
 *  \brief A file to perform the grasp algorthm
 *    */
#include "grasp.h"

/* Global Variables */
int n_nodes, m_edges;
int fo_star;
int maxIterations;
int descending;

NODE *nodi_grafo = NULL;
int *sol_star = NULL;

void graspProcess(){
    int i, esci;
    int completeUndirect, completeDirect;

    completeDirect = n_nodes * (n_nodes - 1);
    completeUndirect = completeDirect / 2;

    esci = 0;
    /*Check if graph is completly connect or completly disconnect*/
    if (m_edges == 0 || m_edges == completeDirect || m_edges == completeUndirect){
        sol_star = (int *) malloc (n_nodes * sizeof(int));
        if(!sol_star){
            printf("MEMORY EXCEPTION: star solution allocation.\n");
            exit(0);
        }
        for (i=0; i<n_nodes; i++){
            /*Graph completly disconnect*/
            if (m_edges == 0){
                fo_star = 1;
                sol_star[i] = 1;
                esci = 1;
            /*Graph completly connect*/
            }else if (m_edges == completeDirect || m_edges == completeUndirect){
                fo_star = n_nodes;
                sol_star[i] = i + 1;
                esci = 1;
            }
        }
    }

    if (!esci){
        /*Core of algorithm*/
        runGrasp();
    }
}

void runGrasp(){
    int x;
    int *sol;
    int fo;
    float alpha;

    /*Number of colors associated to the best solution found*/
    fo_star = INF;
    /*At each iteration will create admissible solution*/
    for (x=0; x<maxIterations; x++){
        /*The parameter alpha denote the greedy's degree*/
        alpha = randomNumber(0, 1);

        /*Number of colors associated to the current solution*/
        fo = 0;

        /*First step: Creation of solution*/
        sol = constructGreedyRandomizedSolution(&fo, alpha);

        /*Second step: Local search to improve the solution*/
        sol = localSearch(sol, &fo);

        /*Check if the solution is admissible*/
        checkConstraints(sol);

        if (fo < fo_star){
            fo_star = fo;
            if (sol_star){
                //free(sol_star);
                sol_star = NULL;
            }
            sol_star = sol;
        }
    }
}

/**This function provide to create an admissible solution*/
int *constructGreedyRandomizedSolution(int *fo, float alpha){
    int i, index, n_rcl, col;
    NODE *node;
    RCL_LIST *rcl = NULL;

    int *sol = (int *) malloc (n_nodes * sizeof(int));
    if(!sol){
        printf("MEMORY EXCEPTION: local solution allocation.\n");
        exit(0);
    }

    for (i=0; i<n_nodes; i++){
        sol[i] = -1;
    }

    /*At each iteration will be chosen a node in a random way. The node chosen will colored*/
    for (i=0; i<n_nodes; i++){
        /*Creation of the Restrected Candidates List*/
        rcl = makeRCL(alpha, &n_rcl, sol);

        /*Get a random index*/
        index = round(randomNumber(0, n_rcl-1));

        /*Get node from RCL*/
        node = &(nodi_grafo[rcl[index].i]);

        /*Get the compatible color in according to node's neighborhood*/
        col = getColor(node, fo, sol);

        /*Put the node in the solution*/
        sol[node->id] = col;

        free(rcl);
        rcl = NULL;
    }

    return sol;
}

/**This function provide to create a Restricted Candidates List*/
RCL_LIST *makeRCL(float alpha, int *n_rcl, int *sol){
    RCL_LIST *rcl = NULL;
    float tao;
    int minDegree, maxDegree;
    int i, n, trovato, j;

    /*If the sort is ascending*/
    if (!descending){
        n = 0;
        i = 0;
        trovato = 0;
        /*Find a minimum node's degree*/
        while (i<n_nodes && !trovato){
            if (sol[nodi_grafo[i].id] == -1){
               minDegree = nodi_grafo[i].degree;
               trovato = 1;
            }
            i++;
        }

        i = n_nodes - 1;
        trovato = 0;
        /*Find a maximum node's degree*/
        while (i>=0 && !trovato){
            if (sol[nodi_grafo[i].id] == -1){
               maxDegree = nodi_grafo[i].degree;
               trovato = 1;
            }
            i--;
        }

        /*Compute parameter tau*/
        tao = maxDegree + (alpha * (minDegree - maxDegree));
        tao = round(tao);

        i = 0;
        /*Calcuate the number of nodes that have a degree <= tau*/
        while (nodi_grafo[i].degree <= tao && i < n_nodes){
            if (sol[nodi_grafo[i].id] == -1){
                n++;
            }
            i++;
        }

        if (n<=0){
            printf("MEMORY EXCEPTION: RCL dimension: tao: %f MinDegree: %d MaxDegree: %d.\n", tao, minDegree, maxDegree);
            exit(0);
        }

        rcl = (RCL_LIST *) malloc (n * sizeof(RCL_LIST));
        if (!rcl){
            printf("MEMORY EXCEPTION: RCL allocation.\n");
            exit(0);
        }

        i = 0;
        j = 0;
        /*Insert the nodes that have a degree <= tau in the RCL*/
        while (nodi_grafo[i].degree <= tao && i < n_nodes){
            if (sol[nodi_grafo[i].id] == -1){
                rcl[j].i = i;
                j++;
            }
            i++;
        }
    }else{
    /*If the sort is descending*/
        n = 0;
        i = 0;
        trovato = 0;
        /*Find a maximum node's degree*/
        while (i<n_nodes && !trovato){
            if (sol[nodi_grafo[i].id] == -1){
               maxDegree = nodi_grafo[i].degree;
               trovato = 1;
            }
            i++;
        }

        i = n_nodes - 1;
        trovato = 0;
        /*Find a minimum node's degree*/
        while (i>=0 && !trovato){
            if (sol[nodi_grafo[i].id] == -1){
               minDegree = nodi_grafo[i].degree;
               trovato = 1;
            }
            i--;
        }

        /*Compute parameter tau*/
        tao = minDegree + (alpha * (maxDegree - minDegree));
        tao = round(tao);

        i = 0;
        while (nodi_grafo[i].degree >= tao && i < n_nodes){
            if (sol[nodi_grafo[i].id] == -1){
                n++;
            }
            i++;
        }

        if (n<=0){
            printf("MEMORY EXCEPTION: RCL dimension: tao: %f MinDegree: %d MaxDegree: %d.\n", tao, minDegree, maxDegree);
            exit(0);
        }


        rcl = (RCL_LIST *) malloc (n * sizeof(RCL_LIST));
        if (!rcl){
            printf("MEMORY EXCEPTION: RCL allocation.\n");
            exit(0);
        }

        i = 0;
        j = 0;
        /*Insert the nodes that have a degree >= tau in the RCL*/
        while (nodi_grafo[i].degree >= tao && i < n_nodes){
            if (sol[nodi_grafo[i].id] == -1){
                rcl[j].i = i;
                j++;
            }
            i++;
        }
    }

    (*n_rcl) = n;

    return rcl;
}

/**This function try to improve the current solution. This procedure try to decrese the number of color utilized*/
int *localSearch(int *sol, int *fo){
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

        temp_sol = (int *) malloc (n_nodes * sizeof(int));
        if (!temp_sol){
            printf("MEMORY EXCEPTION: temporany solution allocation.\n");
            exit(0);
        }

        /*Make a copy of the current solution*/
        for (x=0; x<n_nodes; x++){
            temp_sol[x] = sol[x];
        }

        i = 0;
        esci = 0;
        /*Local search procedure*/
        /*For each node will be analysed his neighborhood*/
        while (i<n_nodes && !esci){
            if (temp_sol[nodi_grafo[i].id] == col){
                for (j=0; j<nodi_grafo[i].degree; j++){
                    adj_id = nodi_grafo[i].arcList[j].adj;
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
                    temp_sol[nodi_grafo[i].id] = new_col;
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
        if (i == n_nodes && !esci){
            free(sol);
            sol = temp_sol;
            (*fo)--;
            colors[k] = 2;
        }else{
            free(temp_sol);
            temp_sol = NULL;
        }
    }

    free(colors);
    colors = NULL;

    return sol;
}

void checkConstraints(int *sol){
    int i,j;
    for(i=0;i<n_nodes;i++){
        if(sol[nodi_grafo[i].id]<=0){
            printf("\nCONSTRAINTS VIOLATED: one or more nodes not present in the solution.\n");
            exit(0);
        }

        for(j=0;j<nodi_grafo[i].degree;j++){
            if(sol[nodi_grafo[i].id]==sol[nodi_grafo[i].arcList[j].adj]){
                printf("\nCONSTRAINTS VIOLATED: equal color.\n");
                exit(0);
            }
        }
    }
}

/***************************************************************************************************/
/**This function identifies, in according to the node's neighborhood, the color that can be associated to the node*/
int getColor(NODE *node, int *fo, int *sol){
    int col, i, trovato, adj_id, pos_col;
    int *colors = NULL;
    int n_col = (*fo);

    /*If the number of colors utilized is is greater than zero*/
    if (n_col > 0){
        /*'colors' is a binary array, colors[i] = -1 indicates that the color 'i' is not utilized*/
        colors = (int *) malloc (n_col * sizeof(int));
        if (!colors){
            printf("MEMORY EXCEPTION: colors allocation.\n");
            exit(0);
        }

        for (i=0; i<n_col; i++){
            colors[i] = -1;
        }

        /*Neighbors analysis: all colors associated to the neighbors of the node will be marked*/
        for (i=0; i<node->degree; i++){
            adj_id = node->arcList[i].adj;
            if (sol[adj_id] != -1){
                pos_col = sol[adj_id] - 1;
                colors[pos_col] = 1;
            }
        }

        trovato = 0;
        i = 0;
        /*Check if exist a color not marked*/
        while (i<n_col && !trovato){
            if (colors[i] == -1){
                trovato = 1;
            }
            i++;
        }

        /*If the color exist*/
        if (trovato){
            col = i;
        }else{
        /*If the color not exist*/
            (*fo)++;
            col = (*fo);
        }
    }else{
    /*If the number of colors utilized is equal to zero*/
       (*fo)++;
       col = (*fo);
    }

    if (colors){
        free(colors);
        colors = NULL;
    }

    return col;
}

void createGraph(cf_data outputForColoring){
    int *idx = NULL;
    int **matrixADJ = outputForColoring.Matrix_Adj;
    n_nodes = outputForColoring.n_nodi;
    m_edges = 0;

	if (n_nodes > 0){
		nodi_grafo = (NODE *) malloc (n_nodes * sizeof(NODE));
		idx = (int *) malloc (n_nodes * sizeof(int));
		if (!nodi_grafo || !idx) {
			printf("MEMORY EXCEPTION: write graph's nodes - idx array.\n");
			exit(0);
        }

		for(int i=0; i<n_nodes; i++){
			nodi_grafo[i].id = outputForColoring.nodes[i].id;
			nodi_grafo[i].degree = outputForColoring.nodes[i].degree;
			if (nodi_grafo[i].degree > 0){
				nodi_grafo[i].arcList = (ARC *) malloc (nodi_grafo[i].degree * sizeof(ARC));
				idx[i] = 0;
				if (!nodi_grafo[i].arcList) {
					printf("MEMORY EXCEPTION: write graph's arcs.\n");
					exit(0);
                }
			}else{
				idx[i] = 0;
				nodi_grafo[i].arcList = NULL;
			}
		}

		for(int i=0; i<n_nodes; i++){
			for(int k=i+1; k<n_nodes; k++){
                if (matrixADJ[i][k] == 1){
                    nodi_grafo[i].arcList[idx[i]].adj = k;
                    nodi_grafo[k].arcList[idx[k]].adj = i;

                    idx[i]++;
                    idx[k]++;

                    m_edges++;
                }
			}
		}

		free(idx);
		idx = NULL;
	}
}

/***************************************************************************************************/
/**Main Function - Input: Max Numbers Of GRASP's Iterations*/
int *graspGraphColoring(int maxIter, cf_data outputForColoring, int *n_col)
{
    descending = 1;

    maxIterations = maxIter;

    createGraph(outputForColoring);

    if (n_nodes > 0){
        /*Sort nodes of the graph by their degree*/
        if (!descending)
            ascendingHeapSort(nodi_grafo, n_nodes);
        else
            descendingHeapSort(nodi_grafo, n_nodes);

        /*Start GRASP process*/
        graspProcess();

        for(int i=0; i<n_nodes; i++){
            if (nodi_grafo[i].arcList != NULL){
                free(nodi_grafo[i].arcList);
                nodi_grafo[i].arcList = NULL;
            }
        }

        free(nodi_grafo);
        nodi_grafo = NULL;
    }else{
        fo_star = 0;
        //printf("\nProcesso completato, valore della funzione obiettivo (Numero Cromatico): %d\n\n",fo_star);
        sol_star = (int *) malloc (1 * sizeof(int));
        sol_star[0] = -1;
        n_nodes = 1;
    }

    (*n_col) = fo_star;
    return sol_star;
}
