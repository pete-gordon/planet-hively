
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "types.h"
#include "shapes.h"

#define FONTW_SMALL 367
#define FONTW_BIG 714

extern int sw, sh, rs, se, midx;
extern PIXEL skytxtpal[], logopal[];
extern PIXEL pal[];
extern Uint8 introfont[];
extern Uint8 sboxtop[], sboxbot[], sboxclos[], sboxclosp[];
extern Uint8 sboxupp[], sboxdownp[];
extern SDL_Surface *screen;
extern Uint8 *layers, *click;

extern int dragx, dragy;

char *fntab = "abcdefghijklmnopqrstuvwxyz0123456789!=.,:;()\"/_-#%*'?@";

int foffsb[] = {   0,  16,  30,  47,  64,  77,  89, 107,
                 124, 130, 145, 160, 172, 192, 208, 227,
                 240, 259, 272, 285, 299, 313, 328, 350,
                 364, 378, 392, 410, 419, 432, 445, 460,
                 473, 487, 499, 512, 527, 534, 542, 549,
                 556, 563, 571, 581, 593, 602, 610, 623,
                 631, 649, 667, 677, 682, 695, 714 };

int foffss[] = {   0,   8,  15,  24,  32,  38,  44,  52,
                  60,  64,  71,  79,  85,  95, 103, 112,
                 119, 128, 135, 142, 149, 157, 166, 176,
                 183, 190, 197, 206, 210, 217, 223, 231,
                 237, 244, 250, 257, 264, 268, 272, 275,
                 278, 281, 285, 290, 296, 302, 309, 315,
                 319, 330, 342, 347, 350, 356, 367 };

extern Uint8 fontsmall[], fontbig[];

static Uint8 *fdat;
static int *foffs;
static int fm;
int spcw;
int fontheight;

static char *fadetext = NULL;
static char *nextfadetext = NULL;
static int fadepos, fadeend, fadex, nextfadex;

PIXEL *bsbuf = NULL;
static int ek;

int tbw, tbh, tbx, tby, tbyo, tbscx, tbscy, tbbrd, tbstop=0;
int tbsch, tbpressed=0;

int tbaw=0, tbah=0, tbauh=0, tbvis=0;
PIXEL *tbab=NULL;

int text_init( void )
{
  int i;
  if( sh < 400 )
  {
    fdat   = &fontsmall[0];
    foffs  = &foffss[0];
    fm     = FONTW_SMALL;
    spcw   = 4;
    ek     = 0;
    fontheight = 12;
    if( sw >= 320 )
    {
      tbaw   = 158;
      tbw    = 180;
      tbh    = 96;
    } else {
      tbaw   = 142;
      tbw    = 166;
      tbh    = 88;
    }
    tbbrd  = 4;
  } else {
    fdat  = &fontbig[0];
    foffs = &foffsb[0];
    fm    = FONTW_BIG;
    spcw  = 8;
    ek    = 0;
    fontheight = 24;
    tbaw  = 316;
    tbw   = 360;
    tbh   = 192;
    tbbrd = 8;
  }

  bsbuf = malloc( rs * fontheight * BYTESPERPIXEL );
  if( !bsbuf )
  {
    printf( "Out of memory\n" );
    return 0;
  }
  
  for( i=0; i<rs*fontheight; i++ )
    bsbuf[i] = pal[4];
  
  return 1;
}

void text_shut( void )
{
  if( bsbuf ) free( bsbuf );
}

int charwidth( char c )
{
  int ch;

  for( ch=0; fntab[ch]; ch++ )
    if( fntab[ch] == c ) break;
    
  if( !fntab[ch] )
    return spcw;

  return (foffs[ch+1]-foffs[ch])-ek;
}

int textlen( char *str )
{
  int i, ch;
  int len;
  
  i=0; len=0;
  while( str[i] )
  {
    for( ch=0; fntab[ch]; ch++ )
      if( fntab[ch] == str[i] ) break;
    
    if( !fntab[ch] )
    {
      len += spcw;
      i++;
      continue;
    }

    len += (foffs[ch+1]-foffs[ch])-ek;
    i++;
  }
  
  return len;
}

