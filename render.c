#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <SDL/SDL.h>

#include "types.h"
#include "shapes.h"
#include "render.h"
#include "main.h"
#include "text.h"
#include "hvl_replay.h"

extern SDL_Surface *screen;
extern int clicked, clickx, clicky;
extern struct hvl_tune *tune;
extern int ctune, paused;
extern int fontheight, starmax;
extern int tbx, tby, tbw, tbh, tbbarh, tbbrd, tbaw;
extern int dragger, enddragger, wdragger, dragx, dragy;
extern int tbpressed;

int wdxo, wdyo;
int voltimer=0;

int part=0, fader, fadeadd;
int plx,  ply,  plr, ptrg;
int pldx, pldy, pldr;
int sunx, suny, sundy, sunty, sunr;
float fla1, fla2, bfbnc, deaddrop;
int stalktopx, stalktopy, intropart, introdoing;
float sunang=0.0f;
int nextfpet=0;
int tbon, tbclos=0;
float tbsc, tbadd;
int notxttime=0;
float tbwob;

int pl_r, pl_rd, pl_g, pl_gd;

// irsandupoghetlvy
// 0123456789012345
Sint8 llorder[] = {  0,  1,  0,  2, -1,
                     3,  4,  5, -1,
                     6,  7, -1,  1,  8,  6,  9, 10,

                     7,  1, 11,  2, 11,  4, 12,

                     7, 13,  3,  4, 11, 12, -1,
                    10,  0, 14, 11, 13, 15 };

int introdat[] = {   0, 17, 0, 110,
                    17,  7, 0, 180,
                    24, 13, 0, 310,
                     0,  0, 0,   0 };

int introappear[17], is1, is2;
float introzoom[17], introzoomsub, introzoomtarg;

#define NUMRAYS 8
struct vtx sunrays[NUMRAYS*4];

#define MAXFPETS 32
struct floating_petal fpt[MAXFPETS];

#define NUMSTARS 32
struct star stars[NUMSTARS];

#define NUMBFLYS 2
struct butterfly bflys[NUMBFLYS];

#define NUMBBFLYS 2
struct butterfly bbflys[NUMBBFLYS];

#define NUMCLOUDS 4
struct cloud clouds[NUMCLOUDS];

struct flower flwr[2];

char *tunename[] = { "sweeties",
                     "forsaken",
                     "freeside run",
                     "peanuts",
                     "galactic emeralds",
                     "pantsomime",
                     "testing123",
                     "incognito crust",
                     "bonus track" };

char *author[] = { "syphus/up rough",
                   "virgill/haujobb",
                   "syphus/up rough",
                   "lavaburn/hsc",
                   "monk",
                   "joey and splif",
                   "breed",
                   "m0d",
                   "???" };

char *authnotes[] = { "syphus says: started ages ago, finished at the last minute "
                      "(4.38am on 31/01/2007;), designed for clumsy "
                      "dancing and star-crossed ceiling-staring. "
                      "much love to all...",
                      
                      "virgill says: nothing!",
                      
                      "syphus says: gartantuan greets to the usuals, with extra "
                      "hearts to xeron, spotup and exobuzz for this "
                      "amazing software and for their tireless feature "
                      "implementations^:) words to d0pefish for all the "
                      "hardware help and fuckings to parapete for "
                      "killing the scene.",
                      
                      "lavaburn says: a little reggae-style piece, done in an insane hurry, "
                      "literally a couple of hours before the deadline! this "
                      "would be my first tune made in hively tracker; i am also "
                      "learning ahx...^:) thanks to syphus for giving me the "
                      "poke in the ribs necessary for me to actually do "
                      "some work. :) and huuuge thanks to xeron, spotup and "
                      "exobuzz for all their hard work in bringing us this "
                      "brilliant tracker! cheers!^:)",

                      "monk says: hey all! hivelytracker is a really wonderful tool, very "
                      "user-friendly and wonderful for old tracker-composers "
                      "like me. i seriously recommend it for anyone who has "
                      "enjoyed protracker, odintracker, goattracker, "
                      "fasttrackerii or renoise. but also for people who have "
                      "used to sequencing^- maybe this is the tracker that will "
                      "let you realize the good side about tracking also! "
                      "greetings to all #amigascne people, klegg, parazite, "
                      "my mom, zetanet, thoron, siiseli, genny and others that "
                      "i forgot. peace and love for all!",
                      
                      "joey and splif say: joey and splif graeet al friens and even haterzx!! lol "
                      "pease out nubes",

                      "breed says: long live commodore!",
                      
                      "m0d says: greetings to all the #milkytracker folks and all of my "
                      "dear friends at #modarchive on espernet, and thanks to "
                      "the folks who've been proactivly bringing freshness to "
                      "the scene i.e the hivelytracker folks :) i've been "
                      "ahx-type-tracker-composing since december 2006 (the "
                      "12th to be exact), at this point i've created 13 ahx "
                      "songs, i haven't tracked since my protracker days in "
                      "1996 and this ahx revival (and being part of it) is "
                      "really kicking some arse! although i mainly use my "
                      "trusty old amiga to run ahx, running hivelytracker "
                      "under winuae was, in a fashion, a lot of fun. "
                      "thanks for the experience, and thanks for reading "
                      "this blahness :)",
                      
                      "congratulations on finding the bonus track. you must "
                      "now listen to it non stop for 3 days, or until your "
                      "head explodes :-)" };

char *top_blah[] = { "www.hivelytracker.com",
                     "www.irishq.dk",
                     "www.uprough.net",
                     "bbcode and unicode don't work on the oneliner",
                     "super special greets to abyss. thanks for ahx :-)",
                     "this port was done by blacky stardust",
                     NULL };
                                           
