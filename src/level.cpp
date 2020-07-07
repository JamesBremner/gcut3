#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "gcut3.h"

namespace c3
{
int cLevel::removePacked()
{
    int start = size();
    if( ! start )
        throw std::runtime_error( "removePacked");

    myOrder.erase(
        remove_if(
            myOrder.begin(),
            myOrder.end(),
            [] ( space_t t )
    {
        return( t->isPacked() );
    } ),
    myOrder.end() );

    int stop = size();
    int ret;
    if( stop == start )
        ret = 0;
    else if( stop )
        ret = 1;
    else
        ret = 2;

    if( ret )
        myStock->myHeight -= height();

    return ret;
}
int cLevel::wastage() const
    {
        if( ! size() )
            return 0;
        return height() * ( myStock->myLength * myStock->myWidth - myAreaUsed );
    }
std::string cLevel::text() const
{
    std::stringstream ss;
    ss <<"level " << height() <<" ( ";
    for( space_t t : myOrder )
    {
        ss << t->ID() << " ";
    }
    ss << ") ";
    if( myStock )
        ss << "stock " << myStock->ID();
    return ss.str();
}
}


