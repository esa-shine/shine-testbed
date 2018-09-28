
/** @file EnvironmentSetup.cpp
 *  \brief A file to setup the environment when the server starts
 *    */
#include "EnvironmentSetup.h"

int n_utenti_env;
int m_files_env;
int *b_chuncks_env;

int *Q_array = NULL;

/******************************************************************************************************************************************/

/*This is a function that provide to generate, for each user, a random request*/
void randomQVector(double **probs){
    int i, idx_rand;

    /******************************************************************************************/
    unsigned long int randSeed = 0;
    gsl_rng *r;
    gsl_ran_discrete_t *rand_disc;

    srand(time(NULL));                    /* initialization for rand() */
    randSeed = rand();                    /* returns a non-negative integer */

    gsl_rng_env_setup();
    gsl_rng_default_seed = randSeed;

    r = gsl_rng_alloc(gsl_rng_default);

    /******************************************************************************************/
    /*For each user generate a random request*/
  //  int k=0;
    for (i=0; i<n_utenti_env; i++){

		rand_disc = gsl_ran_discrete_preproc (m_files_env, probs[i]);
        idx_rand =  gsl_ran_discrete (r, rand_disc);

		if (idx_rand >= m_files_env || idx_rand < 0){
			printf("\nError: Random Index.\n");
			exit(0);
		}/////////////////////****************************////////////////////////
		//if(i==5)k=0;
        Q_array[i] = idx_rand; //=k;
       // k++;
    }
}

/*This is a function that provide to generate a binary random cache matrix*/
void randomIndMatrix(float memory_discretizzata, int **M, vector<int> &input, int number_of_request, vector<int> memory_per_user){
    int i, j, k,  idx_rand, x, idx_sel;
    int  n_list;
    random_list *l = NULL, *temp = NULL, *h = NULL;

    int index_input = input.size() - 1;

    for (i=0; i<n_utenti_env; i++){
        input.at(index_input) = number_of_request;
        index_input--;
        input.at(index_input) = Q_array[i];
        index_input--;
        input.at(index_input) = memory_per_user.at(i);
        index_input--;

        for (j=0; j<m_files_env; j++){
            /*Make the list that contains the chunk's id*/
            for (k=0; k<b_chuncks_env[j]; k++){
                if (l == NULL){
                    l = (random_list *) malloc (1 * sizeof(random_list));
                    l->idx = k;
                    l->next = NULL;
                }else{
                    temp = (random_list *) malloc (1 * sizeof(random_list));
                    temp->idx = k;
                    temp ->next = NULL;
                    h = l;
                    l = temp;
                    temp->next = h;
                }
            }

            n_list = b_chuncks_env[j];
            /*The matrix M indicates the number of chunks related to files 'j' that user 'i' can memorize*/
            for (k=0; k<M[i][j]; k++){
                /*Random index for extract from the id chunks list the id chunk, related to files 'j', that the user 'i' must memorize*/
                idx_rand = round(randomNumber(0, n_list-1));
                if (idx_rand == 0){
                    idx_sel = l->idx;
                    //Ind[i][j][idx_sel] = 1;
                    input.at(index_input) = j;
                    index_input--;
                    input.at(index_input) = idx_sel;
                    index_input--;

                    h = l;
                    l = l->next;
                    free(h);
                    h = NULL;

                    n_list--;
                }else{
                    h = l;
                    x = 0;

                    while (x<(idx_rand-1)){
                        h = h->next;
                        x++;
                    }

                    temp = h;
                    h = h->next;
                    idx_sel = h->idx;
                    //Ind[i][j][idx_sel] = 1;

                    input.at(index_input) = j;
                    index_input--;
                    input.at(index_input) = idx_sel;
                    index_input--;

                    temp->next = h->next;
                    free(h);
                    h = NULL;

                    n_list--;
                }
            }
            while(l != NULL){
                h = l;
                l = l->next;
                free(h);
                h = NULL;
            }
        }
    }

    if (index_input >=0){
        printf("Error input vector!! \n\n");
        exit(0);
    }

    /*Check if all free memory slot have been filled */
    //check_Ind_Matrix();
}

/*void check_Ind_Matrix(){
    int i, j, k, sum;

    sum = 0;
    for (i=0; i<n_utenti_env; i++){
        for (j=0; j<m_files_env; j++){
            for (k=0; k<b_chuncks_env; k++){
                if (Ind[i][j][k] == 1){
                    sum++;
                }
            }
        }
    }

    if (sum != memory_discretizzata ){
        printf("\nError: randomIndMatrix Function.\n");
		exit(0);
    }
}*/

/******************************************************************************************************************************************/
void _allocVariables(){
    Q_array = (int *) malloc (n_utenti_env * sizeof(int));
    check_memory_allocation_1D(Q_array, "Q Vector.");
}

void _deallocVariables(){
    free(Q_array);
    Q_array = NULL;
}
/******************************************************************************************************************************************/


vector <int>setEnvironment(int n_utenti_in, int m_files_in, int* b_chuncks_in, double **probs, int **M){

	vector <int> input;
    vector <int> memory_per_user(n_utenti_in, 0);
    int input_size;
    int number_of_request = 1;

    n_utenti_env = n_utenti_in;
    m_files_env = m_files_in;
    b_chuncks_env = b_chuncks_in;

    _allocVariables();
    float memory_discretizzata = 0;
    for (int k=0; k<n_utenti_env; k++){
        for (int j=0; j<m_files_env; j++){
            memory_discretizzata += M[k][j];
            memory_per_user.at(k) += M[k][j];
        }
    }

    input_size = memory_discretizzata * 2 + (n_utenti_env * (2 + number_of_request));
    input.resize(input_size);

    /*Generate the random vector of the user's requests*/
    randomQVector(probs);


    /*Generate the random binary matrix for the user's cache*/
    randomIndMatrix(memory_discretizzata, M, input, number_of_request, memory_per_user);
//    cout<<"b_chuncks[id_file] con envsetup: "<<0<<" : "<<b_chuncks_in[0]<<endl;
    _deallocVariables();

    /* Input variables deallocation */
    for (int i = 0; i < n_utenti_env; i++){
        free(probs[i]);
    }

    for (int i = 0; i < n_utenti_env; i++){
        free(M[i]);
    }

    free(M);
    free(probs);

    return input;
}