char *othertexts[] = { "|welcome to\n\n"
                       "planet hively\n"
                       "\n"
                       "{this music disk contains all the entries to the "
                       "first official hively tracker competition. users of "
                       "hively tracker were invited to submit tunes to be "
                       "judged by the members of iris and up rough. the "
                       "winner was given a c64 dtv, courtesy of ggs-data!\n\n\n"
                       "|the results\n"
                       "\n"
                       "{1:sweeties by syphus\n"
                       "}(17 points)\n\n"
                       "{2:forsaken by virgill\n"
                       "}(16 points)\n\n"
                       "{3:freeside run by syphus\n"
                       "}(13 points)\n\n"
                       "{4:peanuts by lavaburn\n"
                       "}(11 points)\n\n"
                       "{4:galactic emeralds by monk\n"
                       "}(11 points)\n\n"
                       "{5:pantsomime by joey and splif\n"
                       "}(3 points)\n\n"
                       "{6:testing123 by breed\n"
                       "}(2 points)\n\n"
                       "{7:incognito crust by m0d\n"
                       "}(0 points)\n\n"
                       "{thanks to all entrants and congratulations to "
                       "syphus! hope you've enjoyed your c64 dtv for the "
                       "past year or so while waiting for this musicdisk!",
                       
                       "|credits\n"
                       "\n"
                       "{code, design, gfx\n"
                       "}xeron/iris\n\n"
                       "{design, gfx\n"
                       "}spot/up rough\n\n"
                       "{music\n"
                       "}syphus/up rough\n"
                       "virgill/haujobb\n"
                       "lavaburn/hsc\n"
                       "monk\n"
                       "joey and splif\n"
                       "breed\n"
                       "m0d\n\n"
                       "{this port\n"
                       "}blacky stardust\n\n"
                       "{yes, i know, this musicdisk is very, very late. "
                       "the original plan was to make it for 030 amigas "
                       "but that requires an assembler optimised "
                       "replayer. currently the replayer "
                       "only exists in fairly unoptimised c code form "
                       "and requires a fast 68k amiga. a couple "
                       "of different people offered to create an optimised "
                       "player, but here we are well over a year later without "
                       "one, so i decided to code the musicdisk in "
                       "plain c and sdl, so that it could be ported to "
                       "as many platforms as possible, and here is the "
                       "result. i hope you like it...\n"
                       "}xeron/iris",
                       
                       "|greets\n"
                       "\n"
                       "apathy, dcs, depth, drifters, ephidrena, fairlight, "
                       "haujobb, loonies, mankind, nature, perihelion, rebels, "
                       "rno, scarab, software failure, tbl, triad, tulou "
                       "and ukscene allstars!"
                       "\n\n\n"
                       "personal greets from xeron\n"
                       "\n"
                       "bonkers: hope to see you soon...\n\n"
                       "britelite: arse!\n\n"
                       "doooooom: egallib!\n\n"
                       "parapete: moo!\n\n"
                       "spot: keep on porting :-)\n\n"
                       "syphus: keep on hivelying :-)\n\n",

                       "\n\nlike these tunes? why not download the "
                       "tracker they were made in? its available "
                       "for amiga os3, os4 and aros, and it kicks "
                       "ass. download it from:\n\n"
                       "|www.hivelytracker.com\n\n"
                       "{where you'll also find friendly forums, "
                       "the latest hively news, and a very exciting "
                       "full stop.\n\n\n\n\n"
                       "do not use hively tracker if you are pregnant, "
                       "stupid, or suffer from tone deafness. "
                       "consult your doctor if symptoms persist.",
                       
                       "\n\nevening phreekz! what's up?\n"
                       "\n"
                       "spot / up rough on da keys! this disk is more "
                       "than a little delayed. life got in the way as "
                       "usual... atm my apartmen is getting a facelift "
                       "and i am living at my dads place in the middle "
                       "of the sea.... me and sabina went to strandbaren "
                       "to get some hangover food and to check out the "
                       "local fishermen types, the usual crowd of fishermen "
                       "and drunks was hanging in the bar, the food was "
                       "so-so. nothing much going on here. i hope i am "
                       "moving back to my apartment in a week or so.. "
                       "hively tracker development has been slow lately, "
                       "xeron has a lot of other projects going on and "
                       "he has also moved to a house, i think. "
                       "the tracks submitted to the compo was of quite "
                       "high quality, that's nice, my personal favourite "
                       "was the track by virgill. it has a genuine oldskool "
                       "touch to it that i like. syphus won the competition, "
                       "congrats not only for winning, but also for making "
                       "it to the top crew, welcome to up rough man!\n"
                       "\n"
                       "so what's going on in up rough then?...\n"
                       "\n"
                       "we are 10 years old! the anniversary party was a "
                       "blast! we packed the place with people, and they "
                       "all seemed to enjoy it a lot! there's lots of "
                       "videos and photos from the party to be found "
                       "on www.uprough.net. we recorded all dj sets and "
                       "live acts from the party too, there's many hours "
                       "of music waiting to be smacked into your ipods! "
                       "some of the compusphere organizers was present "
                       "at the party too, and they decided to dedicate "
                       "this years edition of compusphere to up rough! "
                       "thanx guys! we will try to deliver something fresh!\n"
                       "\n"
                       "about this disk then.. the aim was "
                       "for it to be multiplatform and we'll "
                       "do our best to get it out on as many "
                       "platforms as possible!\n"
                       "\n"
                       "gotta send this text to xeron now, "
                       "so see ya all next time!\n"
                       "\n"
                       "dipswitch on the keys. i am forced to write this "
                       "text else i will be toe'd by spot. greets to all "
                       "people who make the scene a warm place in these "
                       "cold days, and see you around the bonfire on "
                       "breakpoint!",

                       "|darkhawk speaks!\n"
                       "\n"
                       "{hey there sceners and welcome to "
                       "a new iris release, this time in "
                       "co-operation with up rough. we've "
                       "been quite silent for a while and "
                       "had some changes in the group, but "
                       "as you can see, we're not dead. "
                       "this production has been released "
                       "on several platforms, continuing "
                       "the small trend the last eurochart "
                       "started of multiplatform release. "
                       "in the future, you will see more "
                       "releases from iris on non 68k platforms, "
                       "demos mostly, but this doesn't mean we "
                       "will forget the classic amiga. perhaps "
                       "you will see a release from us at "
                       "breakpoint 2008 and perhaps our next "
                       "release will first see the light of day "
                       "later this year, but we're still here, "
                       "both giving to the scene and watching it.\n\n"
                       "keep the scene in your heart and keep "
                       "releasing your work, be it for amiga "
                       "or pc - the world needs the little "
                       "niche of computer art and creativity "
                       "here. :)",

                       "|some ramblings\nfrom syphus\n"
                       "\n"
                       "{well, we waited a year for this musicdisk to "
                       "be ready and once it finally got finished, i "
                       "held it up for another few weeks by being a lazy "
                       "bastard about writing this text... and now i don't "
                       "even know what to say. hivelytracker is fucking "
                       "amazing, first of all. since its release in 2006/7, "
                       "xeron has gone from being quiet, mild-mannered amiga genius "
                       "to the face that men and women now shut their eyes "
                       "and imagine while having loud sex.\n\n"
                       "if you're checking out this disk, you probably already "
                       "know why hivelytracker is so amazing - and not only "
                       "because of its sixteen channels of ahx-spec, "
                       "sid-reminiscent chipsynth sound, nor because of its "
                       "extra effects column, amazing ringmod capabilities, "
                       "tabbed module loading, the ability to load and save ahx "
                       "files or the 256-increment, per-instrument/per-channel "
                       "stereo positioning feature. no, it's not only for these "
                       "reasons: it also has to do with buzz/bitfellas tirelessly "
                       "porting it to 68k amiga and aros from os4 and with "
                       "spot/up rough making it look so fucking brill with a "
                       "selection of interface skins.\n\n"
                       "so while these towering bastions of manhood "
                       "doth bestride this narrow world like code-colossi, "
                       "we peons do our best to use their gift for good. "
                       "it's been loads of fun writing hvl tunes for the last "
                       "year, and it was great fun to take part in the competition "
                       "that led to this chipdisk. some great stuff's appeared "
                       "for hively, from amiga oldschoolers and tracking newcomers "
                       "alike. anyway, i don't need to ramble on about that, "
                       "since you can check that shit out for yourself.\n\n"
                       "spot and xeron need extra creds for this lovely, "
                       "multi-platform sdl musicdisk (which was well worth "
                       "waiting for) and of course shouts and smiles have to "
                       "fly to everyone who contributed. oh, and to ggs data ;)\n\n"
                       "p.s. - lavaburn: get off that fucking xbox 360 and "
                       "finish some hively tunes :d",
                       
                       "|so this is it\n"
                       "\n"
                       "it is finally done\n"
                       "\n"
                       "after many delays and let downs\n"
                       "\n"
                       "we can rest.\n"
                       "\n"
                       "\n"
                       "until the next time..." };

char nptxt[512];
char antxt[512];
char *antxtptr = NULL;
extern int sw, sh;
int rs;
int tbtxtnum=0;

int midx, midy, se, bsw, sxe, sye, bss, bse;
int qsh;
int isqrt[32768];

int gcount;

Uint8 *layers = NULL;
Uint8 *click;
PIXEL *bg = NULL;

PIXEL skytxtpal[20];
PIXEL logopal[16];

#define PAL_ENTRIES 55
Uint8  spal[] = { 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00,
                  0xff, 0xff, 0x00,
                  0x40, 0xff, 0x40,
                  0xff, 0xff, 0xff,
                  0xe0, 0xe0, 0xe0,
                  0xd0, 0xd0, 0xd0,
                  0xc0, 0xc0, 0xc0,
                  0xb0, 0xb0, 0xb0,
                  0xa0, 0xa0, 0xa0,
                  0x90, 0x90, 0x90,
                  0x80, 0x80, 0x80,
                  0x70, 0x70, 0x70,
                  0x60, 0x60, 0x60,
                  0x50, 0x50, 0x50,
                  0x40, 0x40, 0x40,
                  0x30, 0x30, 0x30,
                  0x20, 0x20, 0x20,
                  0x10, 0x10, 0x10,
                  0x00, 0x00, 0x00,
                  0x40, 0xff, 0xff,
                  0xff, 0x20, 0x20,
                  0xee, 0xbb, 0x00,   // 22
                  0xff, 0xff, 0xbb,   // 23
                  0xa0, 0x00, 0xa0,   // 24
                  0x01, 0x06, 0x08,   // 25
                  0x01, 0x06, 0x08,
                  0x02, 0x0c, 0x0f,
                  0x03, 0x12, 0x17,
                  0x03, 0x18, 0x1f,
                  0x04, 0x1e, 0x26,   // 30
                  0x05, 0x24, 0x2e,
                  0x06, 0x2a, 0x36,
                  0x07, 0x30, 0x3d,
                  0x08, 0x36, 0x45,
                  0x09, 0x3c, 0x4d,   // 35
                  0x0a, 0x42, 0x54,
                  0x0a, 0x48, 0x5c,
                  0x0b, 0x4e, 0x64,
                  0x0c, 0x54, 0x6b,
                  0x0d, 0x5a, 0x73,   // 40
                  0x0e, 0x5f, 0x7b,
                  0x0f, 0x65, 0x82,
                  0x10, 0x6b, 0x8a,
                  0x10, 0x71, 0x92,
                  0x11, 0x77, 0x99,   // 45
                  0x12, 0x7d, 0xa1,
                  0x13, 0x83, 0xa9,
                  0x14, 0x89, 0xb0,
                  0x15, 0x8f, 0xb8,
                  0x16, 0x95, 0xc0,   // 50
                  0x17, 0x9b, 0xc7,
                  0x17, 0xa1, 0xcf,
                  0x18, 0xa7, 0xd7,
                  0x19, 0xad, 0xde,
                  0x1a, 0xb3, 0xe6 };
                  

Uint8 *skyrgb;

PIXEL pal[PAL_ENTRIES];

