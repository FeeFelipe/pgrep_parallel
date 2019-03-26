//
//  main.cpp
//  EP01
//
//  Created by Felipe Cavalieri on 19/03/19.
//  Copyright © 2019 Felipe Cavalieri. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <regex>
#include <string>
#include <dirent.h>

using namespace std;

struct Line {
    int number;
    std::string text;
};

struct File {
    std::string fileName;
    vector<Line> lines;
};

struct Path {
    std::string pathName;
    std::string pathType;
    vector<File> files;
    vector<Path> paths;
};

struct ReadDirectory {
    std::string path;
    std::string exp;
};

struct arg_struct {
    int arg1;
    int arg2;
};

vector<Path> read_directory(string path, string exp)
{
    vector<Path> vPath;
    ifstream openFile;
    string line;
    DIR* dirPatch = opendir(path.c_str());
    struct dirent * file;

    while ((file = readdir(dirPatch)) != NULL) {
        Path sPath;

        if (strcmp(file->d_name, ".") == 0 or strcmp(file->d_name, "..") == 0) continue;

        sPath.pathName = path;
        if (file->d_type == DT_DIR)
        {
            sPath.pathType = "directory";

            std::stringstream directory;
            directory << sPath.pathName << "/" << file->d_name;

            sPath.paths = read_directory(directory.str(), exp);
        } else {
            File sFile;

            sFile.fileName = file->d_name;
            sPath.pathType = "file";

            std::stringstream directoryFile;
            directoryFile << sPath.pathName << "/" << sFile.fileName;
            openFile.open(directoryFile.str());

            if (openFile.good())
            {
                while (getline(openFile, line))
                {
                    regex e(exp);
                    bool match = regex_search(line, e);
                    if (match) {
                        cout << "Retornou File[" << line << "] " << sFile.fileName << endl;
                    }
                }
            }

            sPath.files.push_back(sFile);
            openFile.close();
        }

        vPath.push_back(sPath);
    }
    closedir(dirPatch);

    return vPath;
}

void show_file(vector<File> files) {
    int size = (int) files.size();

    for (int i = 0; i < size; i++) {
        cout << "Retornou File[" << files[i].fileName << "]" << endl;

    }
}

void show_path(vector<Path> paths) {
    int size = (int) paths.size();

    for (int i = 0; i < size; i++) {
        cout << "Retornou Path[" << paths[i].pathName << "] Type[" << paths[i].pathType << "]" << endl;

        if (strcmp(paths[i].pathType.c_str(), "file") == 0)
        {
            show_file(paths[i].files);
        } else
        {
            show_path(paths[i].paths);
        }
    }
}

void *pgrep_thread(void *args) {
    struct ReadDirectory *readDirectory = (struct ReadDirectory *)args;
    
    vector<Path> rread_directory = read_directory(readDirectory->path, readDirectory->exp);
    
    show_path(rread_directory);
    
    pthread_exit(NULL);
    return NULL;
}

void pgrep(int max_threads, string regex_pesquisa, string caminho_diretorio) {
    int rc;
    void *status;
    pthread_t threads[max_threads];
    struct ReadDirectory readDirectory;
    readDirectory.path = caminho_diretorio;
    readDirectory.exp = regex_pesquisa;

    for(int i = 0; i < max_threads; i++ ) {
        cout << "main() : creating thread, " << i << endl;
        rc = pthread_create(&threads[i], NULL, &pgrep_thread, (void *)&readDirectory);

        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        } else {
            cout << readDirectory.path << endl;
            cout << readDirectory.exp << endl;
        }
    }
    
    for(int i = 0; i < max_threads; i++ ) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
        
        cout << "Main: completed thread id :" << i ;
        cout << "  exiting with status :" << status << endl;
    }
    
}

int main()
{
    pgrep(5, "class.*", "/Users/felipecavalieri/Documents/poc");
    
    
    return EXIT_SUCCESS;
}