void textprint( int x, int y, char *str, PIXEL *wpal )
{
  int i, ch;
  int fxs, xc, yc, xs, xe, fx;
  PIXEL *p;
  Uint8  *fp;
  
  i=0;
  while( str[i] )
  {
    if( x > sw ) break;
    for( ch=0; fntab[ch]; ch++ )
      if( fntab[ch] == str[i] ) break;
    
    if( !fntab[ch] )
    {
      xs = x;
      xe = x+spcw;
      
      if( xs < 0 ) xs = 0;
      if( xe > sw ) xe = sw;

      p = &((PIXEL *)screen->pixels)[y*rs];
      for( yc=0; yc<fontheight; yc++ )
      {
        for( xc=xs; xc<xe; xc++ )
          p[xc] = wpal[4];
        p += rs;
      }
      
      x += xe-x;

      i++;
      continue;
    }
    
    fxs = foffs[ch];
    
    xs = x;
    xe = x+(foffs[ch+1]-fxs);
    
    if( xs < 0 )  xs = 0;
    if( xe > sw ) xe = sw;
    
    p = &((PIXEL *)screen->pixels)[y*rs];
    fp = &fdat[fxs];
    for( yc=0; yc<fontheight; yc++ )
    {
      for( xc=xs, fx=0; xc<xe; xc++, fx++ )
        p[xc] = wpal[fp[fx]];
      p += rs;
      fp += fm;
    }
    
    x += (xe-x)-ek;
    
    i++;
  }
}

void textprintfade( int x, int y, int fade, char *str, PIXEL *wpal )
{
  int i, ch;
  int fxs, xc, yc, xs, xe, fx;
  PIXEL *p;
  Uint8  *fp;
  int col, rcol, fran, fmin;
  
  fmin = fade-16;
  if( fmin < 0 ) fmin = 0;
  fran = fade-fmin;
  
  i=0;
  while( str[i] )
  {
    if( x > sw ) break;
    for( ch=0; fntab[ch]; ch++ )
      if( fntab[ch] == str[i] ) break;
    
    if( !fntab[ch] )
    {
      xs = x;
      xe = x+spcw;

      p = &((PIXEL *)screen->pixels)[y*rs];
      for( yc=0; yc<fontheight; yc++ )
      {
        for( xc=xs; xc<xe; xc++ )
          p[xc] = wpal[4];
        p += rs;
      }
      
      x += xe-x;

      i++;
      continue;
    }
    
    fxs = foffs[ch];
    
    xs = x;
    xe = x+(foffs[ch+1]-fxs);
    
    if( xs < 0 )  xs = 0;
    if( xe > sw ) xe = sw;
    
    p = &((PIXEL *)screen->pixels)[y*rs];
    fp = &fdat[fxs];
    for( yc=0; yc<fontheight; yc++ )
    {
      for( xc=xs, fx=0; xc<xe; xc++, fx++ )
      {
        col = fp[fx]-4;
        rcol = fran==0?fmin:(rand()%fran)+fmin;
        p[xc] = wpal[rcol>col?col+4:rcol+4];
      }
      p += rs;
      fp += fm;
    }
    
    x += (xe-x)-ek;
    
    i++;
  }
}

void fade_in( char *str, int immediate )
{
  if( ( immediate ) || ( fadetext == NULL ) )
  {
    fadetext = str;
    fadex = (sw-textlen( str ))/2;
    nextfadetext = NULL;
    fadepos  = 0;
    fadeend = strlen( str )+90;
    return;
  }
  
  nextfadetext = str;
  nextfadex = (sw-textlen( str ))/2;
}