static inline int edge_check1( Uint8 c, Uint8 *lb, Uint8 *pm, Uint8 *pb )
{
  return (( *lb != c )||( *pm != c )||( *pb != c ));
}

static inline int edge_check2( Uint8 *lu, Uint8 c, Uint8 *lb, Uint8 *pu, Uint8 *pm, Uint8 *pb )
{
  return (( *lb != c )||( *lu != c )||( *pu != c )||( *pm != c )||( *pb != c ));
}

static inline int edge_check3( Uint8 *lm, Uint8 *lb, Uint8 *pm, Uint8 *pb )
{
  Uint8 c;

  c = *lm;
  if(( *lb != c )||( *pm != c )||( *pb != c )) return 1;
  if(( *++lm != c )||( *++lb != c )) return 1; 
  return 0;
}

static inline int edge_check4( Uint8 *lu, Uint8 *lm, Uint8 *lb, Uint8 *pu, Uint8 *pm, Uint8 *pb )
{
  Uint8 c;

  c = *lm;
  if(( *lb != c )||( *lm != c )||( *pu != c )||( *pm != c )||( *pb != c )) return 1;
  if(( *++lu != c )||( *++lm != c )||( *++lb != c )) return 1;
  return 0;
}

void render_edge_zone( struct minmax *m )
{
  int x, y, xe, ty, ye;
  PIXEL *p;
  Uint8 *lu, *lm, *lb;

  ty = m->miny;
  if( ty == 0 )
  {
    lm = layers;
    lb = &lm[sw];
    p  = (PIXEL *)screen->pixels;
    
    x = m->minx;
    if( x == 0 )
    {
      if( edge_check3( lm, lb, lm, lb ) )
        *p = 0;
      x++;
    }
    
    for( xe=x-1; x<=m->maxx; x++, xe++ )
      if( edge_check3( &lm[x], &lb[x], &lm[xe], &lb[xe] ) )
        p[x] = 0;
    ty++;
  }
  
  lm = &layers[ty*sw];
  lu = &lm[-sw];
  lb = &lm[sw];
  
  p  = &((PIXEL *)screen->pixels)[ty*rs];
  
  ye = m->maxy+1;
  if( ye >= sye )
  {
    m->maxy = sye;
    ye = sye-1;
  }
  
  for( y=ty; y<=ye; y++ )
  {
    xe = m->minx-1;
    for( x=m->minx; x<=m->maxx; x++, xe++ )
    {
      if( edge_check4( &lu[x], &lm[x], &lb[x], &lu[xe], &lm[xe], &lb[xe] ) )
        p[x] = 0;
    }
    
    lm += sw;
    lu += sw;
    lb += sw;
    
    p += rs;
  }
  
  if( y > m->maxy ) return;

  x = m->minx;
  if( x == 0 )
  {
    if( edge_check3( &lm[x], &lu[x], &lm[x], &lu[x] ) )
      p[x] = 0;
    x++;
  }

  for( xe=x-1; x<=m->maxx; x++, xe++ )
    if( edge_check3( &lm[x], &lu[x], &lm[xe], &lu[xe] ) )
      p[x] = 0;  
}

void render_edges( void )
{
  int x, y, xe, yo;
  PIXEL *p;
  Uint8 *lu, *lm, *lb, *pu, *pm, *pb;
  
  lu = layers;
  lm = layers;
  lb = &layers[sw];
  
  pu = lu-1;
  pm = lm;
  pb = lb;

  p = (PIXEL *)screen->pixels;
  if( edge_check3( lm++, lb++, pm, pb ) )
    *p = 0;

  for( x=1; x<sxe; x++, lm++, lb++, pm++, pb++ )
    if( edge_check3( lm, lb, pm, pb ) )
      p[x] = 0;  

  if( edge_check1( *(lm++), lb++, pm++, pb++ ) )
    p[x] = 0;

  xe = rs+sxe;
  for( y=1, yo=rs; y<sye; y++, yo+=rs, xe+=rs )
  {
    if( edge_check4( lu, lm, lb, lu, lm, lb ) )
      p[yo] = 0;
    
    lu++; lm++; lb++; pu++; pm++; pb++;
    
    for( x=yo+1; x<xe; x++, lu++, lm++, lb++, pu++, pm++, pb++ )
      if( edge_check4( lu, lm, lb, pu, pm, pb ) )
        p[x] = 0;
    
    if( edge_check2( lu++, *(lm++), lb++, pu++, pm++, pb++ ) )
      p[x] = 0;
  }

  if( edge_check3( lm, lu, lm, lu ) )
    p[yo] = 0;
  
  lm++; lb++; pm++; pu++;

  for( x=1; x<sxe; x++, lm++, lu++, pm++, pu++ )
    if( edge_check3( lm, lu, pm, pu ) )
      p[x+yo] = 0;  

  if( edge_check1( *lm, lu, pm, pu ) )
    p[x+yo] = 0;
}

void create_bg( void )
{
  int r, g, b, x, yt, yb, ye;
  int rd, gd, bd, m;
  PIXEL col;
  
  r = g = b = 0;
  rd = (85<<8) / midy;
  gd = ( 8<<8) / midy;
  bd = (65<<8) / midy;
  
  for( yt=0, yb=(sh-1); yt<midy; yt++, yb-- )
  {
    skyrgb[yt*3  ] = (r>>8)+(rand()&3);
    skyrgb[yt*3+1] = (g>>8)+(rand()&3);
    skyrgb[yt*3+2] = (b>>8)+(rand()&3);
    skyrgb[yb*3  ] = (r>>8)+(rand()&3);
    skyrgb[yb*3+1] = (g>>8)+(rand()&3);
    skyrgb[yb*3+2] = (b>>8)+(rand()&3);
    r += rd; g += gd; b += bd;
  }
  
  ye = rs*sh;

  yt = 0;
  m = rs-sw;
  for( yb=0; yb<sh; yb++ )
  {
    col = SDL_MapRGB( screen->format, skyrgb[yb*3], skyrgb[yb*3+1], skyrgb[yb*3+2] );
    for( x=0; x<sw; x++ )
      bg[yt++] = col;
    yt += m;
  }
}

void sunrise( int amount )
{
  int r, g, b, ia;
  int fr, fg, fb;
  int rd, gd, bd;
  int yt, yb, x, m;
  PIXEL col;

  r = 45<<8;
  g = 225<<8;
  b = 245<<8;
  rd = ((255<<8)-r) / sh;
  gd = ((255<<8)-g) / sh;
  bd = ((255<<8)-b) / sh;
  
  ia = 256-amount;

  yt = 0;
  m = rs-sw;
  for( yb=0; yb<sh; yb++ )
  {
    fr = (skyrgb[yb*3  ]*ia)+(r>>8)*amount;
    fg = (skyrgb[yb*3+1]*ia)+(g>>8)*amount;
    fb = (skyrgb[yb*3+2]*ia)+(b>>8)*amount;
    r += rd; g += gd; b += bd;
    
    col = SDL_MapRGB( screen->format, fr>>8, fg>>8, fb>>8 );

    for( x=0; x<sw; x++ )
      bg[yt++] = col;

    yt += m;
  }
  
}

void sunset( int amount )
{
  int r, g, b, ia;
  int fr, fg, fb;
  int rd, gd, bd;
  int yt, yb, x, m;
  PIXEL col;

  for( r=0; r<PAL_ENTRIES; r++ )
    pal[r] = SDL_MapRGB( screen->format, (spal[r*3]*amount)>>8, (spal[r*3+1]*amount)>>8, (spal[r*3+2]*amount)>>8 );

  r = 45<<8;
  g = 225<<8;
  b = 245<<8;
  rd = ((255<<8)-r) / sh;
  gd = ((255<<8)-g) / sh;
  bd = ((255<<8)-b) / sh;

  ia = 256-amount;
  
  yt = 0;
  m = rs-sw;
  for( yb=0; yb<sh; yb++ )
  {
    fr = (r>>8)*amount;
    fg = (g>>8)*amount;
    fb = (b>>8)*amount;
    r += rd; g += gd; b += bd;
    
    col = SDL_MapRGB( screen->format, fr>>8, fg>>8, fb>>8 );

    for( x=0; x<sw; x++ )
      bg[yt++] = col;
    
    yt += m;
  }
}


void show_now_playing( int immediate )
{
  if( sw >= 320 )
  {
    sprintf( nptxt, "now playing: \"%s\" by %s",
      tunename[ctune],
      author[ctune] );
    fade_in( nptxt, immediate );
    antxtptr = authnotes[ctune];
  } else {
    sprintf( nptxt, "now playing: \"%s\"", tunename[ctune] );
    sprintf( &nptxt[256], "by %s", author[ctune] );
    fade_in( nptxt, immediate );
    antxtptr = &nptxt[256];
    return;
  }
}

