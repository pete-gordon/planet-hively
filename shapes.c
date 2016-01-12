
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "types.h"
#include "shapes.h"

extern SDL_Surface *screen;

extern int sw, sh, rs, se;
extern Uint8 *layers, *click;

extern int32 volume;

extern int isqr[], isqrt[];
extern int stalktopx, stalktopy;

extern PIXEL pal[];

float stopx, stopy;

#define NUM_STALK_VERTICES 22
Sint32 stalk_pts[] = { -16,0,
                       16,0,
                       16,-48,
                       -16,-48,
                       8,-88,
                       -24,-88,
                       0,-120,
                       -24,-136,
                       -48,-112,
                       -56,-160,
                       -32,-176,
                       -56,-200,
                       -32,-216,
                       -56,-216,
                       0,-152,
                       16,-160,
                       24,-152,
                       24,-136,
                       -88,-120,
                       -96,-112,
                       -96,-104,
                       -88,-96 };

#define NUM_STALK_POLYS 18
Sint32 stalk_polys[] = { 0,1,2,
                         0,3,2,
                         2,4,3,
                         3,5,4,
                         4,6,5,
                         5,6,7,
                         5,8,7,
                         8,9,7,
                         9,10,7,
                         9,11,10,
                         10,12,11,
                         11,13,12,
                         6,14,15,
                         6,15,16,
                         6,16,17,
                         8,18,19,
                         8,19,20,
                         8,20,21 };

#define NUM_PETAL_VERTICES 9
Sint32 petal_pts[] = { -16,0,
                       16,0,
                       16,-48,
                       -16,-48,
                       32,-32,
                       12,-32,
                       -32,-32,
                       -12,-32,
                       0,-56 };

#define NUM_PETAL_POLYS 7
Sint32 petal_polys[] = { 0,1,2,
                         0,2,3,
                         1,4,5,
                         5,4,2,
                         0,6,7,
                         6,3,7,
                         3,8,2 };

#define NUM_STAR_VERTICES 10
Sint32 star_pts[] = { -24,-32,
                       24,-32,
                       0,-104,
                       -88,-32,
                       -40,8,
                       88,-32,
                       40,8,
                       -72,80,
                       0,32,
                       64,80 };

#define NUM_STAR_POLYS 8
Sint32 star_polys[] = { 0,1,2,
                        0,3,4,
                        1,5,6,
                        4,7,8,
                        6,8,9,
                        4,1,0,
                        8,1,6,
                        8,1,4 };

#define NUM_BUTTERFLY_VERTICES 66
Sint32 bf_pts[] = { -48,-104,
                       -88,-64,
                       -8,-64,
                       -56,-32,
                       -8,-8,
                       -80,-8,
                       -40,24,
                       0,-64,
                       40,-104,
                       80,-64,
                       48,-32,
                       0,-8,
                       72,-8,
                       32,24,
                       -48,-96,
                       -56,-88,
                       -48,-80,
                       -40,-88,
                       -36,-84,
                       -44,-76,
                       -36,-68,
                       -28,-76,
                       -60,-84,
                       -52,-76,
                       -60,-68,
                       -68,-76,
                       40,-96,
                       32,-88,
                       40,-80,
                       48,-88,
                       28,-84,
                       36,-76,
                       28,-68,
                       20,-76,
                       44,-76,
                       52,-84,
                       60,-76,
                       52,-68,
                       -40,16,
                       -28,4,
                       -40,-8,
                       -52,4,
                       32,16,
                       44,4,
                       32,-8,
                       20,4,
                       48,0,
                       40,-8,
                       48,-16,
                       56,-8,
                       16,0,
                       24,-8,
                       16,-16,
                       8,-8,
                       -24,0,
                       -32,-8,
                       -24,-16,
                       -16,-8,
                       -56,0,
                       -48,-8,
                       -56,-16,
                       -64,-8,
                       -8,-72,
                       0,-72,
                       0,0,
                       -8,0 };