int do_fade( int fadeadd )
{
  int fd;

  if( !fadetext )
  {
    if( nextfadetext )
    {
      fadetext = nextfadetext;
      fadex    = nextfadex;
      
      nextfadetext = NULL;
      fadepos = 0;
      fadeend = strlen( fadetext )+90;
    } else {
      fadepos = 0;
      return 1;
    }
  }

  fadepos += fadeadd;
  
  if( fadepos < 32 )
  {
    textprintfade( fadex, 0, fadepos, fadetext, skytxtpal );
    return 0;
  }
  
  if( fadepos < fadeend )
  {
    textprint( fadex, 0, fadetext, skytxtpal );
    return 0;
  }
  
  fd = 31-(fadepos-fadeend);
  if( fd <= 0 )
  {
    if( nextfadetext )
    {
      fadetext = nextfadetext;
      fadex    = nextfadex;
      
      nextfadetext = NULL;
      fadepos = 0;
      fadeend = strlen( fadetext )+90;
      return 0;
    }
  
    fadetext = NULL;
    fadepos = 0;
    return 1;
  }
  
  textprintfade( fadex, 0, fd, fadetext, skytxtpal );
  return 0;
}

void textboxprint( int x, int y, char *str )
{
  int i, ch;
  int fxs, xc, yc, xs, xe, fx;
  PIXEL *p;
  Uint8  *fp;
  
  if( !tbab ) return;
  
  i=0;
  while( str[i] )
  {
    if( x > tbaw ) break;
    for( ch=0; fntab[ch]; ch++ )
      if( fntab[ch] == str[i] ) break;
    
    if( !fntab[ch] )
    {
      xs = x;
      xe = x+spcw;
      
      if( xs < 0 ) xs = 0;
      if( xe > tbaw ) xe = tbaw;

      p = &tbab[y*tbaw];
      for( yc=0; yc<fontheight; yc++ )
      {
        for( xc=xs; xc<xe; xc++ )
          p[xc] = pal[4];
        p += tbaw;
      }
      
      x += xe-x;

      i++;
      continue;
    }
    
    fxs = foffs[ch];
    
    xs = x;
    xe = x+(foffs[ch+1]-fxs);
    
    if( xs < 0 )  xs = 0;
    if( xe > tbaw ) xe = tbaw;
    
    p = &tbab[y*tbaw];
    fp = &fdat[fxs];
    for( yc=0; yc<fontheight; yc++ )
    {
      for( xc=xs, fx=0; xc<xe; xc++, fx++ )
        p[xc] = pal[fp[fx]];
      p += tbaw;
      fp += fm;
    }
    
    x += (xe-x)-ek;
    
    i++;
  }
}

