#ifndef __INSIGNIA__DFBFX_H__
#define __INSIGNIA__DFBFX_H__

#include <direct/Types++.h>
#include <++dfb.h>

#include <string>

extern "C" {
#include <directfb.h>
#include <core/state.h>
}


class DFBfx {
public:
     static DFBColor BlitPixel( const CardState *state, DFBColor src, DFBColor dst );
     static void     DumpState( const CardState *state, bool blitting );
};

#endif