#define NUM_BF_LWING_POLYS 5
Sint32 bf_lwing_polys[] = { 0,1,2,
                         1,3,2,
                         3,4,2,
                         3,5,4,
                         5,6,4 };

#define NUM_BF_RWING_POLYS 5
Sint32 bf_rwing_polys[] = { 7,8,9,
                         9,10,7,
                         11,7,10,
                         11,12,10,
                         11,13,12 };

#define NUM_BF_LPTRN_POLYS 12
Sint32 bf_lptrn_polys[] = { 14,15,16,
                         16,17,14,
                         18,19,20,
                         20,21,18,
                         22,23,24,
                         22,25,24,
                         38,39,40,
                         38,41,40,
                         54,55,56,
                         54,57,56,
                         58,59,60,
                         60,61,58 };

#define NUM_BF_RPTRN_POLYS 12
Sint32 bf_rptrn_polys[] = { 26,27,28,
                         28,29,26,
                         30,31,32,
                         32,33,30,
                         34,35,36,
                         34,37,36,
                         42,43,44,
                         42,45,44,
                         46,47,48,
                         46,49,48,
                         50,51,52,
                         52,53,50 };

#define NUM_BF_BODY_POLYS 2
Sint32 bf_body_polys[] = { 62,63,64,
                         64,62,65 };

struct vtx stalk[NUM_STALK_VERTICES];
struct vtx petal[NUM_PETAL_VERTICES];
struct vtx star[NUM_STAR_VERTICES];
struct vtx bfly[NUM_BUTTERFLY_VERTICES];
struct vtx bbfly[NUM_BUTTERFLY_VERTICES];

struct vtx cloudbot[4];
float clox1, cloy1, clr1;
float clox2, cloy2, clr2;
float clox3, cloy3, clr3;

int starmax;

PIXEL volon[8*16], voloff[8*16];

int shapes_init( void )
{
  float scale;
  int i, x, y;

  scale = ((float)sw)/800;
  for( i=0; i<NUM_STALK_VERTICES; i++ )
  {
    stalk[i].x = ((float)stalk_pts[i*2]) * scale * 0.7f;
    stalk[i].y = ((float)stalk_pts[i*2+1]) * scale;
  }
  
  stopx = -44.0f * scale * 0.7f;
  stopy = -216.0f * scale;
  
  scale = ((float)sw)/950;
  for( i=0; i<NUM_PETAL_VERTICES; i++ )
  {
    petal[i].x = ((float)petal_pts[i*2]) * scale;
    petal[i].y = ((float)petal_pts[i*2+1]) * scale;
  }
  
  scale = ((float)sw)/1024;
  for( i=0; i<NUM_STAR_VERTICES; i++ )
  {
    star[i].x = ((float)star_pts[i*2]) * scale;
    star[i].y = ((float)star_pts[i*2+1]) * scale;
  }

  starmax = (int)(star[5].x*0.7);
  
  scale = ((float)sw)/1200;
  for( i=0; i<NUM_BUTTERFLY_VERTICES; i++ )
  {
    bfly[i].x = ((float)bf_pts[i*2]) * scale;
    bfly[i].y = ((float)bf_pts[i*2+1]) * scale;
    bbfly[i].x = ((float)bf_pts[i*2]) * scale * 0.6f;
    bbfly[i].y = ((float)bf_pts[i*2+1]) * scale * 0.6f;
  }
  
  // Cloud parameters
  scale = ((float)sw)/16;

  clr1   = (int)(1.0f*scale);
  clr2   = (int)(0.8f*scale);
  clr3   = (int)(0.7f*scale);
  
  clox1  = 0.0f;
  cloy1  = -clr1;
  clox2  = -(clr2/2.0f+clr1);
  cloy2  = -clr2;
  clox3  = clr3/2.0f+clr1;
  cloy3  = -clr3;
  
  cloudbot[0].x = clox2;
  cloudbot[0].y = cloy2;
  cloudbot[1].x = clox3;
  cloudbot[1].y = cloy2;
  cloudbot[2].x = clox3;
  cloudbot[2].y = 0.0f;
  cloudbot[3].x = clox2;
  cloudbot[3].y = 0.0f;
  
  if( sh < 400 )
  {
    for( y=0; y<6; y++ )
    {
      volon[y] = pal[4];
      voloff[y] = pal[4];
    }

    for( y=6; y<7*6; y+=6 )
      for( x=0; x<6; x++ )
      {
        volon[x+y] = ((x>0)&&(x<5)) ? pal[0] : pal[4];
        
        if( ( y == 6 ) || ( y == 6*6 ) )
          voloff[x+y] = ((x>0)&&(x<5)) ? pal[0] : pal[4];
        else
          voloff[x+y] = ((x==1)||(x==4)) ? pal[0] : pal[4];
      }
    
    for( ; y<8*6; y++ )
    {
      volon[y] = pal[4];
      voloff[y] = pal[4];
    }
  } else {
    for( y=0; y<8; y++ )
    {
      volon[y] = pal[4];
      voloff[y] = pal[4];
    }

    for( y=8; y<15*8; y+=8 )
      for( x=0; x<8; x++ )
      {
        volon[x+y] = ((x>0)&&(x<7)) ? pal[0] : pal[4];
        
        if( ( y == 8 ) || ( y == 14*8 ) )
          voloff[x+y] = ((x>0)&&(x<7)) ? pal[0] : pal[4];
        else
          voloff[x+y] = ((x==1)||(x==6)) ? pal[0] : pal[4];
      }
    
    for( ; y<16*8; y++ )
    {
      volon[y] = pal[4];
      voloff[y] = pal[4];
    }
  }
  
  return 1;
}



