
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>

#ifdef __XBOX__
#include <hal/xbox.h>
#include <openxdk/debug.h>
#endif

#include "types.h"
#include "hvl_replay.h"
#include "render.h"
#include "shapes.h"
#include "text.h"

#define FREQ 44100
#define HIVELY_LEN FREQ/50
#define OUTPUT_LEN 4096

#ifndef INITIAL_VOL
#define INITIAL_VOL 16
#endif

#ifdef __GP2X__
#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (15)
#define GP2X_BUTTON_Y               (14)
#define GP2X_BUTTON_L               (11)
#define GP2X_BUTTON_R               (10)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)
#endif

#ifndef SDL_DISABLE
#define SDL_DISABLE 0
#endif

extern int part;
extern Uint8 tempdata[];
#ifdef __NO_MOUSE__
extern int tbon, tbclos, tbpressed;
#endif

SDL_Surface *screen;
int ctune = 0;
int dragger=0, enddragger, dragx, dragy;
int wdragger=0;
int paused=0;

extern int32 volume;
extern int voltimer;

#ifdef __EMBED_TUNES__
#include "embedded_tunes/syphus-sweeties.h"
#include "embedded_tunes/Virgill-Forsaken.h"
#include "embedded_tunes/syphus-freeside_run.h"
#include "embedded_tunes/lb-peanuts.h"
#include "embedded_tunes/galactic_emeralds.h"
#include "embedded_tunes/joey_splif-pantsomime.h"
#include "embedded_tunes/testing123.h"
#include "embedded_tunes/m0d_-_incognito_crust.h"
#else
char *tunes[] = { "tunes/syphus-sweeties.hvl",
                  "tunes/Virgill-Forsaken.hvl",
                  "tunes/syphus-freeside_run.hvl",
                  "tunes/lb-peanuts.hvl",
                  "tunes/galactic_emeralds.hvl",
                  "tunes/joey_splif-pantsomime.hvl",
                  "tunes/testing123.hvl",
                  "tunes/m0d_-_incognito_crust.hvl" };
#endif

int16 hivelyLeft[HIVELY_LEN], hivelyRight[HIVELY_LEN];
size_t hivelyIndex=0;
struct hvl_tune *tune = NULL;

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif

#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif

#ifdef __NO_MOUSE__
SDL_Joystick *joy;
int joyx=0,joyy=0,joyb1=0,joyb2=0;
int ojx=0,ojy=0,ojb1=0,ojb2=0;
#endif

int sw=DEFAULT_WIDTH, sh=DEFAULT_HEIGHT;

int clicked, clickx, clicky;

void do_the_music( void *dummy, int8 *stream, int length )
{
  int16 *out;
  int i;
  size_t streamPos = 0;
  length = length >> 1;

  if(tune)
  {
    // Mix to 16bit interleaved stereo
    out = (int16*) stream;
    // Flush remains of previous frame
    for(i = hivelyIndex; i < (HIVELY_LEN) && streamPos < length; i++)
    {
      out[streamPos++] = hivelyLeft[i];
      out[streamPos++] = hivelyRight[i];
    }

    while(streamPos < length)
    {
      hvl_DecodeFrame( tune, (int8 *) hivelyLeft, (int8 *) hivelyRight, 2 );
      for(i = 0; i < (HIVELY_LEN) && streamPos < length; i++)
      {
        out[streamPos++] = hivelyLeft[i];
        out[streamPos++] = hivelyRight[i];
      }
    }
    hivelyIndex = i;
  }
}