void textbox_layout( char *str )
{
  int x, y, i, j, k, l, m, lw, w;
  int just;
  char line[256];
  
  tbauh = 0;
  tbyo = 0;
  
  if( !str )
  {
    tbauh = 0;
    return;
  }

  // Initial box is big enough for 16 lines
  if( tbab == NULL )
  {
    tbah = fontheight * 16;
    tbab = malloc( tbaw * tbah * BYTESPERPIXEL );
    if( !tbab )
    {
      fade_in( "out of memory !!!!", 1 );
      return;
    }
    
    for( i=0; i<tbaw*tbah; i++ )
      tbab[i] = pal[4];
  }
  
  just = 0;
  x=0; y=0; i=0;
  while( str[i] )
  {
    if( y >= tbah )
    {
      int nh;
      PIXEL *nb;
      
      nh = tbah + (fontheight*8);
      nb = malloc( tbaw * nh * BYTESPERPIXEL );
      if( !nb )
      {
        free( tbab );
        tbab = NULL;
        fade_in( "out of memory !!!!", 1 );
        return;
      }

      for( w=tbaw*tbah; w<tbaw*nh; w++ )
        nb[w] = pal[4];
      
      memcpy( nb, tbab, tbaw*tbah*BYTESPERPIXEL );
      free( tbab );
      tbab = nb;
      tbah = nh;
    }
  
    for( w=0; w<tbaw*fontheight; w++ )
      tbab[w+(y*tbaw)] = pal[4];

    w=0;
    j=0;
    l=0;
    for( ;; )
    {
      // Skip initial whitespace
      while( str[i] == 32 )
      {
        if( str[i] == '{' ) just = 0;
        if( str[i] == '}' ) just = 1;
        if( str[i] == '|' ) just = 2;
        i++;
      }
    
      lw = w;
      k = j;
      m = i;
      while( ( str[i] != 32 ) && ( str[i] != 0 ) && ( str[i] != '\n' ) )
      {
        if( str[i] == '{' ) { just = 0; i++; continue; }
        if( str[i] == '}' ) { just = 1; i++; continue; }
        if( str[i] == '|' ) { just = 2; i++; continue; }

        l = charwidth( str[i] );
        w += l;
        if( w > tbaw ) break;
        line[j++] = str[i++];
      }
      
      if( w > tbaw )
      {
        if( k != 0 )
        {
          w = lw;
          j = k;
          i = m;
        } else {
          w -= l;
        }
        break;
      }
      
      if( str[i] == 0 )
        break;
      
      if( str[i] == '\n' )
      {
        i++;
        break;
      }
      
      line[j++] = 32;
      w += spcw;
    }

    line[j] = 0;

    switch( just )
    {
      case 0:
        x = 0;
        break;
      
      case 1:
        x = tbaw - w;
        break;
      
      case 2:
        x = (tbaw-w) / 2;
        break;
    }
    
    textboxprint( x, y, line );
    y += fontheight;
  }
  
  tbauh = y;
}

void textbox_drag( int y )
{
  if( ( tbauh <= tbvis ) ||
      ( tbvis == 0 ) )
  {
    tbyo = 0;
    return;
  }
  
  y -= tbstop;
  
  if( y < 0 ) y = 0;
  if( y >= tbsch ) y = tbsch-1;

  tbyo = (y * ((tbauh+24)-tbvis)) / tbsch;
}