void draw_volume( void )
{
  PIXEL *s, *d, *ld;
  int x, y;

  if( sh < 400 )
  {
    ld = &((PIXEL *)screen->pixels)[4*rs+(sw-(16*6+4))];

    for( x=0; x<16; x++ )
    {
      s = (x<volume) ? volon : voloff;
      d = ld;

      for( y=0; y<8; y++ )
      {
        memcpy( d, s, 6*BYTESPERPIXEL );
        s += 6;
        d += rs;
      }
      
      ld += 6;
    }
    return;
  }

  ld = &((PIXEL *)screen->pixels)[4*rs+(sw-(16*8+4))];

  for( x=0; x<16; x++ )
  {
    s = (x<volume) ? volon : voloff;
    d = ld;

    for( y=0; y<16; y++ )
    {
      memcpy( d, s, 8*BYTESPERPIXEL );
      s += 8;
      d += rs;
    }
    
    ld += 8;
  }
}

void draw_cloud( float ang, int orx, int ory, float r, float scale, PIXEL col, Uint8 lyr, Uint8 clk )
{
  float ca, sa, a2;
  int sx, sy;

  a2 = ang - (3.14159265f/2.0f);

  ca = cos( ang );
  sa = sin( ang );

  sx = ((int)(cos(a2)*r))+orx;
  sy = ((int)(sin(a2)*r))+ory;

  circle( ((int)(clox1*scale*ca - cloy1*scale*sa))+sx,
          ((int)(clox1*scale*sa + cloy1*scale*ca))+sy,
          clr1*scale, col, lyr, clk );

  circle( ((int)(clox2*scale*ca - cloy2*scale*sa))+sx,
          ((int)(clox2*scale*sa + cloy2*scale*ca))+sy,
          clr2*scale, col, lyr, clk );

  circle( ((int)(clox3*scale*ca - cloy3*scale*sa))+sx,
          ((int)(clox3*scale*sa + cloy3*scale*ca))+sy,
          clr3*scale, col, lyr, clk );

  cloudbot[0].sx = ((int)(cloudbot[0].x*scale*ca - cloudbot[0].y*scale*sa))+sx;
  cloudbot[0].sy = ((int)(cloudbot[0].x*scale*sa + cloudbot[0].y*scale*ca))+sy;
  cloudbot[1].sx = ((int)(cloudbot[1].x*scale*ca - cloudbot[1].y*scale*sa))+sx;
  cloudbot[1].sy = ((int)(cloudbot[1].x*scale*sa + cloudbot[1].y*scale*ca))+sy;
  cloudbot[2].sx = ((int)(cloudbot[2].x*scale*ca - cloudbot[2].y*scale*sa))+sx;
  cloudbot[2].sy = ((int)(cloudbot[2].x*scale*sa + cloudbot[2].y*scale*ca))+sy;
  cloudbot[3].sx = ((int)(cloudbot[3].x*scale*ca - cloudbot[3].y*scale*sa))+sx;
  cloudbot[3].sy = ((int)(cloudbot[3].x*scale*sa + cloudbot[3].y*scale*ca))+sy;

  ph_poly( &cloudbot[0], &cloudbot[1], &cloudbot[2], col, lyr, clk );  
  ph_poly( &cloudbot[0], &cloudbot[2], &cloudbot[3], col, lyr, clk );  
}

