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
        // sort orders into levels of the same height
        Levels( *this );

        // allocate levels to stock
        LevelsToStock( *this );

        // cut out orders from stock by level
        LevelCuts( *this );
}

void cInstance::expandCount()
{
    expandCount( myOrder );
    expandCount( myStock );
}
void cInstance::expandCount( spaceV_t& tv )
{
    spaceV_t ex;
    for( auto& s : tv )
    {
        spaceV_t sv = s->expand();
        ex.insert(
                  ex.end(),
                  sv.begin(), sv.end() );
    }
    std::cout << "cInstance::expandCount " << tv.size() <<" "<< ex.size() << "\n";
    tv.insert(
        tv.end(),
        ex.begin(), ex.end() );
}

std::string cInstance::textProblem()
{
    std::stringstream ss;
    for( auto& s : myOrder )
    {
        ss << s->text() << "\n";
    }
    return ss.str();
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