void reset_bbfly( int i )
{
  if( rand()&1 )
  {
    bbflys[i].sx = ((rand()%sw)-sw)<<8;
    bbflys[i].tsx = (rand()%sw)+sw;
    bbflys[i].ang = 3.14159265/6.0f;
  } else {
    bbflys[i].sx = ((rand()%sw)+sw)<<8;
    bbflys[i].tsx = (rand()%sw)-sw;
    bbflys[i].ang = -3.14159265f/6.0f;
  }
  bbflys[i].sy = ((rand()%qsh)+midy)<<8;
  bbflys[i].tsy = (rand()%qsh)+midy;
  bbflys[i].dsx = ((bbflys[i].tsx<<8)-bbflys[i].sx) / (40*TARGET_FPS);
  bbflys[i].dsy = ((bbflys[i].tsy<<8)-bbflys[i].sy) / (40*TARGET_FPS);
  bbflys[i].dead = 0;
}

void reset_bfly( int i )
{
  if( rand()&1 )
  {
    bflys[i].sx = ((rand()%sw)-sw)<<8;
    bflys[i].tsx = (rand()%sw)+sw;
    bflys[i].ang = 3.14159265/6.0f;
  } else {
    bflys[i].sx = ((rand()%sw)+sw)<<8;
    bflys[i].tsx = (rand()%sw)-sw;
    bflys[i].ang = -3.14159265f/6.0f;
  }
  bflys[i].sy = ((rand()%midy)+qsh)<<8;
  bflys[i].tsy = (rand()%midy)+qsh;
  bflys[i].dsx = ((bflys[i].tsx<<8)-bflys[i].sx) / (20*TARGET_FPS);
  bflys[i].dsy = ((bflys[i].tsy<<8)-bflys[i].sy) / (20*TARGET_FPS);
  bflys[i].dead = 0;
}

void reset_cloud( int i )
{
  clouds[i].ang   = (3.14159265f/4.0f)+(((float)(rand()%20))/40.0f);
  clouds[i].r     = (plr>>8) + qsh + (rand()%qsh);
  clouds[i].speed = ((float)((rand()%20)+10))/(TARGET_FPS*210.0f);
}

// Shitty shitty bubble sort
// but for 4 items, who cares?
void sort_clouds_by_speed( void )
{
  struct cloud c;
  int sorted, i;
  
  do
  {
    sorted = 1;
    for( i=0; i<(NUMCLOUDS-1); i++ )
    {
      if( clouds[i].speed > clouds[i+1].speed )
      {
        c = clouds[i];
        clouds[i] = clouds[i+1];
        clouds[i+1] = c;
        sorted = 0;
      }
    }
  } while( sorted == 0 );
}

void start_part( int n )
{
  int i, j;
  float ang, anga, fi, fj;

  switch( n )
  {
    case PART_STARFIELD:
      intropart = 0;
      introdoing = 0;

      if( sh < 400 )
      {
        is1 = 10;
        is2 = 17;
      } else {
        is1 = 20;
        is2 = 34;
      }

      for( i=0, j=0; i<17; i++ ) j += llorder[i]<1?is1:is2;
      introdat[2] = midx - j/2 + is1;
      for( i=17, j=0; i<24; i++ ) j += llorder[i]<1?is1:is2;
      introdat[6] = midx - j/2 + is1;
      for( i=24, j=0; i<37; i++ ) j += llorder[i]<1?is1:is2;
      introdat[10] = midx - j/2 + is1;

      for( i=0; i<17; i++ )
      {
        introappear[i] = -(i+1) * TARGET_FPS / 15;
        introzoom[i] = sh<400?4.0f:8.0f;
      }
      introzoomsub = (sh<400?10.0f:20.0f) / TARGET_FPS;
      introzoomtarg = sh<400?1.0f:2.0f;
      
      for( i=0; i<NUMSTARS; i++ )
      {
        stars[i].sx = (rand()%sw)<<8;
        stars[i].sy = rand()%sh;
        stars[i].ang = ((float)(rand()&255))/160.0f;
        stars[i].speed = (((rand()%4)+1)<<9);
        stars[i].scale = ((float)stars[i].speed)/3000.0f;
        stars[i].speed = (stars[i].speed*sw)/(TARGET_FPS*16);
      }
      break;
      
    case PART_PLANET_IN:
      ptrg = midx;

      plr  = (sw/32)<<8;
      pldr = (((sw/6)<<8)-plr) / (TARGET_FPS*2);
      plx  = (sw<<8)+plr*3;
      ply  = midy<<8;
      pldx = ((ptrg<<8)-plx) / (TARGET_FPS*2);
      pldy = 0;
      break;
    
    case PART_PLANET_ZOOM:
      // Ugly bodge
      if( (sh*5/4) == sw )
      {
        ptrg = sh*2+(midy*2)/3;
      } else if( (sh*16/10) == sw ) {
        ptrg = sh*2+(midy*4)/3;
      } else if( (sh*16/9) == sw ) {
        ptrg = sh*3;
      } else {
        ptrg = sh*2+midy;
      }

      pldx = ((((sw*7)/10)<<8)-plx) / (TARGET_FPS*2);
      pldy = ((ptrg<<8)-ply) / (TARGET_FPS*2);
      pldr = (( ((sw*4)/3) <<8)-plr) / (TARGET_FPS*2);
      break;
    
    case PART_STARS_OUT:
      break;
    
    case PART_SUNRISE:
      gcount = 0;
      
      sunr  = sw/14;
      sunx  = sw/7;
      suny  = (sh+sunr)<<8;
      sunty = (sh/6)+fontheight;
      sundy = ((sunty<<8)-suny) / 128;
      
      pl_r   = 0;
      pl_rd  = (0x20<<8)/128;
      pl_g   = 0;
      pl_gd  = (0xc0<<8)/128;

      ang = 0.0f;
      anga = 3.14159265f/NUMRAYS;
      
      fi = ((float)sunr)*0.9f;
      fj = fi * 1.7f;
      
      for( i=0; i<NUMRAYS; i++ )
      {
        sunrays[i*4  ].x = cos( ang )*fj;
        sunrays[i*4  ].y = sin( ang )*fj;
        sunrays[i*4+1].x = cos( ang+anga )*fj;
        sunrays[i*4+1].y = sin( ang+anga )*fj;
        sunrays[i*4+2].x = cos( ang+anga )*fi;
        sunrays[i*4+2].y = sin( ang+anga )*fi;
        sunrays[i*4+3].x = cos( ang )*fi;
        sunrays[i*4+3].y = sin( ang )*fi;
        
        ang += anga*2.0f;
      }
      
      break;
    
    case PART_FLOWERGROW:
      flwr[0].sx   = (sw*8)/10;
      flwr[0].sy   = (sh*8)/10;
      flwr[0].stkp = -20;
      flwr[0].r    = 0;
      flwr[0].tr   = sw/15;
      flwr[0].dr   = (flwr[0].tr<<8)/(TARGET_FPS*2/3);
      flwr[0].sc   = 0.0f;
      
      flwr[1].sx   = (sw*5)/10;
      flwr[1].sy   = (sh*13)/14;
      flwr[1].stkp = 0;
      flwr[1].r    = 0;
      flwr[1].tr   = sw/19;
      flwr[1].dr   = (flwr[1].tr<<8)/(TARGET_FPS*2/3);
      flwr[1].sc   = 0.0f;

      for( i=0; i<8; i++ )
      {
        flwr[0].p[i].attached = 1;
        flwr[0].p[i].wobble   = 0;
        flwr[0].p[i].cangrow  = 1;
        flwr[0].p[i].clk      = ACTION_TUNEPETAL_1+i;
        flwr[0].p[i].wba      = 3.14159265f;
        flwr[0].p[i].wbs      = 0.8f;
        flwr[0].p[i].scx      = -i;
        flwr[0].p[i].scy      = -i;

        flwr[1].p[i].attached = 1;
        flwr[1].p[i].wobble   = 0;
        flwr[1].p[i].cangrow  = 1;
        flwr[1].p[i].clk      = ACTION_TEXTPETAL_1+i;
        flwr[1].p[i].wba      = 3.14159265f;
        flwr[1].p[i].wbs      = 0.8f;
        flwr[1].p[i].scx      = -i;
        flwr[1].p[i].scy      = -i;
      }
      break;
    
    case PART_MAIN:
      tbwob = 0.0f;
      clicked = 0;
#ifndef __NO_MOUSE__
      SDL_ShowCursor( SDL_ENABLE );
#endif
      if( sw >= 320 )
        fade_in( "click the petals! then the sun when you're done!", 1 );
      else
        fade_in( "click the petals! the sun quits", 1 );
      show_now_playing( 0 );
      fader = 0;
      fadeadd = TARGET_FPS/45;
      
      tbx = midx - tbw;
      if( tbx < 0 ) tbx = sw/16;
      tby = sh/8;
      tbon = 0;
      tbclos = 0;
      tbadd = 1.0f / TARGET_FPS;
      
      bfbnc = (float)sh/48;
      deaddrop = (midy/TARGET_FPS)<<8;
      
      for( i=0; i<NUMBBFLYS; i++ )
      {
        reset_bbfly( i );
        bbflys[i].wscale = 1.0f;
        bbflys[i].wscd = -2.0f / TARGET_FPS;
      }
      
      for( i=0; i<NUMBFLYS; i++ )
      {
        reset_bfly( i );
        bflys[i].wscale = 1.0f;
        bflys[i].wscd = -2.0f / TARGET_FPS;
      }
      
      circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
      for( i=0; i<NUMCLOUDS; i++ )
        reset_cloud( i );
      sort_clouds_by_speed();
      
      notxttime=0;
      break;
    
    case PART_SUNSET:
      sunty = sh+sunr;
      gcount = 256;
      break;
  }
  
  part = n;
}