void draw_bf_leftwing( float ca, float sa, float wscale, int sx, int sy, Uint8 lyr, Uint8 clk, PIXEL col1, PIXEL col2, struct vtx *bs )
{
  int i;
  struct vtx *a, *b, *c;

  for( i=0; i<NUM_BF_LWING_POLYS; i++ )
  {
    a = &bs[bf_lwing_polys[i*3]];
    b = &bs[bf_lwing_polys[i*3+1]];
    c = &bs[bf_lwing_polys[i*3+2]];

    if( a->f2 )
    {
      a->f2 = 0;
      a->sx = ((int)(a->x*wscale*ca - a->y*sa)) + sx;
      a->sy = ((int)(a->x*wscale*sa + a->y*ca)) + sy;
    }

    if( b->f2 )
    {
      b->f2 = 0;
      b->sx = ((int)(b->x*wscale*ca - b->y*sa)) + sx;
      b->sy = ((int)(b->x*wscale*sa + b->y*ca)) + sy;
    }

    if( c->f2 )
    {
      c->f2 = 0;
      c->sx = ((int)(c->x*wscale*ca - c->y*sa)) + sx;
      c->sy = ((int)(c->x*wscale*sa + c->y*ca)) + sy;
    }

    ph_poly( a, b, c, col1, lyr, clk );
  }

  for( i=0; i<NUM_BF_LPTRN_POLYS; i++ )
  {
    a = &bs[bf_lptrn_polys[i*3]];
    b = &bs[bf_lptrn_polys[i*3+1]];
    c = &bs[bf_lptrn_polys[i*3+2]];

    if( a->f2 )
    {
      a->f2 = 0;
      a->sx = ((int)(a->x*wscale*ca - a->y*sa)) + sx;
      a->sy = ((int)(a->x*wscale*sa + a->y*ca)) + sy;
    }

    if( b->f2 )
    {
      b->f2 = 0;
      b->sx = ((int)(b->x*wscale*ca - b->y*sa)) + sx;
      b->sy = ((int)(b->x*wscale*sa + b->y*ca)) + sy;
    }

    if( c->f2 )
    {
      c->f2 = 0;
      c->sx = ((int)(c->x*wscale*ca - c->y*sa)) + sx;
      c->sy = ((int)(c->x*wscale*sa + c->y*ca)) + sy;
    }

    ph_poly( a, b, c, col2, lyr, clk );
  }
}

