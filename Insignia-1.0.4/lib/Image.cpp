#include <++dfb.h>

#include <sys/wait.h>

#include "Image.h"

namespace Insignia {

/***********************************************************************************************************************
** Constructor/Destructor
*/

Image::Image()
{
}

Image::Image( const char *filename )
{
     Load( filename );
}

Image::~Image()
{
}

/***********************************************************************************************************************
** Loading
*/

void
Image::Load( const char *filename )
{
     IDirectFB dfb = DirectFB::Create();

     IDirectFBImageProvider provider = dfb.CreateImageProvider( filename );

     provider.GetImageDescription( &m_image_desc );


     DFBSurfaceDescription  desc;

     provider.GetSurfaceDescription( &desc );

     m_surface = dfb.CreateSurface( desc );

     provider.RenderTo( m_surface, NULL );
}


}

