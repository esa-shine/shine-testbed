/** @file utilityForTesting.cpp
 *  \brief  A file containing utility function
 *    */
#include "utilityForTesting.h"

/*************************************************************************************************************/
/**function to get a vector of files contained in a directory*/
vector<string> getDirectoryFiles(string &directory){
    vector<string> files;

    //Windows
    /*HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return files;
    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (file_name[0] == '.')
            continue;

        if (is_directory)
            continue;

        files.push_back(full_file_name);
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);*/

    //LINUX
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(directory.c_str());
    if (dir){
        while ((ent = readdir(dir)) != NULL) {
            const string file_name = ent->d_name;
            const string full_file_name = directory + "/" + file_name;

            if (file_name[0] == '.')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            files.push_back(full_file_name);
        }
    }
    closedir(dir);

    return files;
}
/**function to know the size of a file*/
double getFileSize(string filepath){
    ifstream in(filepath, std::ifstream::ate | std::ifstream::binary);
    double size_ = in.tellg();
    in.close();

    return size_;
}

/*************************************************************************************************************/
