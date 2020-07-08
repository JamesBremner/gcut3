#include <sstream>
#include "gcut3.h"
namespace c3 {
std::string cCut::text()
{
    std::stringstream ss;
    ss << "c " << myStock->ID() << " "
       << myDirection <<" "<< myLocation <<" "
       << myLevelHeight;
    return ss.str();
}
}
