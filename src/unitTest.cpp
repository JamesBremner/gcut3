#include <iostream>
#include <memory>
#include <cutest.h>
#include "gcut3.h"

using namespace std;

TEST( tid9 )
{
//    i 1000 600 200 1 i1
//d 500 300 200 4 o1

    c3::cInstance I;
    c3::space_t T( new c3::cSpace(1000, 600, 200 ) );
    T->ID( "i1" );
    I.addStock( T );
    c3::space_t O( new c3::cSpace(500, 300, 200 ) );
    O->ID( "o1" );
    O->count( 4 );
    I.addOrder( O );
    I.expandCount();

    // sort orders into levels of the same height
    Levels( I );

    // allocate levels to stock
    LevelsToStock( I );

    // cut out orders from stock by level
    LevelCuts( I );

     std::cout << I.textSolution();

    // check cut count and positions
    CHECK_EQUAL( 2, I.cutListSize() );
    CHECK_EQUAL( 300, I.cutListPosition(0) );
    CHECK_EQUAL( 500, I.cutListPosition(1) );
    CHECK_EQUAL( 0, I.unPackedCount() );
}

TEST( twoLevels )
{
//    i 1000 600 400 1 i1
//d 500 300 200 8 o1

    c3::cInstance I;
    c3::space_t T( new c3::cSpace(1000, 600, 400 ) );
    T->ID( "i1" );
    I.addStock( T );;
    c3::space_t O( new c3::cSpace(500, 300, 200 ) );
    O->ID( "o1" );
    O->count( 8 );
    I.addOrder( O );
    I.expandCount();


    // sort orders into levels of the same height
    Levels( I );

    // allocate levels to stock
    LevelsToStock( I );

    // cut out orders from stock by level
    LevelCuts( I );

    //std::cout << I.textSolution();

    // check cut count and positions
    CHECK_EQUAL( 5, I.cutListSize() );
    CHECK_EQUAL( 200, I.cutListPosition(0) );
    CHECK_EQUAL( 300, I.cutListPosition(1) );
    CHECK_EQUAL( 500, I.cutListPosition(2) );
    CHECK_EQUAL( 300, I.cutListPosition(3) );
    CHECK_EQUAL( 500, I.cutListPosition(4) );
    CHECK_EQUAL( 0, I.unPackedCount() );
}

TEST( shortStock )
{
//    i 1000 600 400 1 i1
//d 500 300 200 12 o1

    c3::cInstance I;
    c3::space_t T( new c3::cSpace(1000, 600, 400 ) );
    T->ID("i1");
        I.addStock( T );
    c3::space_t O( new c3::cSpace(500, 300, 200 ) );
    O->ID("o1");
    O->count( 12 );
    I.addOrder( O );
    I.expandCount();

    Levels( I );

    // allocate levels to stock
    LevelsToStock( I );

    // cut out orders from stock by level
    LevelCuts( I );

    //std::cout << I.textSolution();

    // check cut count and positions
    CHECK_EQUAL( 5, I.cutListSize() );
    CHECK_EQUAL( 200, I.cutListPosition(0) );
    CHECK_EQUAL( 300, I.cutListPosition(1) );
    CHECK_EQUAL( 500, I.cutListPosition(2) );
    CHECK_EQUAL( 300, I.cutListPosition(3) );
    CHECK_EQUAL( 500, I.cutListPosition(4) );
    CHECK_EQUAL( 4, I.unPackedCount() );
}

TEST( twoStock )
{
//    i 1000 600 400 2 i1
//d 500 300 200 12 o1

    c3::cInstance I;
    c3::space_t T( new c3::cSpace(1000, 600, 400 ) );
    T->ID("i1");
    T->count( 2 );
        I.addStock( T );
    c3::space_t O( new c3::cSpace(500, 300, 200 ) );
    O->ID("o1");
    O->count( 12 );
    I.addOrder( O );

    I.expandCount();

    // sort orders into levels of the same height
    Levels( I );

    // allocate levels to stock
    LevelsToStock( I );

    // cut out orders from stock by level
    LevelCuts( I );

    //std::cout << I.textSolution();

    // check cut count and positions
    CHECK_EQUAL( 8, I.cutListSize() );
    CHECK_EQUAL( 200, I.cutListPosition(0) );
    CHECK_EQUAL( 300, I.cutListPosition(1) );
    CHECK_EQUAL( 500, I.cutListPosition(2) );
    CHECK_EQUAL( 300, I.cutListPosition(3) );
    CHECK_EQUAL( 500, I.cutListPosition(4) );
    CHECK_EQUAL( 0, I.unPackedCount() );
}

int main(  )
{
    raven::set::UnitTest::RunAllTests();

}