int render_init( void )
{
  int i, r, g, b, rd, gd, bd;

  rs   = screen->pitch/BYTESPERPIXEL;
  se   = sw*sh;
  midx = sw/2;
  midy = sh/2;
  bsw  = sw*BYTESPERPIXEL;
  sxe  = sw-1;
  sye  = sh-1;
  bss  = rs*sh*BYTESPERPIXEL;
  bse  = se*BYTESPERPIXEL;
  qsh  = sh/4;

  r = 45<<8;
  g = 225<<8;
  b = 245<<8;
  
  rd = r/16;
  gd = g/16;
  bd = b/16;

  for( i=4; i<20; i++, r-=rd, g-=gd, b-=bd )
    skytxtpal[i] = SDL_MapRGB( screen->format, r>>8, g>>8, b>>8 );
  
  r = 255<<8;
  g = 255<<8;
  b = 255<<8;

  rd = ((17<<8)-r)/14;
  gd = ((34<<8)-g)/14;
  bd = ((51<<8)-b)/14;
  for( i=1; i<15; i++, r+=rd, g+=gd, b+=bd )
    logopal[i] = SDL_MapRGB( screen->format, r>>8, g>>8, b>>8 );
  logopal[15] = SDL_MapRGB( screen->format, 0, 0, 0 );

  layers = malloc( sw * sh * 2 );
  if( !layers )
  {
    printf( "Out of memory\n" );
    return 0;
  }

  bg = malloc( bss );
  if( !bg )
  {
    printf( "Out of memory\n" );
    return 0;
  }
  
  skyrgb = malloc( sh * 3 );
  if( !skyrgb )
  {
    printf( "Out of memory\n" );
    return 0;
  }
  
  click = &layers[se];
  
  create_bg();
  
  for( i=0; i<PAL_ENTRIES; i++ )
    pal[i] = SDL_MapRGB( screen->format, spal[i*3], spal[i*3+1], spal[i*3+2] );
  
  for( i=0; i<32768; i++ )
    isqrt[i] = (int)sqrt( (float)i );
  
  for( i=0; i<MAXFPETS; i++ )
    fpt[i].active = 0;

  start_part( 1 );

  return 1;
}

void render_shut( void )
{
  if( layers ) free( layers );
  if( bg ) free( bg );
  if( skyrgb ) free( skyrgb );
}

float get_to( float current, float dest, float speed )
{
  if( current == dest ) return current;
  if( current > dest )
  {
    current -= speed;
    if( current < dest )
      current = dest;
    return current;
  }
  current += speed;
  if( current > dest )
    current = dest;
  return current;
}

void naturetiming( void )
{
  int i, j;

  j = 0;
  for( i=0; i<NUMCLOUDS; i++ )
  {
    clouds[i].ang -= clouds[i].speed;
    if( clouds[i].ang < -(3.14159265f/2.0f) )
    {
      reset_cloud( i );
      j = 1;
    }
  }
  
  if( j ) sort_clouds_by_speed();

  for( i=0; i<NUMBBFLYS; i++ )
  {
    if( (bbflys[i].sx>>8) > bbflys[i].tsx )
    {
      bbflys[i].sx += bbflys[i].dsx;
      if( (bbflys[i].sx>>8) <= bbflys[i].tsx )
      {
        reset_bbfly( i );
        continue;
      }
    } else {
      bbflys[i].sx += bbflys[i].dsx;
      if( (bbflys[i].sx>>8) >= bbflys[i].tsx )
      {
        reset_bbfly( i );
        continue;
      }
    }
    bbflys[i].sy += bbflys[i].dsy;
  
    if( bbflys[i].wscd > 0.0f )
    {
      if( bbflys[i].wscale < 1.0f )
        bbflys[i].wscale += bbflys[i].wscd;
      else
        bbflys[i].wscd = -bbflys[i].wscd;
    } else {
      if( bbflys[i].wscale > 0.5f )
        bbflys[i].wscale += bbflys[i].wscd;
      else
        bbflys[i].wscd = -bbflys[i].wscd;
    }
  }  

  for( i=0; i<NUMBFLYS; i++ )
  {
    if( bflys[i].dead )
    {
      if( bflys[i].ang > 0.0f )
        bflys[i].ang += 3.1f/TARGET_FPS;
      else
        bflys[i].ang -= 3.1f/TARGET_FPS;

      if( (bflys[i].sy>>8) < sh+midy )
        bflys[i].sy += deaddrop;
      else
        reset_bfly( i );
      continue;
    }
    if( (bflys[i].sx>>8) > bflys[i].tsx )
    {
      bflys[i].sx += bflys[i].dsx;
      if( (bflys[i].sx>>8) <= bflys[i].tsx )
      {
        reset_bfly( i );
        continue;
      }
    } else {
      bflys[i].sx += bflys[i].dsx;
      if( (bflys[i].sx>>8) >= bflys[i].tsx )
      {
        reset_bfly( i );
        continue;
      }
    }
    bflys[i].sy += bflys[i].dsy;
  
    if( bflys[i].wscd > 0.0f )
    {
      if( bflys[i].wscale < 1.0f )
        bflys[i].wscale += bflys[i].wscd;
      else
        bflys[i].wscd = -bflys[i].wscd;
    } else {
      if( bflys[i].wscale > 0.5f )
        bflys[i].wscale += bflys[i].wscd;
      else
        bflys[i].wscd = -bflys[i].wscd;
    }
  }  
}

void intro_timing( void )
{
  int tunepos, i, j;

  if( !tune ) return; // WTF?!
  if( intropart > 2 ) return;

  tunepos = tune->ht_PosNr*64+tune->ht_NoteNr;
  j = introdat[intropart*4+1];

  switch( introdoing )
  {
    case 0:  
      if( tunepos < 64 ) break;

      for( i=0; i<j; i++ )
      {
        if( introappear[i] < 24 ) introappear[i]++;
        if( ( introappear[i] > 0 ) && ( introzoom[i] > introzoomtarg ) )
        {
          introzoom[i] -= introzoomsub;
          if( introzoom[i] < introzoomtarg ) introzoom[i] = introzoomtarg;
        }
      }
      
      if( introappear[j-1] == 24 )
      {
        for( i=0; i<j; i++ )
          introappear[i] = -i*2;
        introdoing = 1;
      }
      break;
    
    case 1:
      for( i=0; i<j; i++ )
        if( introappear[i] < 24 )
        {
          introappear[i]+=2;
          if( introappear[i] > 24 ) introappear[i] = 24;
        }
      if( tunepos >= introdat[intropart*4+3] )
        introdoing = 2;
      break;
    
    case 2:
      for( i=0; i<j; i++ )
      {
        if( introappear[i] > -1 )
        {
          introappear[i]--;
          introzoom[i] -= introzoomsub;
          if( introzoom[i] < 0.02f ) introzoom[i] = 0.02f;
        }
        if( introappear[i] > 24-(TARGET_FPS/15) ) break;
      }
      
      if( introappear[j-1] == -1 )
      {
        intropart++;
        introdoing = 0;
        if( intropart > 2 ) break;

        j = introdat[intropart*4+1];
        for( i=0; i<j; i++ )
        {
          introappear[i] = -i * TARGET_FPS / 15;
          introzoom[i] = sh<400?3.0f:6.0f;
        }
      }
      break;
  }
}

#ifdef __BROKEN_SDL_TIMER__
Uint32 now=0;
Uint32 next=0;
#endif