#ifdef __EMBED_TUNES__
int start_tune( int n )
{
  SDL_PauseAudio( 1 );

  switch (n)
  {
    case 0:
      tune = hvl_reset( (uint8 *)syphus_sweeties_hvl, SYPHUS_SWEETIES_HVL_LEN, 4, FREQ, 0 );
      break;
    case 1:
      tune = hvl_reset( (uint8 *)Virgill_Forsaken_hvl, VIRGILL_FORSAKEN_HVL_LEN, 4, FREQ, 0 );
      break;
    case 2:
      tune = hvl_reset( (uint8 *)syphus_freeside_run_hvl, SYPHUS_FREESIDE_RUN_HVL_LEN, 4, FREQ, 0 );
      break;
    case 3:
      tune = hvl_reset( (uint8 *)lb_peanuts_hvl, LB_PEANUTS_HVL_LEN, 4, FREQ, 0 );
      break;
    case 4:
      tune = hvl_reset( (uint8 *)galactic_emeralds_hvl, GALACTIC_EMERALDS_HVL_LEN, 4, FREQ, 0 );
      break;
    case 5:
      tune = hvl_reset( (uint8 *)joey_splif_pantsomime_hvl, JOEY_SPLIF_PANTSOMIME_HVL_LEN, 4, FREQ, 0 );
      break;
    case 6:
      tune = hvl_reset( (uint8 *)testing123_hvl, TESTING123_HVL_LEN, 4, FREQ, 0 );
      break;
    case 7:
      tune = hvl_reset( (uint8 *)m0d_incognito_crust_hvl, M0D_INCOGNITO_CRUST_HVL_LEN, 4, FREQ, 0 );
      break;
    case 8:
      tune = hvl_reset( (uint8 *)tempdata, 1388, 4, FREQ, 0 );
      break;
  }

  hvl_InitSubsong( tune, 0 );
  SDL_PauseAudio( 0 );

  ctune = n;
  return 1;
}
#else
int start_tune( int n )
{
  SDL_PauseAudio( 1 );
  if( tune ) hvl_FreeTune( tune );
  tune = NULL;

  if( n == 8 )
  {
    tune = hvl_reset( (uint8 *)tempdata, 1388, 4, FREQ, 0 );
    if( !tune ) n = ctune;
  }

  if( !tune )
    tune = hvl_LoadTune( tunes[n], FREQ, 4 );

  if( !tune )
  {
    printf( "Couldn't load '%s'!!!11111one\n", tunes[n] );
    return 0;
  }
  hvl_InitSubsong( tune, 0 );
  SDL_PauseAudio( 0 );

  ctune = n;
  return 1;
}
#endif

void death( void )
{
#ifdef __NO_MOUSE__
  if( SDL_JoystickOpen( 0 ) )
    SDL_JoystickClose( joy );
#endif
  SDL_Quit();
  if( tune ) hvl_FreeTune( tune );
  text_shut();
  shapes_shut();
  render_shut();
}

int isnum( char c )
{
  if( ( c >= '0' ) && ( c <= '9' ) ) return 1;
  return 0;
}

#ifdef __NO_MOUSE__
void newjoystate( void )
{
  int cx, cy, cb1, cb2, i;
  
  // Find out what changed since the last event
  cx  = joyx != ojx;
  cy  = joyy != ojy;
  cb1 = joyb1 != ojb1;
  cb2 = joyb2 != ojb2;

  // Anything?
  if( (cx||cy||cb1||cb2) == 0 )
    return;
  
  // Remember current state
  ojx = joyx;
  ojy = joyy;
  ojb1 = joyb1;
  ojb2 = joyb2;
  
  if( part != PART_MAIN )
    return;
    
  // Ensure nothing gets "stuck"
  if( ( joyy == 0 ) &&
      ( tbpressed > 1 ) )
    user_interaction( ACTION_STOPSCROLL );
  
  // No buttons held?
  if( ( joyb1 == 0 ) && ( joyb2 == 0 ) )
  {
    // Y-Axis changed?
    if( cy )
    {
      switch( joyy )
      {
        case -1:
          user_interaction( ACTION_SCROLL_UP );
          break;
        
        case 1:
          user_interaction( ACTION_SCROLL_DOWN );
          break;
      }
    }
    return;
  }
  
  // Button just pressed?
  if( ( cb1 ) && ( joyb1 ) )
  {
    if( joyb2 )
    {
      // Both buttons
      if( tbon )
      {
        tbclos = 1;
        tbpressed = 0;
      }
      return;
    }

    // Button A triggered
    i=0;
    switch( joyy )
    {
      case -1:
        switch( joyx )
        {
          case -1:
            i = ACTION_TUNEPETAL_8;
            break;
          
          case 0:
            i = ACTION_TUNEPETAL_1;
            break;
          
          case 1:
            i = ACTION_TUNEPETAL_2;
            break;
        }
        break;

      case 0:
        switch( joyx )
        {
          case -1:
            i = ACTION_TUNEPETAL_7;
            break;
          
          case 0:
            i = ACTION_RESET_TUNEFLOWER;
            break;
          
          case 1:
            i = ACTION_TUNEPETAL_3;
            break;
        }
        break;

      case 1:
        switch( joyx )
        {
          case -1:
            i = ACTION_TUNEPETAL_6;
            break;
          
          case 0:
            i = ACTION_TUNEPETAL_5;
            break;
          
          case 1:
            i = ACTION_TUNEPETAL_4;
            break;
        }
        break;
    }    
    user_interaction( i );
  }    
  
  if( ( cb2 ) && ( joyb2 ) )
  {
    if( joyb1 )
    {
      // Both buttons
      if( tbon )
      {
        tbclos = 1;
        tbpressed = 0;
      }
      return;
    }
  
    // Button B triggered
    i=0;
    switch( joyy )
    {
      case -1:
        switch( joyx )
        {
          case -1:
            i = ACTION_TEXTPETAL_8;
            break;
          
          case 0:
            i = ACTION_TEXTPETAL_1;
            break;
          
          case 1:
            i = ACTION_TEXTPETAL_2;
            break;
        }
        break;

      case 0:
        switch( joyx )
        {
          case -1:
            i = ACTION_TEXTPETAL_7;
            break;
          
          case 0:
            i = ACTION_RESET_TEXTFLOWER;
            break;
          
          case 1:
            i = ACTION_TEXTPETAL_3;
            break;
        }
        break;

      case 1:
        switch( joyx )
        {
          case -1:
            i = ACTION_TEXTPETAL_6;
            break;
          
          case 0:
            i = ACTION_TEXTPETAL_5;
            break;
          
          case 1:
            i = ACTION_TEXTPETAL_4;
            break;
        }
        break;
    }    

    user_interaction( i );
  }
}
#endif