void draw_bf_rightwing( float ca, float sa, float wscale, int sx, int sy, Uint8 lyr, Uint8 clk, PIXEL col1, PIXEL col2, struct vtx *bs )
{
  int i;
  struct vtx *a, *b, *c;

  for( i=0; i<NUM_BF_RWING_POLYS; i++ )
  {
    a = &bs[bf_rwing_polys[i*3]];
    b = &bs[bf_rwing_polys[i*3+1]];
    c = &bs[bf_rwing_polys[i*3+2]];

    if( a->f2 )
    {
      a->f2 = 0;
      a->sx = ((int)(a->x*wscale*ca - a->y*sa)) + sx;
      a->sy = ((int)(a->x*wscale*sa + a->y*ca)) + sy;
    }

    if( b->f2 )
    {
      b->f2 = 0;
      b->sx = ((int)(b->x*wscale*ca - b->y*sa)) + sx;
      b->sy = ((int)(b->x*wscale*sa + b->y*ca)) + sy;
    }

    if( c->f2 )
    {
      c->f2 = 0;
      c->sx = ((int)(c->x*wscale*ca - c->y*sa)) + sx;
      c->sy = ((int)(c->x*wscale*sa + c->y*ca)) + sy;
    }

    ph_poly( a, b, c, col1, lyr, clk );
  }

  for( i=0; i<NUM_BF_RPTRN_POLYS; i++ )
  {
    a = &bs[bf_rptrn_polys[i*3]];
    b = &bs[bf_rptrn_polys[i*3+1]];
    c = &bs[bf_rptrn_polys[i*3+2]];

    if( a->f2 )
    {
      a->f2 = 0;
      a->sx = ((int)(a->x*wscale*ca - a->y*sa)) + sx;
      a->sy = ((int)(a->x*wscale*sa + a->y*ca)) + sy;
    }

    if( b->f2 )
    {
      b->f2 = 0;
      b->sx = ((int)(b->x*wscale*ca - b->y*sa)) + sx;
      b->sy = ((int)(b->x*wscale*sa + b->y*ca)) + sy;
    }

    if( c->f2 )
    {
      c->f2 = 0;
      c->sx = ((int)(c->x*wscale*ca - c->y*sa)) + sx;
      c->sy = ((int)(c->x*wscale*sa + c->y*ca)) + sy;
    }

    ph_poly( a, b, c, col2, lyr, clk );
  }
}

void draw_butterfly( float angle, float wscale, int sx, int sy, Uint8 lyr1, Uint8 lyr2, Uint8 clk, PIXEL col1, PIXEL col2, PIXEL col3, int small )
{
  int i;
  float ca, sa;
  struct vtx *a, *b, *c;
  struct vtx *bs;
  
  if( small )
    bs = &bbfly[0];
  else
    bs = &bfly[0];

  ca = cos( angle );
  sa = sin( angle );
  
  for( i=0; i<NUM_BUTTERFLY_VERTICES; i++ )
  {
    bs[i].f1 = 1;
    bs[i].f2 = 1;
  }
  
  for( i=0; i<NUM_BF_BODY_POLYS; i++ )
  {
    a = &bs[bf_body_polys[i*3]];
    b = &bs[bf_body_polys[i*3+1]];
    c = &bs[bf_body_polys[i*3+2]];
   
    if( a->f1 )
    {
      a->f1 = 0;
      a->sx = ((int)(a->x*ca - a->y*sa)) + sx;
      a->sy = ((int)(a->x*sa + a->y*ca)) + sy;
    }
    
    if( b->f1 )
    {
      b->f1 = 0;
      b->sx = ((int)(b->x*ca - b->y*sa)) + sx;
      b->sy = ((int)(b->x*sa + b->y*ca)) + sy;
    }
    
    if( c->f1 )
    {
      c->f1 = 0;
      c->sx = ((int)(c->x*ca - c->y*sa)) + sx;
      c->sy = ((int)(c->x*sa + c->y*ca)) + sy;
    }
    
    ph_poly( a, b, c, col1, lyr1, clk );
  }
  
  // Bodge to give a slight "3D" look
  if( angle > 0.0f )
  {
    draw_bf_leftwing( ca, sa, wscale, sx, sy, lyr2, clk, col2, col3, bs );
    draw_bf_rightwing( ca, sa, wscale-0.8f, sx, sy, lyr1, clk, col2, col3, bs );
  } else {
    draw_bf_rightwing( ca, sa, wscale, sx, sy, lyr1, clk, col2, col3, bs );
    draw_bf_leftwing( ca, sa, wscale-0.8f, sx, sy, lyr2, clk, col2, col3, bs );
  }
}

