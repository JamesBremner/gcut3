#pragma once
#include <memory>
#include <vector>

namespace c3
{

class cSpace;
typedef std::shared_ptr< cSpace > space_t;
typedef std::vector< space_t > spaceV_t;


/// A 3D object with dimension and location
class cSpace
{
public:
    int myLength, myWidth, myHeight;    // dimensions
    int myLocL, myLocW, myLocH;


    cSpace( int L, int W, int H )
        : myLength( L ), myWidth( W ), myHeight{ H }
        , myCount( 1 )
        , myPacked( false )
        , myUsed( false )
    {

    }
    cSpace()
        : cSpace( 0, 0, 0 )
    {

    }

    int ParseSpaceDelimited(
        const std::string& l );

    spaceV_t expand();

    /** Cut the order
    @param[in] l length position
    @param[in] w width position
    @param[in] k height position
    @param[in] stock from which order is cut
    */
    void pack( int l, int w, int h, space_t stock );

    bool fit( const cSpace& squeeze ) const
    {
        return ( squeeze.myLength <= myLength &&
                 squeeze.myLength <= myLength &&
                 squeeze.myHeight <= myHeight );
    }

    int size_horiz()
    {
        return myLength * myWidth;
    }
    /// rotate so that L, W, H are in decending size order
    void rotateLWH();

    std::string ID() const
    {
        return myUserID;
    }
    void ID( const std::string& id )
    {
        myUserID = id;
    }
    bool isPacked() const
    {
        return myPacked;
    }
    void usedbyLevel( bool f )
    {
        myUsedbyLevel = f;
    }
    bool isUsedbyLevel()
    {
        return myUsedbyLevel;
    }
    void count( int c )
    {
        myCount = c;
    }
    // set the location of the top of the highest level cut
    void level( int h )
    {
        myLevel = h;
    }
    int level() const
    {
        return myLevel;
    }
        void used( bool f = true )
    {
        myUsed = f;
    }
    bool isUsed() const
    {
        return myUsed;
    }
    std::string text();

private:
    int         myCount;
    std::string myUserID;
    bool        myPacked;      // true if an order that has been allocated
    bool        myUsedbyLevel;  // true if stock has been allocated to current level
    int         myLevel;         // the location of the top of the highest level cut
    bool        myUsed;        // true if a stock that has been used to cut orders
    space_t     myStock;   // if an allocated order, the stock allocated to
};

class cCut
{
public:
    space_t myStock;
    char myDirection;
    int myLocation;
    int myLevelHeight;
    cCut( space_t stock, char D, int loc, int levelHeight )
        : myStock( stock )
        , myDirection( D )
        , myLocation( loc )
        , myLevelHeight( levelHeight )
    {

    }
    std::string text();
};

/// orders that have the same height
class cLevel
{
public:
    spaceV_t myOrder;      // orders
    space_t  myStock;      // stock allocated to level

    cLevel()
        : myAreaUsed( 0 )
    {

    }

    /** remove orders that have been packed
        @return 0: none packed, 1: some packed, 2: all packed
    */
    int removePacked();

    std::string text() const;
    int height() const
    {
        return myOrder[0]->myHeight;
    }
    int size() const
    {
        return (int) myOrder.size();
    }
    void use( space_t order )
    {
        myAreaUsed += order->myLength * order->myWidth;
    }
    /** Volume wasted when level cut from stock
        @return volume wasted

        This calculates wastage only for the currently stacked level
        so this needs to be called for each stacked level
    */
    int wastage() const;

private:
    int myAreaUsed;
};

class cInstance
{
public:
    void read(
        const std::string& fname );

    void cut();

    std::string textProblem();
    std::string textSolution();

    /** Expand multiple timbers
        @param[in] tv vector of timbers
    */
    static void expandCount( spaceV_t& tv );


    void addStock( space_t t );
    void addOrder( space_t t );
    void addUnpacked( spaceV_t& unpacked );

    /** Expand multiple timbers
    @param[in] tv vector of timbers
    */
    void expandCount();

    /// Number of orders that could not fit
    int unPackedCount() const
    {
        return (int)myUnpacked.size();
    }

    bool isEveryIDUnique();

    /** Record allocation in solution
        @param[in] order
        @param[in] stock
    */
    void allocate( space_t order, space_t stock );

    void add( const cCut& cut )
    {
        myCut.push_back( cut );
    }
    int cutListSize() const
    {
        return (int)myCut.size();
    }
    /** cut position along orthogonal axis
        @param[in] index of cut in cut list
        @return position, -1 on error
    */
    int cutListPosition( int index )
    {
        if( 0 > index || index > (int)myCut.size() - 1 )
            return -1;
        return myCut[ index ].myLocation;
    }

    /// rotate, if neccesary, so L > W > H
    void rotateLWH();

    /// sort by height
    void sortByHeight();

    spaceV_t& orders()
    {
        return myOrder;
    }

    std::vector< cLevel >& levels()
    {
        return myLevels;
    }

    spaceV_t& stock()
    {
        return myStock;
    }

private:

    spaceV_t       myStock;

    spaceV_t       myOrder;            /// the timbers that have to be delivered

    std::vector< cLevel > myLevels;

    spaceV_t       myUnpacked;         /// orders that could not be met from the inventory

    std::vector<std::pair<space_t,space_t>>
                                         myAllocation;         /// stock that order was cut from

    std::vector< cCut > myCut;          /// cuts required

    /// Parse a line in the instance file
    std::vector< int > ParseSpaceDelimited(
        const std::string& l );

};
/** allocate timbers of same height to levels
@param[in] I the problem instance
*/
void Levels( cInstance& I );

/** allocate levels to stock
    @param[out] I the instance
*/
void
LevelsToStock(
    cInstance& I );

/** allocate a level to a stock
    @param[out] I the instance
    @param[in] level
    @param[in] stock inventory
    @return true if allocation successful
*/
bool
LevelToStock(
    cInstance& I,
    cLevel& level,
    spaceV_t& stock );

/** cut out the orders
    @param[out] I the instance
*/
void LevelCuts(
    cInstance& I );

/** Cutting stock 2 dimensional no width
    @param[out] I the instance
    @param[in level
    @param[in] h height in stock
    @return true if all timbers in level were packed

    This is an extremely simple 2D cutting stock algorithm
    to be used as a placeholder in the H3CS algorithm.

    This makes no effort to stack in the width dimension!

    The effect is that the orders will be cut from each level
    in one line along the side of the stock timbers.
    The wastage is likely to be enormous!
*/
bool CS2LNW(
    cInstance& I,
    cLevel& level, int h );

/// Use pack2 engine to do 2D level cutting
void CS2Pack2(
    cInstance& I,
    cLevel& level, int h );

/** Allocate an order
    @param[in] I the instance
    @param[in] level
    @param[in] order index in level
    @param[[in] length position
    @param[[in] width position
    @param[[in] height position
*/
void AllocateOrder(
    cInstance& I,
    cLevel& level,
    int order,
    int length, int width, int height );

/** Record the V cut for a level in the instance
    @param[in] I the instance
    @param[in] stock
    @param[in] h height for cut at top of level
*/
void CutLevel(
    cInstance& I,
    space_t stock,
    int h );

void ReturnToInventory(
    cInstance& I );

void DisplayWastage(
    cInstance& I );

/// Use pack2 engine to do 2D level cutting
void CS2Pack2(
    cInstance& I,
    cLevel& level, int h );

/** Record the V cut for a level in the instance
@param[in] I the instance
@param[in] stock
@param[in] h height for cut at top of level
*/
void CutLevel(
    cInstance& I,
    space_t stock,
    int h );
}
