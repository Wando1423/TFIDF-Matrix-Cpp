
# TFIDF in C++

Implementation of file-similarity check using TFIDF.


### How it's done

Creates Term-Document Matrix built upon .txt files in the specified directory. Then each word in the texts has calculateda specific weight and value for every file.

### Using the project

A basic implementation of how it could be used is included within project in the console window, but it can be used in any different way. The only requirement is to provide the exact path to files (to compare) and the string you want them to be compared with

### Code

It's using modern C++17 filesystem functions to get all the files inside the directory and stringstream to get all the needed data from them.
