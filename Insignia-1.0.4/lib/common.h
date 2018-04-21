#ifndef __INSIGNIA__COMMON_H__
#define __INSIGNIA__COMMON_H__

#include <config.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <iostream>

#include <++dfb.h>

#include <directfb.h>
#include <directfb_util.h>

#include <direct/String.h>
#include <direct/ToString.h>

extern "C" {
#include <direct/direct.h>
#include <core/state.h>
#include <gfx/convert.h>
}

using namespace std;




/* Testing some flag automation. */
template<class Enum> class FlagSet {

public:
     FlagSet() : m_flags(0) {
     }

     FlagSet( int flags ) : m_flags(flags) {
     }


     inline void operator = (int flags) {
          m_flags = flags;
     }

     inline void operator |= (int flags) {
          m_flags |= flags;
     }

     inline int operator & (int flags) const {
          return (m_flags & flags);
     }

     inline operator Enum() const {
          return (Enum) m_flags;
     }

private:
     int m_flags;
};







#endif
