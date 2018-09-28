/** @file CodingDecodingData.cpp
 *  \brief The file that perform the decoding.
 *    */
#include "CodingDecodingData.h"
#include <cstring>
/**function to decode and save the data */
void decodingData(header_transmission header, vector<char> &coded_data, int m_files, int id_utente, int id_requested_file , string pathFolder, int maxSizePackage){

	log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("decodingData"));

    char *coded_file_buffer = NULL;
    int trovato = 0;
    int id_requested_chunck;
    int size_requested_package;
    int size_package_;

	string chunkUsed;
	string chunkDecoded;
	sub1 << log4cpp::Priority::INFO  <<"header.id_utenti.size(): "<<header.id_utenti.size();

    for (unsigned int i = 0; i < header.id_utenti.size(); i++){
        /*All info for requested package*/
        int id_user = header.id_utenti.at(i);
	
	sub1 << log4cpp::Priority::INFO  <<"header.id_utenti.at(" << i <<"): "<<header.id_utenti.at(i);

        int id_file = header.id_files.at(i);
        int id_chunck = header.id_chunks.at(i);
        if(header.padding==0){
        	size_package_= maxSizePackage;
        }else{
		size_package_=header.size_package.at(i);
        }

	sub1 << log4cpp::Priority::INFO  <<"[id_user,id_utente] = [" << id_user << "," << id_utente << "], [id_file, id_requested_file] = [" << id_file << "," << id_requested_file << "]";

        if (id_user == id_utente && id_file == id_requested_file){	//
	sub1 << log4cpp::Priority::INFO  <<"Sono entrato nell'IF!";
            if (size_package_ > 0){

                trovato = 1;
                id_requested_chunck = id_chunck;
                size_requested_package = size_package_;
                coded_file_buffer = new char[coded_data.size()];
                for (unsigned int k = 0; k < coded_data.size(); k++){
                    coded_file_buffer[k] = coded_data.at(k);
                }

                for (unsigned int j = 0; j < header.id_files.size(); j++){
                    int id_chunck_xor = header.id_chunks.at(j);
                    int id_file_xor = header.id_files.at(j);
                    int size_package_xor = header.size_package.at(j);
                    /*************************************************/
                    int isAlreadyAnalized = 0;
                    unsigned int r = 0;
                    while (r < j && !isAlreadyAnalized){
                        if (id_file_xor == header.id_files.at(r) && id_chunck_xor == header.id_chunks.at(r)){
                            isAlreadyAnalized = 1;
                        }
                        r++;
                    }
                    /*************************************************/
                    if (i != j && (id_file != id_file_xor || id_chunck != id_chunck_xor) && !isAlreadyAnalized && size_package_xor > 0){
                        /********************************** READ CACHE FILE *********************************/
                        char *cache = new char[size_package_xor];
                        // Open file cache for read package
                        string pathFileCache = pathFolder + "/_" + to_string(id_file_xor) + "_" + to_string(id_chunck_xor) + ".cache";
                        chunkUsed = "_" + to_string(id_file_xor) + "_" + to_string(id_chunck_xor) + ".cache";
			
//				    cout << "pathFileCache = " << pathFileCache << endl;

                        ifstream inFilePackage (pathFileCache, ifstream::binary);
                        if (inFilePackage){
                            inFilePackage.read(cache, size_package_xor);
                            inFilePackage.close();
                        }
                        else{
                        	sub1 << log4cpp::Priority::ERROR << "Error reading file cache: " << pathFileCache;
                            exit(0);
                        }
                        /***********************************************************************************/

                        /*Reading packages in cache*/
                        for (int k = 0; k < size_package_xor; k++){
                            coded_file_buffer[k] = coded_file_buffer[k] ^ cache[k];
                        }
                    }
                }
            }
        }
    }

    if (trovato){

        /********************************** DELIVERY PACKAGE *********************************/

        string pathFileDelivery = pathFolder + "/_" + to_string(id_requested_file) + "_" + to_string(id_requested_chunck) + ".cache";
	chunkDecoded = "_" + to_string(id_requested_file) + "_" + to_string(id_requested_chunck) + ".cache";
        sub1 << log4cpp::Priority::INFO  <<"TROVATO -> pathFileDelivery : "<<pathFileDelivery;


//		cout << "pathFileDelivery = " << pathFileDelivery << endl;

        ofstream outFileDelivery (pathFileDelivery, ifstream::binary);
        if (outFileDelivery.is_open()){
            outFileDelivery.write(coded_file_buffer, size_requested_package);
            outFileDelivery.close();
        }
        else{
        	sub1 << log4cpp::Priority::ERROR  << "Error writing delivery package: " << pathFileDelivery ;
            exit(0);
        }
        /************************************************************************************/
    }

    //cout << "chunkUsed: " << chunkUsed <<", chunkDecoded: " << chunkDecoded << endl;

    delete[] coded_file_buffer;
}