Uint32 timing( Uint32 interval, void *dummy )
{
  SDL_Event     event;
  SDL_UserEvent userevent;
  int i, j;
  
#ifdef __BROKEN_SDL_TIMER__
  if (next != 0)
  {
    now = SDL_GetTicks();
    if (now < next)
      SDL_Delay(next-now);
  }
  next = SDL_GetTicks() + interval;
#endif

  if( paused ) return interval;
  
  if( voltimer > 0 ) voltimer--;
  
  userevent.type  = SDL_USEREVENT;
  userevent.code  = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;
  
  event.type = SDL_USEREVENT;
  event.user = userevent;
  
  SDL_PushEvent( &event );
  
  switch( part )
  {
    case PART_STARFIELD:
      intro_timing();

      for( i=0; i<NUMSTARS; i++ )
      {
        stars[i].sx -= stars[i].speed;
        if( (stars[i].sx>>8) < -(sw/5) )
        {
          stars[i].sx = (sw+(sw/5))<<8;
          stars[i].sy = rand()%sh;
        }
        
        stars[i].ang += 2.4f/TARGET_FPS;
      }
      break;

    case PART_PLANET_IN:
      intro_timing();

      if( (plx>>8) > ptrg )
      {
        plx += pldx;
        plr += pldr;
      }

      for( i=0; i<NUMSTARS; i++ )
      {
        stars[i].sx -= stars[i].speed;
        if( (stars[i].sx>>8) < -(sw/5) )
        {
          stars[i].sx = (sw+(sw/5))<<8;
          stars[i].sy = rand()%sh;
        }
        
        stars[i].ang += 2.4f/TARGET_FPS;
      }
      break;
    
    case PART_PLANET_ZOOM:
      intro_timing();

      if( (ply>>8) < ptrg )
      {
        plx += pldx;
        ply += pldy;
        plr += pldr;
      }

      for( i=0; i<NUMSTARS; i++ )
      {
        stars[i].sx -= stars[i].speed;
/*
        if( (stars[i].sx>>8) < -(sw/5) )
        {
          stars[i].sx = (sw+(sw/5))<<8;
          stars[i].sy = rand()%sh;
        }
*/      
        stars[i].ang += 2.4f/TARGET_FPS;
      }
      break;
    
    case PART_STARS_OUT:
      intro_timing();

      for( i=0; i<NUMSTARS; i++ )
      {
        stars[i].sx -= stars[i].speed * 2;
        stars[i].ang += 2.4f/TARGET_FPS;
      }
      break;
    
    case PART_SUNRISE:
      sunang -= 1.8f/TARGET_FPS;

      if( (suny>>8) > sunty )
        suny += sundy;
      
      if( gcount < 256 )
      {
        gcount += 2;
        pl_r += pl_rd;
        pl_g += pl_gd;
        pal[1] = SDL_MapRGB( screen->format, pl_r>>8, pl_g>>8, pl_r>>8 );
        spal[3] = pl_r>>8;
        spal[4] = pl_g>>8;
        spal[5] = pl_r>>8;
      }
      
      break;
    
    case PART_FLOWERGROW:
      sunang -= 1.8f/TARGET_FPS;
      fla1   += 1.2f/TARGET_FPS;
      fla2   -= 1.35f/TARGET_FPS;
      
      for( i=0; i<2; i++ )
      {
        if( flwr[i].stkp < (18*(TARGET_FPS/15)) )
        {
          flwr[i].stkp++;
        } else {
          if( (flwr[i].r>>8) < flwr[i].tr ) flwr[i].r += flwr[i].dr;
        }
        
        if( (flwr[i].r>>8) < flwr[i].tr )
          continue;

        for( j=0; j<8; j++ )
        {

          if( flwr[i].p[j].cangrow )
          {
            if( flwr[i].p[j].scy < 1.0f )
            {
              flwr[i].p[j].scx += 7.5f/TARGET_FPS;
              flwr[i].p[j].scy += 7.5f/TARGET_FPS;
            } else {
              flwr[i].p[j].cangrow = 0;
              flwr[i].p[j].wobble  = 1;
              flwr[i].p[j].wbs     = 0.8f;
              flwr[i].p[j].wba     = 3.14159265f;
            }
            continue;
          }
          
          if( flwr[i].p[j].wobble )
          {
            flwr[i].p[j].wbs -= 0.40f/TARGET_FPS;
            if( flwr[i].p[j].wbs < 0.0f )
            {
              flwr[i].p[j].wbs = 0.0f;
              flwr[i].p[j].wobble = 0;
            }
            flwr[i].p[j].wba += 8.5f/TARGET_FPS;
          }
        }
      }

      if(( flwr[0].stkp > (TARGET_FPS/15) ) && ( flwr[0].sc < 1.0f ))  flwr[0].sc += 0.015f;
      if(( flwr[1].stkp > (TARGET_FPS/15) ) && ( flwr[1].sc < 0.68f )) flwr[1].sc += 0.01f;
      break;
    
    case PART_MAIN:
      tbwob += 4.8f/TARGET_FPS;
      sunang -= 1.8f/TARGET_FPS;
      fla1   += 1.2f/TARGET_FPS;
      fla2   -= 1.35f/TARGET_FPS;
      
      fader += fadeadd;
      
      naturetiming();

      if( antxtptr )
      {
        notxttime = 0;
      } else {
        notxttime++;
        if( notxttime >= TARGET_FPS * 10 )
        {
          fade_in( top_blah[tbtxtnum++], 0 );
          if( top_blah[tbtxtnum] == NULL )
            tbtxtnum = 0;
          notxttime = 0;
        }
      }
      
      if( tbon )
      {
        if( !tbclos )
        {
          if( tbsc < 1.0f )
            tbsc += tbadd;
          else
            tbsc = 1.0f;
        } else {
          if( tbsc > 0.0f )
          {
            tbsc -= tbadd;
          } else {
            tbsc = 0.0f;
            tbon = 0;
          }
        }
      }

      for( i=0; i<MAXFPETS; i++ )
      {
        fpt[i].angle -= 1.8f/TARGET_FPS;

        fpt[i].scx = get_to( fpt[i].scx, 1.0f, 1.8f/TARGET_FPS );
        fpt[i].scy = get_to( fpt[i].scy, 0.2f, 1.8f/TARGET_FPS );

        if( fpt[i].active )
        {
          fpt[i].fa += 2.4f/TARGET_FPS;
          if( (fpt[i].sy>>8) < fpt[i].tsy )
          {
            fpt[i].sy += (sh<<5)/TARGET_FPS;
          } else {
            fpt[i].copytoback = 1;
            fpt[i].active = 0;
          }
        }
      }

      for( i=0; i<2; i++ )
      {
        for( j=0; j<8; j++ )
        {
          if( !flwr[i].p[j].attached )
            continue;

          if( flwr[i].p[j].cangrow )
          {
            if( flwr[i].p[j].scy < 1.0f )
            {
              flwr[i].p[j].scx += 7.5f/TARGET_FPS;
              flwr[i].p[j].scy += 7.5f/TARGET_FPS;
            } else {
              flwr[i].p[j].cangrow = 0;
              flwr[i].p[j].wobble  = 1;
              flwr[i].p[j].wbs     = 0.8f;
              flwr[i].p[j].wba     = 3.14159265f;
            }
            continue;
          }
          
          if( flwr[i].p[j].wobble )
          {
            flwr[i].p[j].wbs -= 0.30f/TARGET_FPS;
            if( flwr[i].p[j].wbs < 0.0f )
            {
              flwr[i].p[j].wbs = 0.0f;
              flwr[i].p[j].wobble = 0;
            }
            flwr[i].p[j].wba += 7.5f/TARGET_FPS;
          }
        }
      }
      break;

    case PART_SUNSET:
      
      sunang -= 1.8f/TARGET_FPS;
      fla1   += 1.2f/TARGET_FPS;
      fla2   -= 1.35f/TARGET_FPS;

      naturetiming();

      if( tbon )
      {
        if( tbsc > 0.0f )
        {
          tbsc -= tbadd * 2;
        } else {
          tbsc = 0.0f;
          tbon = 0;
        }
        break;
      }

      if( gcount > 0 ) gcount-=2;

      if( (suny>>8) < sunty )
        suny -= sundy;      
      break;
  }
  
  return interval;
}

void render_sun( void )
{
  int i;
  float x, y, ca, sa;
  
  ca = sin( sunang );
  sa = cos( sunang );

  for( i=0; i<NUMRAYS*4; i++ )
  {
    x = sunrays[i].x*ca - sunrays[i].y*sa;
    y = sunrays[i].x*sa + sunrays[i].y*ca;
    
    sunrays[i].sx = x+sunx;
    sunrays[i].sy = y+(suny>>8);
  }
  
  for( i=0; i<NUMRAYS*4; i+=4 )
  {
    ph_poly( &sunrays[i], &sunrays[i+1], &sunrays[i+2], pal[2], 1, 19 );
    ph_poly( &sunrays[i], &sunrays[i+2], &sunrays[i+3], pal[2], 1, 19 );
  }

  circle( sunx, suny>>8, sunr, pal[2], 1, ACTION_START_SUNSET );
}

