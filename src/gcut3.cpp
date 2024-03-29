#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include "gcut3.h"
#include "pack2.h"

namespace c3
{

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

    int h = 0;                      // start at the bottom

    // loop over levels
    for( cLevel& level : I.levels() )
    {
        //std::cout << "cutting level h= "<< h <<" " << level.text() << "\n";

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
                ;
                h < level.myStock->myHeight;    // does stock have enough height to stack another level?
                h += level.height() )           // up one level
            {

                // Use 2D cutting algorithm to cut orders from level.

                CS2Pack2( I, level, h );

                // remove packed timbers from level
                int ret = level.removePacked();

                if(  ret == 2 )
                {
                    // all timbers in this level are packed
                    std::cout << "all timbers in this level are packed\n";
                    h += level.height();
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
                h = 0;                      // start at the bottom
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
void CS2Pack2(
    cInstance& I,
    cLevel& level, int h )
{
    //std::cout << "CS2Pack2 " << h << "\n";

    // load level into pack2 engine
    pack2::cPackEngine E;

    E.add( pack2::bin_t( new pack2::cBin( "stock",
                                          level.myStock->myLength,
                                          level.myStock->myWidth )) );
    int k = 0;
    for( space_t& t : level.myOrder )
    {
        E.addItem(
            std::to_string( k++ )+"_"+t->ID(),
            t->myLength, t->myWidth );
        E.items().back()->spinEnable();
    }

    // run the Pack2 engine
    Pack( E );

//    std::cout << "Pack2 cutlist\n";
//    for( auto& c : pack2::CutListEndPoints( E ) )
//    {
//        for( int v : c )
//            std::cout << v << ", ";
//        std::cout << "\n";
//    }
//
//    std::cout << "\nPack2 csv\n";
//    std::cout << pack2::CSV( E );


    // loop over items in level, allocating orders that fit
    int packedCount = 0;
    for( pack2::item_t item : E.items() )
    {
        if( ! item->isPacked() )
            continue;

        packedCount++;
        if( packedCount == 1 )
        {
            /* We are going to cut at least one order for this level from this stock
                first we need to cut the level from the stock.
                so record the level cut in the instance
                unless at top of stock
            */
            CutLevel(
                I,
                level.myStock,
                h + level.height() );
        }

        AllocateOrder(
            I,
            level,
            atoi( item->userID().c_str() ),
            item->locX(), item->locY(), h,
            item->isSpun() );
    }

    // check for nothing packed
    if( ! packedCount )
    {
        std::cout << "nothing fit\n";
        return;
    }

    // at least one order was cut for this stock
    // store cuts in the instance

    /* Loop over cuts in level

        The pack2 engine returns a vector of vectors, one for each bin
        There is only one bin, the level,
        so we need only look at the first vector of cuts
    */
    auto cl = CutList( E )[0];
    for( auto& c : cl )
    {
        // check if cut is at stock boundary
        char LW;
        if( c.myIsVertical )
        {
            // vertical cuts in pack2 ( which is 2d ) are orthogonasl to the length dimension in 3D
            if( c.myIntercept == 0 || c.myIntercept == level.myStock->myLength )
                continue;   // unneccesary cut at stock edge
            LW = 'L';
        }
        else
        {
            // horizontal cuts in pack2 ( which is 2d ) are orthogonasl to the width dimension in 3D
            if( c.myIntercept == 0 || c.myIntercept == level.myStock->myWidth )
                continue;   // unneccesary cut at stock edge
            LW = 'W';
        }
        // construct a 3D cut
        cCut cut(
            level.myStock,
            LW,
            c.myIntercept,
            h );

        //std::cout << cut.text() << "\n\n";

        // add it to the instance cut list
        I.add( cut );
    }
}
void CutLevel(
    cInstance& I,
    space_t stock,
    int h )
{
    //std::cout << "CutLevel " << h <<" "<< stock->text() << "\n";
    stock->level( h );

    if( h >= stock->myHeight )
    {
        //no need for a cut, we are at the top of the stock
        return;
    }
    I.add( cCut(
               stock,
               'H',
               h,
               h ));
}
void AllocateOrder(
    cInstance& I,
    cLevel& level,
    int order,
    int length, int width, int height,
    bool fSpun )
{
    // allocate order to stock
    level.myOrder[ order ]->pack( length, width, height, level.myStock );
    if( fSpun )
        level.myOrder[ order ]->spun();

    // record allocation
    I.allocate( level.myOrder[ order ], level.myStock  );

    // mark stock as used
    level.myStock->used();

    // record how much of level used
    level.use( level.myOrder[ order ] );
}

}