void render_scrollbox( float scale, Uint8 lyr, Uint8 clk, Uint8 slyr, Uint8 sclk, Uint8 dclk )
{
  int sbhs, sbm, sbms;
  int x, y, o, c, md, omd, ttop, ch;
  PIXEL *s, *d, *p3d;
  Uint8  *ld, *cd, *ps;
  
  if( tbw <= 180 )
  {
    sbhs = (int)(96.0f*scale);
    sbm  = tby+48;
    sbms = sbhs/2;
    if( sbms == 0 ) return;
    
    ttop = sbm-sbms;
    
    md = rs-tbw;
    omd = sw-tbw;

    d = &((PIXEL *)screen->pixels)[ttop*rs+tbx];
    ld = &layers[ttop*sw+tbx];
    cd = &click[ttop*sw+tbx];
    
    o=0;
    for( y=0; y<17; y++ )
    {
      if( y >= sbms ) break;
      for( x=0; x<180; x++, o+=2 )
      {
        if( ( tbw < 180 ) && ( x == 80 ) )
        {
          x += 180-tbw;
          o += (180-tbw)*2;
        }

        if( (c=sboxtop[o]) != 0x14 )
        {
          *(d++)  = pal[c];
          *(ld++) = lyr;
          *(cd++) = dclk;
        } else {
          d++;
          ld++;
          cd++;
        }
      }
      o += 360;
      d += md;
      ld += omd;
      cd += omd;
    }
    
    if( sbhs > 34 )
    {
      int h = sbhs-34;
      PIXEL pl = pal[0x04];
      for( y=0; y<h; y++ )
      {
        for( x=0; x<tbw; x++ )
        {
          *(d++)  = pl;
          *(ld++) = lyr;
          *(cd++) = dclk;
        }
        d += md;
        ld += omd;
        cd += omd;
      }
    }

    if( sbms > 17 )
    {
      y = 0;
      o = 360;
      d = &((PIXEL *)screen->pixels)[(sbm+sbms-17)*rs+tbx];
      ld = &layers[(sbm+sbms-17)*sw+tbx];
      cd = &click[(sbm+sbms-17)*sw+tbx];
    } else {
      y = 17-sbms;
      o = y*720+360;
      d = &((PIXEL *)screen->pixels)[sbm*rs+tbx];
      ld = &layers[sbm*sw+tbx];
      cd = &click[sbm*sw+tbx];
    }  
    
    p3d = d;

    for( ; y<17; y++ )
    {
      for( x=0; x<180; x++, o+=2 )
      {
        if( ( tbw < 180 ) && ( x == 80 ) )
        {
          x += 180-tbw;
          o += (180-tbw)*2;
        }

        if( (c=sboxbot[o]) != 0x14 )
        {
          *(d++)  = pal[c];
          *(ld++) = lyr;
          *(cd++) = dclk;
        } else {
          d++;
          ld++;
          cd++;
        }
      }
      o += 360;
      d += md;
      ld += omd;
      cd += omd;
    }
    
    tbvis = sbhs-8;

    if( ( tbauh != 0 ) && ( tbvis > 0 ) && ( tbab != NULL ) )
    {
      int tbbot;

      s = &tbab[tbyo*tbaw];
      d = &((PIXEL *)screen->pixels)[(ttop+4)*rs+tbx+4];
     
      tbbot = tbyo+tbvis;
      for( y=tbyo; y<tbbot; y++ )
      {
        if( y >= tbauh ) break;
        memcpy( d, s, tbaw * BYTESPERPIXEL );
        d += rs;
        s += tbaw;
      }
    }
    
    tbsch = sbhs - 42;
    tbstop = ttop+21;
    if( tbsch > 0 )
    {
      tbscx = tbx+tbw-11;
      
      // Render scroll bar
      d = &((PIXEL *)screen->pixels)[(ttop+21)*rs+tbscx];
      for( y=0; y<tbsch; y++ )
      {
        *d = pal[0xb];
        d += rs;
      }
      
      if( tbauh > tbvis )
        tbscy = (tbyo * tbsch) / ((tbauh+24)-tbvis) + ttop+21;
      else
        tbscy = ttop+21;

      circle( tbscx, tbscy, 4, pal[0xb], lyr, sclk );
    }

    ch = sbhs-4;
    if( ch < 1 ) return;
    if( ch > 12 ) ch = 12;

    md = rs-12;
    omd = sw-12;

    if( ( ( dragx >= tbx ) && ( dragx < (tbx+tbw) ) &&
          ( dragy >= tby ) && ( dragy < (tby+tbh) ) ) ||
        ( tbpressed == 1 ) )
    {
      ps = sboxclos;
      if( tbpressed == 1 ) ps = sboxclosp;

      d = &((PIXEL *)screen->pixels)[(ttop+4)*rs+tbx+4];
      ld = &layers[(ttop+4)*sw+tbx+4];
      cd = &click[(ttop+4)*sw+tbx+4];

      for( y=0; y<ch; y++ )
      {
        for( x=0; x<12; x++ )
        {
          *(d++)  = pal[*ps];
          ps += 2;
          *(ld++) = lyr;
          *(cd++) = clk;
        }
        ps += 24;
        d += md;
        ld += omd;
        cd += omd;
      }
    }
    
    if( sbhs < 35 ) return;
    
    switch( tbpressed )
    {
      case 2:
        tbyo -= (fontheight/4);
        if( tbyo < 0 ) tbyo = 0;

        ps = sboxupp;
        d = &((PIXEL *)screen->pixels)[(ttop+5)*rs+tbx+tbw-18];
        break;
      
      case 3:
        tbyo += (fontheight/4);
        if( tbyo > ((tbauh+24)-tbvis) ) tbyo = ((tbauh+24)-tbvis);

        ps = sboxdownp;
        d = &p3d[tbw-18];
        break;
      
      default:
        return;
    }
    
    md = rs-14;
    for( y=0; y<13; y++ )
    {
      for( x=0; x<14; x++ )
      {
        *(d++) = pal[*ps];
        ps += 2;
      }
      ps += 28;
      d += md;
    }
    return;
  }
  
  sbhs = (int)(192.0f*scale);
  sbm  = tby+96;
  sbms = sbhs/2;
  if( sbms == 0 ) return;
  
  ttop = sbm-sbms;
  
  md = rs-360;
  omd = sw-360;

  d = &((PIXEL *)screen->pixels)[ttop*rs+tbx];
  ld = &layers[ttop*sw+tbx];
  cd = &click[ttop*sw+tbx];
  
  o=0;
  for( y=0; y<35; y++ )
  {
    if( y >= sbms ) break;
    for( x=0; x<360; x++ )
    {
      if( (c=sboxtop[o++]) != 0x14 )
      {
        *(d++)  = pal[c];
        *(ld++) = lyr;
        *(cd++) = dclk;
      } else {
        d++;
        ld++;
        cd++;
      }
    }
    d += md;
    ld += omd;
    cd += omd;
  }
  
  if( sbhs > 70 )
  {
    int h = sbhs-70;
    PIXEL pl = pal[0x04];
    for( y=0; y<h; y++ )
    {
      for( x=0; x<360; x++ )
      {
        *(d++)  = pl;
        *(ld++) = lyr;
        *(cd++) = dclk;
      }
      d += md;
      ld += omd;
      cd += omd;
    }
  }

  if( sbms > 35 )
  {
    y = 0;
    o = 0;
    d = &((PIXEL *)screen->pixels)[(sbm+sbms-35)*rs+tbx];
    ld = &layers[(sbm+sbms-35)*sw+tbx];
    cd = &click[(sbm+sbms-35)*sw+tbx];
  } else {
    y = 35-sbms;
    o = y*360;
    d = &((PIXEL *)screen->pixels)[sbm*rs+tbx];
    ld = &layers[sbm*sw+tbx];
    cd = &click[sbm*sw+tbx];
  }  
  
  p3d = d;

  for( ; y<35; y++ )
  {
    for( x=0; x<360; x++ )
    {
      if( (c=sboxbot[o++]) != 0x14 )
      {
        *(d++)  = pal[c];
        *(ld++) = lyr;
        *(cd++) = dclk;
      } else {
        d++;
        ld++;
        cd++;
      }
    }
    d += md;
    ld += omd;
    cd += omd;
  }
  
  tbvis = sbhs-16;

  if( ( tbauh != 0 ) && ( tbvis > 0 ) && ( tbab != NULL ) )
  {
    int tbbot;

    s = &tbab[tbyo*tbaw];
    d = &((PIXEL *)screen->pixels)[(ttop+8)*rs+tbx+8];
   
    tbbot = tbyo+tbvis;
    for( y=tbyo; y<tbbot; y++ )
    {
      if( y >= tbauh ) break;
      memcpy( d, s, tbaw * BYTESPERPIXEL );
      d += rs;
      s += tbaw;
    }
  }
  
  tbsch = sbhs - 84;
  tbstop = ttop+42;
  if( tbsch > 0 )
  {
    tbscx = tbx+338;
    
    // Render scroll bar
    d = &((PIXEL *)screen->pixels)[(ttop+42)*rs+tbscx];
    for( y=0; y<tbsch; y++ )
    {
      *d = pal[0xb];
      d += rs;
    }
    
    if( tbauh > tbvis )
      tbscy = (tbyo * tbsch) / ((tbauh+24)-tbvis) + ttop+42;
    else
      tbscy = ttop+42;

    circle( tbscx, tbscy, 6, pal[0xb], lyr, sclk );
  }

  ch = sbhs-8;
  if( ch < 1 ) return;
  if( ch > 24 ) ch = 24;

  md = rs-24;
  omd = sw-24;

  if( ( ( dragx >= tbx ) && ( dragx < (tbx+tbw) ) &&
        ( dragy >= tby ) && ( dragy < (tby+tbh) ) ) ||
      ( tbpressed == 1 ) )
  {
    ps = sboxclos;
    if( tbpressed == 1 ) ps = sboxclosp;

    d = &((PIXEL *)screen->pixels)[(ttop+8)*rs+tbx+8];
    ld = &layers[(ttop+8)*sw+tbx+8];
    cd = &click[(ttop+8)*sw+tbx+8];

    for( y=0; y<ch; y++ )
    {
      for( x=0; x<24; x++ )
      {
        *(d++)  = pal[*(ps++)];
        *(ld++) = lyr;
        *(cd++) = clk;
      }
      d += md;
      ld += omd;
      cd += omd;
    }
  }
  
  if( sbhs < 70 ) return;
  
  switch( tbpressed )
  {
    case 2:
      tbyo -= (fontheight/4);
      if( tbyo < 0 ) tbyo = 0;

      ps = sboxupp;
      d = &((PIXEL *)screen->pixels)[(ttop+10)*rs+tbx+325];
      break;
    
    case 3:
      tbyo += (fontheight/4);
      if( tbyo > ((tbauh+24)-tbvis) ) tbyo = ((tbauh+24)-tbvis);

      ps = sboxdownp;
      d = &p3d[325];
      break;
    
    default:
      return;
  }
  
  md = rs-28;
  for( y=0; y<26; y++ )
  {
    for( x=0; x<28; x++ )
      *(d++) = pal[*(ps++)];
    d += md;
  }
}

