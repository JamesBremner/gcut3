#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include "gcut3.h"

namespace c3 {

void Levels( cInstance& I )
{
    I.rotateLWH();

    I.sortByHeight();

    // allocate items of same height to a level
    int h_level = -1;
    for( space_t t : I.orders() )
    {
        if( t->myHeight != h_level )
        {
            // create a new level
            h_level = t->myHeight;
            cLevel level;
            I.levels().push_back( level );
        }

        // add to current level
        I.levels().back().myOrder.push_back( t );
    }

    for( cLevel& l : I.levels() )
        std::cout << l.text() <<"\n";
}
void LevelsToStock(
    cInstance& I )
{
    for( space_t t : I.stock() )
        t->usedbyLevel( false );
    for ( auto& level : I.levels() )
    {
        LevelToStock( I, level, I.stock() );
    }
}
bool
LevelToStock(
    cInstance& I,
    cLevel& level,
    spaceV_t& stock )
{
    if( ! stock.size() )
        throw std::runtime_error("No stock");

    int least_waste = std::numeric_limits<int>::max();
//        int least_stock_height = std::numeric_limits<int>::max();
    space_t best_stock = stock[0];
    int level_height = level.height();
    bool found = false;
    for( space_t t : stock )
    {
        // check if this stock was previously allocated to this level
        // if so, it means that the level could not be packed into the stock
        // probably because the W and/or length were too short
        // in any case we do not want to allocate the stock again
        if( t->isUsedbyLevel() )
            continue;

        int stockHeight = t->myHeight;
        if( stockHeight < level_height )
            continue;
        if( stockHeight == level_height )
        {
            found = true;
            least_waste = 0;
            best_stock = t;
            break;
        }
        int waste = stockHeight % level_height;
        if( waste < least_waste )
        {
            found = true;
            least_waste = waste;
            best_stock = t;

            //std::cout << best_stock->myUserID << " " << waste << "\n";
        }
    }

    // check for no stock available
    if( ! found )
    {
        std::cout << "level " << level_height << " no stock available\n";
        return false;
    }

    std::cout << "level " << level_height << " order count " << level.size()
              << " allocated stock " << best_stock->text() << "\n";

    level.myStock = best_stock;

    return true;
}
void LevelCuts(
    cInstance& I )
{
    bool allPacked;

    // loop over levels
    for( cLevel& level : I.levels() )
    {
        std::cout << "cutting level " << level.text() << "\n";

        for( space_t t : I.stock() )
            t->usedbyLevel( false );

        do
        {
            /* stack levels

            If not all the orders in a level can be fitted into the stock timber
            at one level, perhaps the stock can be cut into several levels to fit them

            */
            allPacked = false;
            for(
                int h = 0;                      // start at the bottom
                h < level.myStock->myHeight;    // does stock have enough height to stack another level?
                h += level.height() )           // up one level
            {

                // Use 2D cutting algorithm to cut orders from level.

#ifdef USE_CS2LNW
                allPacked = CS2LNW( I, level, h );
#endif // USE_CS2LNW
#ifdef USE_CS2Pack2
                CS2Pack2( I, level, h );
#endif // CS2Pack2

                // remove packed timbers from level
                int ret = level.removePacked();

                if(  ret == 2 )
                {
                    // all timbers in this level are packed
                    std::cout << "all timbers in this level are packed\n";
                    allPacked = true;
                    break;
                }

                if( ret == 1 )
                {
                    // some packed
                    continue;
                }

                if( ret == 0 )
                {
                    // nothing packed
                    level.myStock->usedbyLevel( true );
                    break;
                }

            }  // loop back to cut another level from the stock

            if( ! allPacked )
            {
                // stock timber exhausted, need to allocate another
                if( ! LevelToStock( I, level, I.stock() ) )
                {
                    // no suitable stock available
                    std::cout << " no suitable stock available\n";
                    I.addUnpacked( level.myOrder );
                    return;
                }
            }

        }
        while( ! allPacked );
    }
    if( allPacked )
    {
        // success
        return;
    }
}

}