#ifdef __XBOX__
void XBoxStartup()
#else
#ifdef __NO_ARGS__
int main( void )
#else
int main( int argc, char *argv[] )
#endif
#endif
{
  int i, flags;  
  int done, needrender;
  SDL_AudioSpec wanted;
  void *timer;

#ifndef __FIXED_RES__
  int j, k;
#endif

  volume = INITIAL_VOL;

#ifdef __XBOX__
  srand( 1 );
#else
  srand( time( 0 ) );
#endif

#ifdef __NO_MOUSE__
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK ) < 0 )
#else
  if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER ) < 0 )
#endif
  {
#ifdef __XBOX__
    debugPrint("SDL says: %s\n", SDL_GetError() );
    XSleep(5000);
#else
    printf( "SDL says: %s\n", SDL_GetError() );
#endif
    return 0;
  }
  atexit( death );

#ifdef __NO_MOUSE__
  if( SDL_NumJoysticks() == 0 )
  {
#ifdef __XBOX__
    debugPrint("No mouse, no joystick\n");
    XSleep(5000);
#else
    printf( "No mouse, no joystick, no luck... :-(\n" );
#endif
    return 0;
  }
  
  joy = SDL_JoystickOpen( 0 );
  SDL_JoystickEventState( SDL_ENABLE );
#endif
  
#ifdef __SW_SURFACE__
  flags = SDL_SWSURFACE|SDL_FULLSCREEN;
#else
  #ifdef __HW_ONLY_WINDOWED__
    flags = SDL_SWSURFACE|SDL_FULLSCREEN;
  #else
    flags = SDL_HWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF;
  #endif
#endif

#ifndef __NO_ARGS__
  for( i=1; i<argc; i++ )
  {
    if( argv[i][0] == '-' )
    {
      switch( argv[i][1] )
      {
#ifndef __FULLSCREEN_ONLY__
        case 'w':
          flags &= ~SDL_FULLSCREEN;
#ifdef __HW_ONLY_WINDOWED__
          flags &= ~SDL_SWSURFACE;
          flags |= SDL_HWSURFACE;
#endif // __HW_ONLY_WINDOWED__
          break;
#endif // __FULLSCREEN_ONLY__
        
#ifndef __FIXED_RES__
        case 'r':
          k = 2;
          if( isnum( argv[i][k] ) )
          {
            j = atoi( &argv[i][k] );
            while( isnum( argv[i][k] ) ) k++;
            if( argv[i][k] != 'x' )
            {
              printf( "You need to specify resolution like -r640x480 or so\n" );
              break;
            }
            k++;
            sw = j;
            sh = atoi( &argv[i][k] );
            if( sw < 256 ) sw = 256;
            if( sh < 192 ) sh = 192;
          } else {
            printf( "You need to specify resolution like -r640x480 or so\n" );
            break;
          }
          break;
#endif
      }
    }
  }
