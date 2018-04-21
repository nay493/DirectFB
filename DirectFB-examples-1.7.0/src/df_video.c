#include <directfb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "/home/nayeem/Desktop/DirectFB/DirectFB-examples-1.7.0/src/util.h"


#define APP "dfbtest_stereo_window"


#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
      }                                                        \
  }

IDirectFB  *dfb = NULL;
DFBResult   ret;
IDirectFBDisplayLayer  *layer      = NULL;
IDirectFBScreen        *screen     = NULL;

int start(int argc, char *argv[]);


static DFBResult
create_window_1( IDirectFBDisplayLayer  *layer,int x,int y,IDirectFBWindow **ret_window,IDirectFBSurface **ret_surface ,int temp)
{
    DFBResult             ret;
    IDirectFBWindow      *window;
    IDirectFBSurface     *surface;
    DFBWindowDescription  window_desc;
    IDirectFBFont *font;
    const char *FONT1="/usr/share/fonts/truetype/tlwg/TlwgTypo-Bold.ttf";
    DFBFontDescription Fdesc;
    Fdesc.flags= DFDESC_HEIGHT;
    Fdesc.height=20;
    window_desc.flags  = DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_POSX | DWDESC_POSY | DWDESC_CAPS;
    /*nay@    If the window is not menu then create it with greater width & height compared to width & height of menu window*/
       
        window_desc.width  = 100;
        window_desc.height = 50;

    window_desc.posx   = x;
    window_desc.posy   = y;
    window_desc.caps   = DWCAPS_ALPHACHANNEL | DWCAPS_LR_MONO;

         ret = layer->CreateWindow( layer, &window_desc, &window );
         if (ret) {
          D_DERROR( ret, APP ": IDirectFBDisplayLayer::CreateWindow() failed!\n" );
          return ret;
         }

         ret = window->GetSurface( window, &surface );
         if (ret) {
          D_DERROR( ret, APP ": IDirectFBWindow::GetSurface() failed!\n" );
          return ret;
         }

    surface->Clear( surface, 0x00, 0x80, 0xa0, 0xd0 );
    dfb->CreateFont( dfb, FONT1, &Fdesc, &font );
    surface->SetFont( surface, font );
    surface->SetColor( surface, 0xff, 0xff, 0xff, 0xff );
surface->DrawString( surface, "A", -1,60, 360, DSTF_TOPLEFT );
    static IDirectFBInputDevice *keyboard = NULL;
    IDirectFBImageProvider *provider;
    int sprite_x, sprite_y, max_x, max_y;
    static IDirectFBSurface *primary = NULL;
    DFBSurfaceDescription   dsc;
    dsc.flags = DSDESC_CAPS;
    dsc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
    DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
    surface->SetColor( surface, 0xff, 0x00, 0x00, 0x00 );//nay@
    surface->Flip( surface, NULL, DSFLIP_NONE );
    window->SetOpacity( window, 0xff );
    //window->CreateEventBuffer( window, &buffer );
    *ret_window  = window;
    *ret_surface = surface;
    return DFB_OK;
}


int main( int argc, char *argv[] )
{
   
    IDirectFBWindow        *window1    = NULL;
    IDirectFBSurface       *surface1   = NULL;
    IDirectFBSurface       *p_surface   = NULL;
    IDirectFBWindow* active = NULL;

start(argc,argv);

IDirectFBEventBuffer *buf;
DFBInputDeviceCapabilities caps;
caps=    DICAPS_ALL ;
DFBBoolean     global=DFB_TRUE;
dfb->CreateInputEventBuffer(dfb,caps,global,&buf);

DFBInputEvent evt;//input event
DFBWindowEvent event;//window event
DFBEvent *event_dfb;

while(1)
{
ret = create_window_1( layer, 10, 100, &window1, &surface1,1 );


/*
while(buf->GetEvent(buf, DFB_EVENT(&evt))  == DFB_OK)
{
    if (evt.type == DIET_KEYPRESS)
    {
        switch(evt.key_symbol)
        {
        case DIKS_0:D_ERROR("DIKS_0\n");break;
        case DIKS_SMALL_A:D_ERROR("DIKS_SMALL_A\n");break;
        case DIKS_F1:D_ERROR("DIKS_F1\n");break;
        default:D_ERROR("default KEYPRESS\n");break;
        }
    }

    else if (evt.type == DIET_BUTTONRELEASE)
    {
        switch(evt.button)
        {
        case DIBI_LEFT:DIBM_LEFT:D_ERROR("DIBI_LEFT\n");break;
        case DIBI_RIGHT:DIBM_RIGHT:D_ERROR("DIBI_RIGHT\n");break;
        case DIBI_MIDDLE:DIBM_MIDDLE:D_ERROR("DIBI_MIDDLE\n");break;

        default:D_ERROR("default BUTTONPRESS\n");break;
        }
    }
    else
    {
        //D_ERROR("final \n");
    }
}
*/



while(buf->GetEvent(buf, DFB_EVENT(&event))  == DFB_OK)
if(evt.type ==DWET_GOTFOCUS)
{
    switch(event.type)
    {
    default:break;
    }
}
else if(evt.type ==DWET_BUTTONDOWN )
{
        switch(evt.button)
        {
        case DIBI_LEFT:DIBM_LEFT:D_ERROR("DIBI_LEFT\n");break;
        case DIBI_RIGHT:DIBM_RIGHT:D_ERROR("DIBI_RIGHT\n");break;
        case DIBI_MIDDLE:DIBM_MIDDLE:D_ERROR("DIBI_MIDDLE\n");break;

        default:D_ERROR("default BUTTONPRESS\n");break;
        }
}

}//while(1)




}