void logoletter( int x, int y, int frame, float zoom, int l )
{
  int off;
  float sx, sy, sxd, syd;
  int size, yo;
  Uint8 *s;
  PIXEL *d;

  /* *sigh* thanks spot */
  if( l < 2 )
  {
    off = l*120*20;
    if( frame > 1 ) frame--;
  } else {
    off = 2*120*20 + (l-2)*140*20;
  }
  off += frame*20*20;

  s = &introfont[off];

  if( ( zoom > 0.9f) && ( zoom < 1.1f ) )
  {
    x -= 10;
    y -= 10;
    
    if( x < 0 ) x = 0;
    if( x > (sw-20) ) x = (sw-20);
    
    d = &((PIXEL *)screen->pixels)[y*rs+x];

    for( y=0; y<20; y++ )
    {
      for( x=0; x<20; x++, s++, d++ )
        if( *s ) *d = logopal[*s];
      d += rs-20;
    }
    return;
  }

  if( ( zoom > 1.9f) && ( zoom < 2.1f ) )
  {
    x -= 20;
    y -= 20;

    if( x < 0 ) x = 0;
    if( x > (sw-40) ) x = (sw-40);

    d = &((PIXEL *)screen->pixels)[y*rs+x];

    for( y=0; y<20; y++ )
    {
      for( x=0; x<20; x++, s++ )
      {
        if( *s )
        {
          d[rs] = logopal[*s];
          *(d++) = logopal[*s];
          d[rs] = logopal[*s];
          *(d++) = logopal[*s];
        } else {
          d += 2;
        }
      }
      d += rs*2-40;
    }
    return;
  }


  size = (int)(20.0f*zoom);
  x -= size/2;
  y -= size/2;

  if( x < 0 ) x = 0;
  if( x > (sw-size) ) x = (sw-size);

  d = &((PIXEL *)screen->pixels)[y*rs+x];

  sxd = 1.0f/zoom;
  syd = 1.0f/zoom;

  sy = 0.0f;
  for( y=0; y<size; y++, sy += syd )
  {
    sx = 0.0f;
    yo = ((int)sy)*20;
    for( x=0; x<size; x++, d++, sx += sxd )
      if( s[yo+(int)sx] ) *d = logopal[s[yo+(int)sx]];
    d += rs-size;
  }
}