#endif

  screen = SDL_SetVideoMode( sw, sh, BITSPERPIXEL, flags );
  if( !screen )
  {
#ifdef __XBOX__
    debugPrint( "SDL says: %s\n", SDL_GetError() );
    XSleep(5000);
#else
    printf( "SDL says: %s\n", SDL_GetError() );
#endif
    return 0;
  }
  
  SDL_WM_SetCaption( "Planet Hively", 0 );
  SDL_ShowCursor( SDL_DISABLE );
  
  if( !render_init() ) return 0;
  if( !shapes_init() ) return 0;
  if( !text_init() ) return 0;

  wanted.freq = FREQ; 
  wanted.format = AUDIO_S16SYS; 
  wanted.channels = 2; /* 1 = mono, 2 = stereo */
  wanted.samples = OUTPUT_LEN; // HIVELY_LEN;

  wanted.callback = (void*)do_the_music;
  wanted.userdata = 0;

  if( SDL_OpenAudio( &wanted, NULL ) < 0 )
  {
#ifdef __XBOX__
    debugPrint("SDL says: %s\n", SDL_GetError() );
    XSleep(5000);
#else
    printf( "SDL says: %s\n", SDL_GetError() );
#endif
    return 0;
  }
  
#ifndef __BROKEN_SDL_TIMER__
  timer = SDL_AddTimer(FPSTIME, (SDL_NewTimerCallback)timing, 0 );
