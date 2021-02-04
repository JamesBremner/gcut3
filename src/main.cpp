#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "gcut3.h"

using namespace std;

int main( int argc, char* argv[] )
{
    cout << "GCUT3" << endl;

    if( argc != 2 )
    {
        std::cout << "Usage: gcut3 <path to instance file>\n";
        exit(1);
    }

    try
    {
        c3::cInstance I;

        // read problem instance file
        I.read( argv[1] );

        I.cut();

        // display solution
        std::cout << I.textSolution();

        ofstream of("gcut3results.txt");
        of << I.textSolution();

    }
    catch( std::runtime_error& e )
    {
        std::cout << "exception: " << e.what() << "\n";
        ofstream of("gcut3results.txt");
        of << "exception: " << e.what() << "\n";
    }
    return 0;
}