void nextauthnote( void )
{
  int i, j, k, l;
  
  if( !antxtptr ) return;
  
  i=0;
  while( antxtptr[i] == 32 ) i++;
  if( antxtptr[i] == 0 )
  {
    antxtptr = NULL;
    return;
  }
  
  j=0;
  for( ;; )
  {
    k = j;
    l = i;
    
    while( antxtptr[i] == 32 ) i++;
    
    if( j ) antxt[j++] = 32;
    while( ( antxtptr[i] != 32 ) && ( antxtptr[i] != 0 ) )
      antxt[j++] = antxtptr[i++];
    
    antxt[j] = 0;
    if( textlen( antxt ) > sw )
    {
      if( k )
        antxt[k] = 0;
      else
        l = i;
      
      if( antxtptr[l] == 0 )
        antxtptr = NULL;
      else
        antxtptr = &antxtptr[l+1];
      break;
    }
    
    if( antxtptr[i] == 0 )
    {
      if( antxtptr == &nptxt[256] )
        antxtptr = authnotes[ctune];
      else
        antxtptr = NULL;
      break;
    }    
  }
    
  fade_in( antxt, 0 );
}

void render_intro_text( int which )
{
  int i, j, k, l;

  l = introdat[which*4+1];
  for( i=0, j=introdat[which*4], k=introdat[which*4+2]; i<l; i++, j++ )
  {
    if( llorder[j] != -1 )
    {
      if( ( introappear[i] > -1 ) || ( introdoing == 1 ) ) logoletter( k, midy-10, introappear[i]>=0?(introappear[i]/4):6, introzoom[i], llorder[j] );
      k += llorder[j]==0?is1:is2;
    } else {
      k += 10;
    }
  }
}

void user_interaction( int i )
{
  int j;

  switch( i )
  {
    case ACTION_TUNEPETAL_1:
    case ACTION_TUNEPETAL_2:
    case ACTION_TUNEPETAL_3:
    case ACTION_TUNEPETAL_4:
    case ACTION_TUNEPETAL_5:
    case ACTION_TUNEPETAL_6:
    case ACTION_TUNEPETAL_7:
    case ACTION_TUNEPETAL_8:
      i -= ACTION_TUNEPETAL_1;

      fpt[nextfpet].angle = flwr[0].p[i].angle;
      fpt[nextfpet].fa    = 0.0f;
      fpt[nextfpet].scx   = flwr[0].p[i].scx * flwr[0].sc;
      fpt[nextfpet].scy   = flwr[0].p[i].scy * flwr[0].sc;
      fpt[nextfpet].sx    = flwr[0].p[i].sx;
      fpt[nextfpet].sy    = flwr[0].p[i].sy<<8;
      fpt[nextfpet].tsy   = ((sh*9)/10) + (rand()%(sh/10));

      fpt[nextfpet].active = 1;
      nextfpet = (nextfpet+1)&31;

      flwr[0].p[i].attached = 0;
      if( i != ctune ) start_tune( i );
      show_now_playing( 1 );
      break;

    case ACTION_TEXTPETAL_1:
    case ACTION_TEXTPETAL_2:
    case ACTION_TEXTPETAL_3:
    case ACTION_TEXTPETAL_4:
    case ACTION_TEXTPETAL_5:
    case ACTION_TEXTPETAL_6:
    case ACTION_TEXTPETAL_7:
    case ACTION_TEXTPETAL_8:
      i-=9;

      fpt[nextfpet].angle = flwr[1].p[i].angle;
      fpt[nextfpet].fa    = 0.0f;
      fpt[nextfpet].scx   = flwr[1].p[i].scx * flwr[1].sc;
      fpt[nextfpet].scy   = flwr[1].p[i].scy * flwr[1].sc;
      fpt[nextfpet].sx    = flwr[1].p[i].sx;
      fpt[nextfpet].sy    = flwr[1].p[i].sy<<8;
      fpt[nextfpet].tsy   = ((sh*9)/10) + (rand()%(sh/10));

      fpt[nextfpet].active = 1;
      nextfpet = (nextfpet+1)&31;

      flwr[1].p[i].attached = 0;

      textbox_layout( othertexts[i] );
      tbon = 1;
      tbclos = 0;
      break;
    
    case ACTION_RESET_TUNEFLOWER:
      j=0;
      for( i=0; i<8; i++ )
      {
        if( flwr[0].p[i].attached == 0 )
        {
          flwr[0].p[i].wba      = 3.14159265f;
          flwr[0].p[i].wbs      = 0.8f;
          flwr[0].p[i].scx      = j;
          flwr[0].p[i].scy      = j;
          flwr[0].p[i].wobble   = 0;
          flwr[0].p[i].cangrow  = 1;
          flwr[0].p[i].attached = 1;
          j--;
        }
      }
      break;

    case ACTION_RESET_TEXTFLOWER:
      j=0;
      for( i=0; i<8; i++ )
      {
        if( flwr[1].p[i].attached == 0 )
        {
          flwr[1].p[i].wba      = 3.14159265f;
          flwr[1].p[i].wbs      = 0.8f;
          flwr[1].p[i].scx      = j;
          flwr[1].p[i].scy      = j;
          flwr[1].p[i].wobble   = 0;
          flwr[1].p[i].cangrow  = 1;
          flwr[1].p[i].attached = 1;
          j--;
        }
      }
      break;
    
    case ACTION_START_SUNSET:
      start_part( PART_SUNSET );
      break;
    
    case ACTION_DRAG_SCROLLBAR:  // Requires pointing device
      dragger = 1;
      dragy = clicky;
      break;
    
    case ACTION_PRESS_X:
      if( ( !tbon ) || ( tbsc < 1.0f ) )
        break;
      
      tbpressed = 1;
      break;
    
    case ACTION_DRAG_SCROLLBOX:   // Requires pointing device
      wdragger = 1;
      dragx = clickx;
      dragy = clicky;
      wdxo = tbx - clickx;
      wdyo = tby - clicky;
      break;
    
    case ACTION_STOPSCROLL:
      if( ( tbpressed != 2 ) &&
          ( tbpressed != 3 ) )
        break;
      
      tbpressed = 0;
      break;
    
    case ACTION_SCROLL_UP:
      tbpressed = 2;
      break;

    case ACTION_SCROLL_DOWN:
      tbpressed = 3;
      break;

    case ACTION_CLOUD_CLICK+2:
      if( ctune != 8 ) start_tune( 8 );
      show_now_playing( 1 );
      break;
    
    default:
      if( ( i >= ACTION_KILL_BUTTERFLY ) &&
          ( i < (ACTION_KILL_BUTTERFLY+NUMBFLYS) ) )
        bflys[i-ACTION_KILL_BUTTERFLY].dead = 1;
      break;              
  }
  
}

