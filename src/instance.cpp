#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include "gcut3.h"

using namespace std;

namespace c3
{

void cInstance::read(
    const std::string& fname )
{
    std::ifstream f( fname );
    if( ! f.is_open() )
        throw std::runtime_error("Cannot read instance file " + fname );

    myOrder.clear();

    // loop over lines in file
    std::string line;
    while( std::getline( f, line ) )
    {
        //std::cout << lcount << " " << line << "\n";
        space_t T( new cSpace() );
        switch ( T->ParseSpaceDelimited( line ) )
        {
        case 0:     // inventory
            myStock.push_back( T );
            break;
        case 1:     // demand
            myOrder.push_back( T );
            break;
        }
    }
    isEveryIDUnique();
    expandCount();
}

void cInstance::cut()
{

}

void cInstance::expandCount()
{
    expandCount( myOrder );
    expandCount( myStock );
}
void cInstance::expandCount( spaceV_t& tv )
{
//    spaceV_t ex;
//    for( auto& t : tv )
//    {
//        if( t->myCount <= 0 )
//            throw std::runtime_error("Bad count for " + t->myUserID );
//
//        // construct required number of copies, append copy number to userID
//        for( int k = 0; k < t->myCount-1; k++ )
//        {
//            space_t newTimber( new cTimber( *t.get() ) );
//            newTimber->myUserID = t->myUserID + ":" + std::to_string( k+2 );
//            ex.push_back( newTimber );
//        }
//    }
//    tv.insert(
//        tv.end(),
//        ex.begin(), ex.end() );
}

std::string cInstance::textSolution()
{
    std::stringstream ss;
    for( auto& a : myAllocation )
    {
        ss << "a " << a.second->ID()
           <<" "<< a.first->ID() << "\n";
    }
    for( auto& c : myCut )
    {
//        ss << c.text() << "\n";
        ss << "todo: cutlist text" << "\n";
    }
    for( auto& u : myUnpacked )
    {
        ss << "u " << u->ID() << "\n";
    }
    for(  auto& t : myStock )
    {
        ss << "i " << t->myLength <<" "<< t->myWidth <<" "<< t->myHeight
           << " 1 " << t->ID() << "\n";
    }
    return ss.str();
}
void cInstance::addStock( space_t t )
{
    myStock.push_back( t );
}
void cInstance::addOrder( space_t t )
{
    myOrder.push_back( t );
}
void cInstance::addUnpacked( spaceV_t& unpacked )
{
    myUnpacked.insert(
        myUnpacked.end(),
        unpacked.begin(), unpacked.end() );
}
bool cInstance::isEveryIDUnique()
{
    std::set<std::string> IDset, dupIDset;
    bool first = true;
    for( space_t t: myOrder )
    {
        if( ! IDset.insert( t->ID() ).second )
        {
            if( dupIDset.insert( t->ID() ).second )
            {
                if( first )
                {
                    std::cout << "Discarding duplicate IDs ( tid11 ):\n";
                    first = false;
                }
                std::cout << t->ID() << " ";
            }
        }
    }

    myOrder.erase(
        remove_if(
            myOrder.begin(),
            myOrder.end(),
            [&dupIDset] ( space_t t )
    {
        return( dupIDset.find(t->ID()) != dupIDset.end() );
    } ),
    myOrder.end() );

    std::cout <<  "\n" << myOrder.size() << " unique order IDs\n";
    return true;
}
void cInstance::allocate( space_t order, space_t stock )
{
    myAllocation.push_back( std::make_pair( order, stock ));
}

void cInstance::rotateLWH()
{
    for( auto t : myOrder )
    {
        t->rotateLWH();
    }
    for( auto t : myStock )
    {
        t->rotateLWH();
    }
}
void cInstance::sortByHeight()
{
    std::sort( myOrder.begin(), myOrder.end(),
               []( space_t a, space_t b)
    {
        return a->myHeight < b->myHeight;
    });
}
}