#endif

  hvl_InitReplayer();

  if( start_tune( 0 ) == 0 )
    return 0;
    
  done = 0;
  needrender = 0;
  while( !done )
  {
    SDL_Event event;
    
#ifdef __BROKEN_SDL_TIMER__
    while (SDL_PollEvent( NULL ) == 0)
    {
      timing(FPSTIME, NULL);
    }
#endif

    SDL_WaitEvent( &event );

    do {
      switch( event.type )
      {

#ifdef __NO_MOUSE__
  #ifdef __GP2X__
  
/**** GP2X joystick reports button events even for D-Pad ****/
  
        case SDL_JOYBUTTONDOWN:
          switch( event.jbutton.button )
          {
            case GP2X_BUTTON_UP:
              joyy = -1;
              break;
            
            case GP2X_BUTTON_DOWN:
              joyy = 1;
              break;
            
            case GP2X_BUTTON_LEFT:
              joyx = -1;
              break;
            
            case GP2X_BUTTON_RIGHT:
              joyx = 1;
              break;
            
            case GP2X_BUTTON_UPLEFT:
              joyx = -1;
              joyy = -1;
              break;
            
            case GP2X_BUTTON_UPRIGHT:
              joyx = 1;
              joyy = -1;
              break;
            
            case GP2X_BUTTON_DOWNLEFT:
              joyx = -1;
              joyy = 1;
              break;
            
            case GP2X_BUTTON_DOWNRIGHT:
              joyx = 1;
              joyy = 1;
              break;
            
            case GP2X_BUTTON_A:
              joyb1 = 1;
              break;
            
            case GP2X_BUTTON_B:
              joyb2 = 1;
              break;
          }
          newjoystate();
          break;
        
        case SDL_JOYBUTTONUP:
          switch( event.jbutton.button )
          {
            case GP2X_BUTTON_UP:
              if( joyy == -1 ) joyy = 0;
              break;

            case GP2X_BUTTON_DOWN:
              if( joyy == 1 ) joyy = 0;
              break;
            
            case GP2X_BUTTON_LEFT:
              if( joyx == -1 ) joyx = 0;
              break;

            case GP2X_BUTTON_RIGHT:
              if( joyx == 1 ) joyx = 0;
              break;
            
            case GP2X_BUTTON_UPLEFT:
              if( joyy == -1 ) joyy = 0;
              if( joyx == -1 ) joyx = 0;
              break;
            
            case GP2X_BUTTON_UPRIGHT:
              if( joyy == -1 ) joyy = 0;
              if( joyx == 1 ) joyx = 0;
              break;
            
            case GP2X_BUTTON_DOWNLEFT:
              if( joyy == 1 ) joyy = 0;
              if( joyx == -1 ) joyx = 0;
              break;
            
            case GP2X_BUTTON_DOWNRIGHT:
              if( joyy == 1 ) joyy = 0;
              if( joyx == 1 ) joyx = 0;
              break;
            
            case GP2X_BUTTON_A:
              joyb1 = 0;
              break;
            
            case GP2X_BUTTON_B:
              joyb2 = 0;
              break;

            case GP2X_BUTTON_VOLUP:
              if( volume < 16 ) volume++;
              voltimer = TARGET_FPS*2;
              break;
            
            case GP2X_BUTTON_VOLDOWN:
              if( volume > 0 ) volume--;
              voltimer = TARGET_FPS*2;
              break;
          }
          
          newjoystate();
          break;

  #else // SPECIAL JOYSTICK TYPES

/**** Normal SDL joystick handling ****/

        case SDL_JOYAXISMOTION:
          switch( event.jaxis.axis )
          {
            case 0:
              if( event.jaxis.value < -3200 ) joyx = -1;
              else if( event.jaxis.value > 3200 ) joyx = 1;
              else joyx = 0;
              newjoystate();
              break;
            
            case 1:
              if( event.jaxis.value < -3200 ) joyy = -1;
              else if( event.jaxis.value > 3200 ) joyy = 1;
              else joyy = 0;
              newjoystate();
              break;
          }
          break;
        
        case SDL_JOYBUTTONDOWN:
          switch( event.jbutton.button )
          {
            case 0:
              joyb1 = 1;
              break;
            
            case 1:
              joyb2 = 1;
              break;
          }
          newjoystate();
          break;
        
        case SDL_JOYBUTTONUP:
          switch( event.jbutton.button )
          {
            case 0:
              joyb1 = 0;
              break;
            
            case 1:
              joyb2 = 0;
              break;
          }
          newjoystate();
          break;

  #endif // SPECIAL JOYSTICK TYPES
#endif

        case SDL_MOUSEMOTION:
          dragx = event.motion.x;
          dragy = event.motion.y;
          break;
            
        case SDL_MOUSEBUTTONUP:
          if( event.button.button == SDL_BUTTON_LEFT )
            enddragger = 1;
          break;

        case SDL_MOUSEBUTTONDOWN:
          if( event.button.button == SDL_BUTTON_LEFT )
          {
            clickx = event.button.x;
            clicky = event.button.y;
            if(( clickx >= 0 ) && ( clickx < sw ) && ( clicky >= 0 ) && ( clicky < sh ))
              clicked = 1;
          }
          enddragger = 0;
          break;
        
        case SDL_KEYDOWN:
          switch( event.key.keysym.sym )
          {
#ifndef __FULLSCREEN_ONLY__
            case 'f':
            case 'F':
            case SDLK_RETURN:	// was going to use alt-return, but since return isn't doing anything else..
              SDL_WM_ToggleFullScreen( screen );
              break;
#endif

            case 'p':
            case 'P':
              paused ^= 1;
              SDL_PauseAudio( paused );
              break;
            
            case SDLK_UP:
              if( volume < 16 ) volume++;
              voltimer = TARGET_FPS*2;
              break;
            
            case SDLK_DOWN:
              if( volume > 0 ) volume--;
              voltimer = TARGET_FPS*2;
              break;

            case SDLK_ESCAPE:
//              if( part == PART_MAIN )
//                fade_in( "click the sun to exit!", 1 );
//              else
              done = 1;
              break;
            
            default:
              break;
          }
          break;
        
        case SDL_QUIT:
//          if( part == PART_MAIN )
//            fade_in( "click the sun to exit!", 1 );
//          else
          done = 1;
          break;
        
        case SDL_USEREVENT:
          needrender = 1;
          break;
      }
    } while( SDL_PollEvent( &event ) );
    
    if( needrender )
    {
      needrender = 0;
      done |= render();
    }
  }
  
  SDL_PauseAudio( 1 );
  return 0;
}
