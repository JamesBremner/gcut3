#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "gcut3.h"

using namespace std;

namespace c3
{

 int cSpace::ParseSpaceDelimited(
    const std::string& l )
{
    std::vector< std::string > token;
    std::stringstream sst(l);
    std::string a;
    while( getline( sst, a, ' ' ) )
        token.push_back( a );
    if( token.size() < 6 )
        throw std::runtime_error(" Error reading: " + l );
    myLength = atoi(token[1].c_str());
    myWidth = atoi(token[2].c_str());
    myHeight = atoi(token[3].c_str());
    myCount = atoi(token[4].c_str());
    myUserID = token[5];

    if( token[0] == "i" )
        return 0;
    else  if( token[0] == "d" )
        return 1;
    else
        throw std::runtime_error(" Error reading: " + l );
}


spaceV_t cSpace::expand()
{
    if( myCount <= 0 )
        throw std::runtime_error("Bad count for " + myUserID );

    spaceV_t ex;

    // construct required number of copies, append copy number to userID
    for( int k = 0; k < myCount-1; k++ )
    {
        space_t newTimber( new cSpace( *this ) );
        newTimber->myUserID = myUserID + ":" + std::to_string( k+2 );
        newTimber->count(1);
        ex.push_back( newTimber );
    }
    myCount = 1;
    return ex;
}
void cSpace::rotateLWH()
{
    std::vector<int> vLWH { myLength, myWidth, myHeight };
    std::sort( vLWH.begin(), vLWH.end(),
               []( int& a, int& b)
    {
        return a > b;
    });
    myLength = vLWH[0];
    myWidth  = vLWH[1];
    myHeight = vLWH[2];

//    for( int i : vLWH )
//        std::cout << i <<" ";
//    std::cout << "\n";
}

std::string cSpace::text()
{
    std::stringstream ss;
    ss <<  myLength <<" "<< myWidth <<" "<< myHeight <<" "<< myCount <<" " << myUserID;
    return ss.str();
}
}