void draw_stalk( float angle, float scale, int sx, int sy, PIXEL col, Uint8 lyr, Uint8 clk, int polys )
{
  float ca, sa;
  int i;

  ca = cos( angle );
  sa = sin( angle );

  for( i=0; i<NUM_STALK_VERTICES; i++ )
  {
    stalk[i].sx = ((int)(stalk[i].x*ca - stalk[i].y*sa) * scale) + sx;
    stalk[i].sy = ((int)(stalk[i].x*sa + stalk[i].y*ca) * scale) + sy;
  }

  for( i=0; i<polys; i++ )
  {
    ph_poly( &stalk[stalk_polys[i*3  ]],
          &stalk[stalk_polys[i*3+1]],
          &stalk[stalk_polys[i*3+2]],
          col, lyr, clk );
  }
  
  stalktopx = ((int)(stopx*ca - stopy*sa)*scale) + sx;
  stalktopy = ((int)(stopx*sa + stopy*ca)*scale) + sy;
}

void draw_star( float scale, float angle, int sx, int sy, int col, Uint8 lyr )
{
  float ca, sa;
  int i;
  
  ca = cos( angle );
  sa = sin( angle );
  
  for( i=0; i<NUM_STAR_VERTICES; i++ )
  {
    star[i].sx = ((int)((star[i].x*ca - star[i].y*sa)*scale)) + sx;
    star[i].sy = ((int)((star[i].x*sa + star[i].y*ca)*scale)) + sy;
  }

  for( i=0; i<NUM_STAR_POLYS; i++ )
  {
    ph_poly( &star[star_polys[i*3  ]],
          &star[star_polys[i*3+1]],
          &star[star_polys[i*3+2]],
          col, lyr, 0 );
  }
}

void setpetalminmax( struct minmax *p, struct vtx *v )
{
  if( v->sx < p->minx ) p->minx = v->sx;
  if( v->sx > p->maxx ) p->maxx = v->sx;
  if( v->sy < p->miny ) p->miny = v->sy;
  if( v->sy > p->maxy ) p->maxy = v->sy;
}

void draw_petal( struct minmax *m, float ca, float sa, float scx, float scy, int sx, int sy, PIXEL col, Uint8 lyr, Uint8 clk )
{
  int i;

  for( i=0; i<NUM_PETAL_VERTICES; i++ )
  {
    petal[i].sx = ((int)(petal[i].x*scx*ca - petal[i].y*scy*sa)) + sx;
    petal[i].sy = ((int)(petal[i].x*scx*sa + petal[i].y*scy*ca)) + sy;
  }

  if( m )
  {
    m->minx = sw;
    m->maxx = -1;
    m->miny = sh;
    m->maxy = -1;
  }

  for( i=0; i<NUM_PETAL_POLYS; i++ )
  {
    if( m )
    {
      setpetalminmax( m, &petal[petal_polys[i*3  ]] );
      setpetalminmax( m, &petal[petal_polys[i*3+1]] );
      setpetalminmax( m, &petal[petal_polys[i*3+2]] );
    }

    ph_poly( &petal[petal_polys[i*3  ]],
          &petal[petal_polys[i*3+1]],
          &petal[petal_polys[i*3+2]],
          col, lyr, clk );
  }
}

