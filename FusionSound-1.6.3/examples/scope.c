#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <directfb.h>

#include <lite/lite.h>
#include <lite/window.h>

#include <fusionsound.h>


static IFusionSound              *sound    = NULL;
static IFusionSoundMusicProvider *provider = NULL;
static IFusionSoundBuffer        *buffer   = NULL;
static IFusionSoundStream        *stream   = NULL;

static LiteWindow                *window;


static void
draw_scope( s16 *data, int len )
{
     IDirectFBSurface *surface;
     DFBRegion        *r;
     int               w, h;
     int               i, j, s;

     surface = window->box.surface;
     surface->AddRef( surface );
     surface->GetSize( surface, &w, &h );

     r = alloca( w * sizeof(DFBRegion) );
     s = (len << 16) / w;

     r[0].x1 =
     r[0].x2 = 0;
     r[0].y1 =
     r[0].y2 = h/2 + ((data[0]*h) >> 16);
     for (i = 1, j = s; i < w; i++, j += s) {
          r[i].x1 =
          r[i].x2 = i;
          r[i].y1 = r[i-1].y2;
          r[i].y2 = h/2 + ((data[(j>>16)*2]*h) >> 16);
     }

     surface->Clear( surface, window->bg.color.r,
                              window->bg.color.g,
                              window->bg.color.b,
                              window->bg.color.a );
     surface->SetColor( surface, 0xff^window->bg.color.r,
                                 0xaf^window->bg.color.g,
                                 0x2f^window->bg.color.b,
                                 0xff );
     surface->DrawLines( surface, r, w );
     surface->Flip( surface, NULL, 0 );
     surface->Release( surface );
}

static FMBufferCallbackResult
buffer_callback( int len, void *ctx )
{
     void *data;

     if (buffer->Lock( buffer, &data, 0, 0 ) != DR_OK)
          return FMBCR_OK;

     /* draw scope */
     draw_scope( data, len );

     /* write samples to output stream */
     stream->Write( stream, data, len );

     buffer->Unlock( buffer );

     return FMBCR_OK;
}

static DirectResult
create_playback( const char *filename )
{
     FSBufferDescription b_desc;
     FSStreamDescription s_desc;
     DirectResult        err;

     err = FusionSoundCreate( &sound );
     if (err != DR_OK) {
          FusionSoundError( "FusionSoundCreate() failed", err );
          return err;
     }

     err = sound->CreateMusicProvider( sound, filename, &provider );
     if (err != DR_OK) {
          FusionSoundError( "CreateMusicProvider() failed", err );
          return err;
     }

     provider->GetBufferDescription( provider, &b_desc );
     provider->GetStreamDescription( provider, &s_desc );

     b_desc.flags        |= FSBDF_SAMPLEFORMAT | FSBDF_CHANNELS | FSBDF_LENGTH;
     b_desc.sampleformat  = FSSF_S16;
     b_desc.channels      = 2;
     b_desc.length        = 1024;

     s_desc.flags        |= FSSDF_SAMPLEFORMAT | FSSDF_CHANNELS;
     s_desc.sampleformat  = FSSF_S16;
     s_desc.channels      = 2;

     err = sound->CreateStream( sound, &s_desc, &stream );
     if (err != DR_OK) {
          FusionSoundError( "CreateStream() failed", err );
          return err;
     }

     err = sound->CreateBuffer( sound, &b_desc, &buffer );
     if (err != DR_OK) {
          FusionSoundError( "CreateBuffer() failed", err );
          return err;
     }

     err = provider->PlayToBuffer( provider, buffer,
                                   buffer_callback, NULL );
     if (err != DR_OK) {
          FusionSoundError( "PlayToBuffer() failed", err );
          return err;
     }

     return DR_OK;
}

static void
destroy_playback( void )
{
     if (provider)
          provider->Release( provider );
     if (buffer)
          buffer->Release( buffer );
     if (stream)
          stream->Release( stream );
     if (sound)
          sound->Release( sound );
}

int
main( int argc, char **argv )
{
     DirectResult err;
     DFBRectangle rect;

     err = DirectFBInit( &argc, &argv );
     if (err != DR_OK)
          DirectFBErrorFatal( "DirectFBInit() failed", err );

     if (argc != 2) {
          fprintf( stderr, "Usage: %s <filaname>\n", basename(argv[0]) );
          return 1;
     }

     err = FusionSoundInit( &argc, &argv );
     if (err != DR_OK)
          FusionSoundErrorFatal( "FusionSoundInit() failed", err );

     /* initialize LiTE */
     if (lite_open( &argc, &argv ))
          return 1;

     rect.x = LITE_CENTER_HORIZONTALLY;
     rect.y = LITE_CENTER_VERTICALLY;
     rect.w = 256;
     rect.h = 180;

     /* create a new window */
     lite_new_window( NULL,
                      &rect,
                      DWCAPS_ALPHACHANNEL,
                      liteDefaultWindowTheme,
                      basename(argv[1]), &window );

     /* show window */
     lite_set_window_opacity( window, 0xff );

     /* initialize FusionSound and load track */
     if (create_playback( argv[1] ) != DR_OK) {
          destroy_playback();
          lite_destroy_window( window );
          lite_close();
          return 1;
     }

     /* event loop */
     while (lite_window_event_loop( window, 20 ) == DR_TIMEOUT) {
          double pos;

          /* check if playback is finished */
          err = provider->GetPos( provider, &pos );
          if (err == DR_EOF)
               break;
     }

     /* deinitialize FusionSound */
     destroy_playback();

     /* destroy the window */
     lite_destroy_window( window );

     /* deinitialize LiTE */
     lite_close();

     return 0;
}