int render( void )
{
  int i, j;
  int finished;
  float scale;
  
  finished = 0;

  memset( layers, 0, se*2 );
  
  if( SDL_MUSTLOCK( screen ) )
  {
    if( SDL_LockSurface( screen ) < 0 )
      return finished;
  }
  
  memcpy( screen->pixels, bg, bss );

  switch( part )
  {
    case PART_STARFIELD:
      for( i=0; i<NUMSTARS; i++ )
        draw_star( stars[i].scale, stars[i].ang, stars[i].sx>>8, stars[i].sy, pal[2], 1 );
      render_edges();
      render_intro_text( intropart );
      if( voltimer > 0 ) draw_volume();

      if( tune->ht_PosNr >= 6 )
        start_part( PART_PLANET_IN );
      break;

    case PART_PLANET_IN:
      for( i=0; i<NUMSTARS; i++ )
        draw_star( stars[i].scale, stars[i].ang, stars[i].sx>>8, stars[i].sy, pal[2], 1 );
      circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
      render_edges();
      render_intro_text( intropart );
      if( voltimer > 0 ) draw_volume();

      if( (plx>>8) <= ptrg )
        start_part( PART_PLANET_ZOOM );
      break;
    
    case PART_PLANET_ZOOM:
      for( i=0; i<NUMSTARS; i++ )
        draw_star( stars[i].scale, stars[i].ang, stars[i].sx>>8, stars[i].sy, pal[2], 1 );
      circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
      render_edges();
      render_intro_text( intropart );
      if( voltimer > 0 ) draw_volume();

      if( (ply>>8) >= ptrg )
        start_part( PART_STARS_OUT );
      break;
    
    case PART_STARS_OUT:
      for( i=0; i<NUMSTARS; i++ )
        draw_star( stars[i].scale, stars[i].ang, stars[i].sx>>8, stars[i].sy, pal[2], 1 );
      circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
      render_edges();
      render_intro_text( intropart );
      if( voltimer > 0 ) draw_volume();

      j=1;
      for( i=0; i<NUMSTARS; i++ )
        if( (stars[i].sx>>8) > -starmax )
          j=0;
      
      if( j )
        start_part( PART_SUNRISE );
      break;
    
    case PART_SUNRISE:
      if( (suny>>8)<=sunty )
      {
        sunrise( gcount );
        circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
        render_edges();
        // Don't need to keep drawing the planet
        memcpy( bg, screen->pixels, bss );
        render_sun();
        render_edges();
        if( voltimer > 0 ) draw_volume();
        start_part( PART_FLOWERGROW );
        break;
      }
    
      sunrise( gcount );
      render_sun();
      circle( plx>>8, ply>>8, plr>>8, pal[1], 2, 0 );
      render_edges();
      if( voltimer > 0 ) draw_volume();
      break;
    
    case PART_FLOWERGROW:
      render_sun();
      if( flwr[0].stkp > 0 )
      {
        draw_stalk( sin( fla1 )*0.1f, flwr[0].sc, flwr[0].sx, flwr[0].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
        if( flwr[0].r > 255 )
          draw_flower( &flwr[0], stalktopx, stalktopy, 2, 0, pal[4], pal[2] );
      }
      if( flwr[1].stkp > 0 )
      {
        draw_stalk( sin( fla2 )*0.08f, flwr[1].sc, flwr[1].sx, flwr[1].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
        if( flwr[1].r > 255 )
          draw_flower( &flwr[1], stalktopx, stalktopy, 2, 0, pal[4], pal[2] );
      }

      if( flwr[0].p[7].cangrow == 0 )
        start_part( PART_MAIN );
      render_edges();
      if( voltimer > 0 ) draw_volume();
      break;
    
    case PART_MAIN:
      render_sun();

      for( i=0; i<NUMCLOUDS; i++ )
      {
        scale = (clouds[i].ang + 1.5f) / 1.5f;
        draw_cloud( clouds[i].ang, plx>>8, ply>>8, clouds[i].r, scale, pal[4], 200+i, ACTION_CLOUD_CLICK+i );
      }

      if( do_fade( fader ) ) nextauthnote();
      fader = 0;

      for( i=0; i<NUMBBFLYS; i++ )
        draw_butterfly( bbflys[i].ang, bbflys[i].wscale, bbflys[i].sx>>8, (bbflys[i].sy>>8)-((int)(bbflys[i].wscale*bfbnc*0.6f)), 140+i, 160+i, 0, pal[22], pal[23], pal[24], 1 );

      if( tbon ) render_scrollbox( tbsc, 5, ACTION_PRESS_X, 6, ACTION_DRAG_SCROLLBAR, ACTION_DRAG_SCROLLBOX );
      draw_stalk( sin( fla1 )*0.1f, flwr[0].sc, flwr[0].sx, flwr[0].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
      draw_flower( &flwr[0], stalktopx, stalktopy, 2, ACTION_RESET_TUNEFLOWER, pal[4], pal[2] );
      draw_stalk( sin( fla2 )*0.08f, flwr[1].sc, flwr[1].sx, flwr[1].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
      draw_flower( &flwr[1], stalktopx, stalktopy, 2, ACTION_RESET_TEXTFLOWER, pal[4], pal[2] );

      for( i=0; i<MAXFPETS; i++ )
      {
        if( fpt[i].copytoback )
        {
          fpt[i].copytoback = 0;
          if( ( fpt[i].m.maxx <= fpt[i].m.minx ) || ( fpt[i].m.maxy <= fpt[i].m.miny ) )
            continue;
          
          draw_petal( &fpt[i].m,
                      cos( fpt[i].angle ), sin( fpt[i].angle ),
                      fpt[i].scx, fpt[i].scy,
                      fpt[i].sx + (int)(sin( fpt[i].fa ) * (float)(sw/10)),  fpt[i].sy>>8,
                      pal[4],
                      3, 0 );

          fpt[i].m.minx -= 2;
          fpt[i].m.maxx += 2;
          fpt[i].m.miny -= 2;
          fpt[i].m.maxy += 2;

          if( fpt[i].m.minx < 0 ) fpt[i].m.minx = 0;
          if( fpt[i].m.maxx >= sw ) fpt[i].m.maxx = sw-1;
          if( fpt[i].m.miny < 0 ) fpt[i].m.miny = 0;
          if( fpt[i].m.maxy >= sh ) fpt[i].m.maxy = sh-1;
          
          render_edge_zone( &fpt[i].m );

          for( j=fpt[i].m.miny; j<=fpt[i].m.maxy; j++ )
            memcpy( &bg[j*rs+fpt[i].m.minx], &((PIXEL *)screen->pixels)[j*rs+fpt[i].m.minx], ((fpt[i].m.maxx-fpt[i].m.minx)+1)*BYTESPERPIXEL );
        }
      }

      for( i=0; i<MAXFPETS; i++ )
      {
        if( fpt[i].active )
        {
          draw_petal( &fpt[i].m,
                      cos( fpt[i].angle ), sin( fpt[i].angle ),
                      fpt[i].scx, fpt[i].scy,
                      fpt[i].sx + (int)(sin( fpt[i].fa ) * (float)(sw/10)),  fpt[i].sy>>8,
                      pal[4],
                      3, 0 );
        }
      }
      
      for( i=0; i<NUMBFLYS; i++ )
        draw_butterfly( bflys[i].ang, bflys[i].wscale, bflys[i].sx>>8, (bflys[i].sy>>8)-((int)(bflys[i].wscale*bfbnc)), 100+i, 120+i, ACTION_KILL_BUTTERFLY+i, pal[22], pal[23], pal[24], 0 );

      render_edges();
      if( voltimer > 0 ) draw_volume();
      
      if( tbpressed )
      {
        if( enddragger )
        {
          switch( tbpressed )
          {
            case 1:
              tbclos = 1;
              break;
          }
          
          tbpressed = 0;
        }
      }

      if( dragger )
      {
        if( enddragger )
          dragger = 0;
        else
          if( ( tbon ) && ( !tbclos ) && ( tbsc >= 1.0f ) ) textbox_drag( dragy );
      }
      
      if( wdragger )
      {
        if( enddragger )
        {
          wdragger = 0;
        } else {
          tbx = dragx + wdxo;
          tby = dragy + wdyo;
          if( tbx < 0 ) tbx = 0;
          if( tbx > (sw-tbw) ) tbx = sw-tbw;
          if( tby < fontheight ) tby = fontheight;
          if( tby > (((ply-plr)>>8)-tbh) ) tby = (((ply-plr)>>8)-tbh);
        }
      }
      
      enddragger = 0;

      if( clicked )
      {
        i = click[clicky*sw+clickx];
        
        if( ( i == 22 ) && ( clickx > tbx+tbbrd+tbaw ) )
        {
          if( clicky < tby+35 )
          {
            i = ACTION_SCROLL_UP;
          } else if( clicky >= (tby+tbh-35) ) {
            i = ACTION_SCROLL_DOWN;
          } else {
            i = ACTION_DRAG_SCROLLBAR;
          }
        }
        
        user_interaction( i );
        
        clicked = 0;
      }
      break;

    case PART_SUNSET:
      sunset( gcount );
      render_sun();
      for( i=0; i<NUMCLOUDS; i++ )
      {
        scale = (clouds[i].ang + 1.5f) / 1.5f;
        draw_cloud( clouds[i].ang, plx>>8, ply>>8, clouds[i].r, scale, pal[4], 200+i, ACTION_CLOUD_CLICK+i );
      }
      for( i=0; i<NUMBBFLYS; i++ )
        draw_butterfly( bbflys[i].ang, bbflys[i].wscale, bbflys[i].sx>>8, (bbflys[i].sy>>8)-((int)(bbflys[i].wscale*bfbnc*0.6f)), 140+i, 160+i, 0, pal[22], pal[23], pal[24], 1 );
      if( tbon ) render_scrollbox( tbsc, 5, ACTION_PRESS_X, 6, ACTION_DRAG_SCROLLBAR, ACTION_DRAG_SCROLLBOX );
      circle( plx>>8, ply>>8, plr>>8, pal[1], 128, 0 );
      draw_stalk( sin( fla1 )*0.1f, flwr[0].sc, flwr[0].sx, flwr[0].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
      draw_flower( &flwr[0], stalktopx, stalktopy, 2, ACTION_RESET_TUNEFLOWER, pal[4], pal[2] );
      draw_stalk( sin( fla2 )*0.08f, flwr[1].sc, flwr[1].sx, flwr[1].sy, pal[3], 1, 0, flwr[0].stkp/(TARGET_FPS/15) );
      draw_flower( &flwr[1], stalktopx, stalktopy, 2, ACTION_RESET_TUNEFLOWER, pal[4], pal[2] );      

      for( i=0; i<NUMBFLYS; i++ )
        draw_butterfly( bflys[i].ang, bflys[i].wscale, bflys[i].sx>>8, (bflys[i].sy>>8)-((int)(bflys[i].wscale*bfbnc)), 100+i, 120+i, ACTION_KILL_BUTTERFLY+i, pal[22], pal[23], pal[24], 0 );

      render_edges();
      if( voltimer > 0 ) draw_volume();
      
      if( gcount <= 0 )
        finished = 1;
      break;
  }
  
  if( SDL_MUSTLOCK( screen ) )
    SDL_UnlockSurface( screen );
  
  SDL_Flip( screen );
  
  return finished;
}