void draw_flower( struct flower *fl, int sx, int sy, Uint8 lyr, Uint8 clk, int col1, int col2 )
{
  int i;
  struct petal *p;
  float pa, r;
  
  r = ((float)(fl->r>>8))*0.95f;
  
  pa = 0.0f;

  // Position and draw all the petals
  for( i=0; i<8; i++ )
  {
    p = &fl->p[i];
    if( p->attached )
    {
      p->angle = pa;
      p->ca    = cos( pa );
      p->sa    = sin( pa );
      p->x     = p->sa * r;
      p->y     = p->ca * -r;

      p->sx    = ((int)p->x)+sx;
      p->sy    = ((int)p->y)+sy;
      
      if( p->wobble )
      {
        p->scx = sin( p->wba ) * p->wbs + 1.0f;
        p->scy = cos( p->wba + 3.14159265 ) * p->wbs + 1.0f;
      }
      
      if( p->scy > 0.0f ) draw_petal( NULL, p->ca, p->sa, p->scx*fl->sc, p->scy*fl->sc, p->sx, p->sy, col1, lyr, p->clk );
    }
    
    pa += 3.14159265f / 4.0f;
  }

  circle( sx, sy, fl->r>>8, col2, lyr, clk );
}

void shapes_shut( void )
{
}

static inline int fastsqrt( int n )
{
  if( n > 32767 ) return sqrt( n );
  return isqrt[n];
}

// screen must be locked already.
void circle( int x, int y, int r, PIXEL col, Uint8 lyr, Uint8 clk )
{
  int yt, yb, xc, xe, xo, rc, yc;
  PIXEL *pt, *pb, *xpt, *xpb;
  Uint8  *lt, *lb;
  
  yt = y-r;
  yb = y+r;
  rc = r*r;
  
  pt = &((PIXEL *)screen->pixels)[rs*yt];
  pb = &((PIXEL *)screen->pixels)[rs*yb];
  
  lt = &layers[sw*yt];
  lb = &layers[sw*yb];
  
  for( yc=r; yt<=y; yt++, yb--, yc-- )
  {
    if( yb < 0 ) return;
    if( yt >= sh ) return;
    
    if( ( yt < 0 ) && ( yb >= sh ) )
    {
      pt += rs;
      pb -= rs;
      lt += sw;
      lb -= sw;
      continue;
    }
    
    xo = fastsqrt( rc-(yc*yc) );
    xc = x - xo;
    xe = x + xo;
    if( xc < 0 ) xc = 0;
    
    xpt = &pt[xc];
    xpb = &pb[xc];
    
    if( yt < 0 )
    {
      for( ; xc<xe; xc++ )
      {
        if( xc >= sw ) break;
        *(xpb++)  = col;
        lb[xc]    = lyr; // Set layers[]
        lb[se+xc] = clk; // Set click[]
      }
      pt += rs;
      pb -= rs;
      lt += sw;
      lb -= sw;
      continue;
    }
    
    if( yb >= sh )
    {
      for( ; xc<xe; xc++ )
      {
        if( xc >= sw ) break;
        *(xpt++)  = col;
        lt[xc]    = lyr; // Set layers[]
        lt[se+xc] = clk; // Set click[]
      }
      pt += rs;
      pb -= rs;
      lt += sw;
      lb -= sw;
      continue;
    }

    for( ; xc<xe; xc++ )
    {
      if( xc >= sw ) break;
      *(xpb++)  = col;
      lb[xc]    = lyr; // Set layers[]
      lb[se+xc] = clk; // Set click[]
      *(xpt++)  = col;
      lt[xc]    = lyr; // Set layers[]
      lt[se+xc] = clk; // Set click[]
    }
    pt += rs;
    pb -= rs;
    lt += sw;
    lb -= sw;
  }
}

