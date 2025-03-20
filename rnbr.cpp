#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Incorrect usage.\n";
        cerr << "Correct usage is\n";
        cerr << "rnbr <file>.rnb";
    }

    string outfilename = string(argv[1] + string(".bat"));
    ifstream runner("run.bat"); 

    ofstream outputfile(argv[1] + string(".bat"));
    ifstream inputFile(argv[1] + string(".rnb"));
    stringstream buffer;
    buffer << inputFile.rdbuf();
    string fileContents = buffer.str();
    outputfile << fileContents;
    inputFile.close();
    outputfile.close();
    system(outfilename.c_str());
    system(("del " + outfilename).c_str());
    return (0);
}