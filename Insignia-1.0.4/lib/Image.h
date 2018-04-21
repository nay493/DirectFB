#ifndef __INSIGNIA__IMAGE_H__
#define __INSIGNIA__IMAGE_H__

#include "common.h"


namespace Insignia {

     class Image {
     public:
          Image();
          Image( const char *filename );
          ~Image();
     
          void Load( const char *filename );

     private:
          IDirectFBSurface    m_surface;
          DFBImageDescription m_image_desc;
     };

}

#endif