void ph_poly( struct vtx *v1, struct vtx *v2, struct vtx *v3, PIXEL c, Uint8 lyr, Uint8 clk )
{
  struct vtx *tmp;
  PIXEL *px, *py, *bp;
  Uint8  *lpx, *lpy;
  int xl, xld, xr, xrd, y, x, xe;
  int swpr;
  
  // Completely flat?
  if( ( ( v1->sy == v2->sy ) && ( v1->sy == v3->sy ) ) ||
      ( ( v1->sx == v2->sx ) && ( v1->sx == v3->sx ) ) )
    return;

  // Sort vertically
  if( v1->sy > v2->sy )
  {
    tmp = v1; v1 = v2; v2 = tmp;
  }
  
  if( v2->sy > v3->sy )
  {
    tmp = v2; v2 = v3; v3 = tmp;
    if( v1->sy > v2->sy )
    {
      tmp = v1; v1 = v2; v2 = tmp;
    }
  }

  // On the screen?
  if( ( v3->sy < 0 ) || ( v1->sy >= sh ) )
    return;
  
  bp = (PIXEL *)screen->pixels;
  
  // First section
  if( v1->sy != v2->sy )
  {
    xl = v1->sx<<8;
    xr = v1->sx<<8;
    xld = ((v3->sx-v1->sx)<<8) / (v3->sy-v1->sy);
    xrd = ((v2->sx-v1->sx)<<8) / (v2->sy-v1->sy);
    
    swpr = 0;    
    if( xld > xrd )
    {
      y=xl;  xl=xr;   xr=y;
      y=xld; xld=xrd; xrd=y;
      swpr = 1;
    }
    
    // Top section on the screen?
    if( v2->sy >= 0 )
    {
      y = v1->sy;
      if( y < 0 )
      {
        xl += xld * -y;
        xr += xrd * -y;
        y=0;
      }
      
      py  = &bp[y*rs];
      lpy = &layers[y*sw];
      for( ; y<v2->sy; y++ )
      {
        if( y >= sh )
          return;
        
        x  = xl>>8;
        xe = (xr>>8)+1;
        if( xe > sw ) xe = sw;
        if( x < 0 ) x = 0;
        px  = &py[x];
        lpx = &lpy[x];
        
        for( ; x<xe; x++ )
        {
          *(px++) = c;
          lpx[se] = clk;
          *(lpx++) = lyr;
        }
       
        xl += xld;
        xr += xrd;
        
        py += rs;
        lpy += sw;
      }

    } else {
      // No, so set up as if we drew the top section
      if( swpr )
        xr += xrd * (v2->sy-v1->sy);
      else
        xl += xld * (v2->sy-v1->sy);
    }

    // No second part?
    if( v2->sy == v3->sy )
      return;
    
    // Set up for second section
    if( swpr )
    {
      xl = v2->sx<<8;
      xld = ((v3->sx-v2->sx)<<8) / (v3->sy-v2->sy);
    } else {
      xr = v2->sx<<8;
      xrd = ((v3->sx-v2->sx)<<8) / (v3->sy-v2->sy);
    }

  } else {
    // No first section. Set up for second section.
    if( v1->sx < v2->sx )
    {
      xl = v1->sx<<8;
      xld = ((v3->sx-v1->sx)<<8) / (v3->sy-v1->sy);
      xr = v2->sx<<8;
      xrd = ((v3->sx-v2->sx)<<8) / (v3->sy-v2->sy);
    } else {
      xl = v2->sx<<8;
      xld = ((v3->sx-v2->sx)<<8) / (v3->sy-v2->sy);
      xr = v1->sx<<8;
      xrd = ((v3->sx-v1->sx)<<8) / (v3->sy-v1->sy);
    }    
  }
  
  // Second part
  y = v2->sy;
  if( y < 0 )
  {
    xl += xld * -y;
    xr += xrd * -y;
    y=0;
  }
  
  py = &bp[y*rs];
  lpy = &layers[y*sw];
  for( ; y<v3->sy; y++ )
  {
    if( y >= sh )
      return;
    
    x  = xl>>8;
    xe = (xr>>8)+1;
    if( xe > sw ) xe = sw;
    if( x < 0 ) x = 0;
    px = &py[x];
    lpx = &lpy[x];
    
    for( ; x<xe; x++ )
    {
      *(px++) = c;
      lpx[se] = clk;
      *(lpx++) = lyr;
    }
   
    xl += xld;
    xr += xrd;

    py += rs;
    lpy += sw;
  }
}
