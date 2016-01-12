
struct vtx
{
  float x, y;
  int sx, sy;
  int f1, f2;
};

struct minmax
{
  int minx, maxx;
  int miny, maxy;
};

struct floating_petal
{
  int active;
  int copytoback;
  float angle;
  float fa;
  float scx, scy;
  int sx, sy, tsy;
  struct minmax m;
};

struct petal
{
  int attached;
  int wobble;
  int cangrow;
  Uint8 clk;
  float x, y;
  float scx, scy;
  float angle, ca, sa;
  float wba, wbs;
  int sx, sy;
};

struct flower
{
  int sx, sy;
  int stkp;
  int r, tr, dr;
  float sc;
  struct petal p[8];
};

struct star
{
  int sx, sy;
  float ang, scale;
  int speed;
};

struct butterfly
{
  int sx, sy, tsx, tsy, dsx, dsy;
  int dead;
  float ang, wscale, wscd;
};

struct cloud
{
  float ang, r;
  float speed;
};

void circle( int x, int y, int r, PIXEL col, Uint8 lyr, Uint8 clk );
void ph_poly( struct vtx *v1, struct vtx *v2, struct vtx *v3, PIXEL c, Uint8 lyr, Uint8 clk );
int shapes_init( void );
void shapes_shut( void );
void draw_stalk( float angle, float scale, int sx, int sy, PIXEL col, Uint8 lyr, Uint8 clk, int polys );
void draw_petal( struct minmax *m, float ca, float sa, float scx, float scy, int sx, int sy, PIXEL col, Uint8 lyr, Uint8 clk );
void draw_flower( struct flower *fl, int sx, int sy, Uint8 lyr, Uint8 clk, int col1, int col2 );
void draw_star( float scale, float angle, int sx, int sy, int col, Uint8 lyr );
void draw_butterfly( float angle, float wscale, int sx, int sy, Uint8 lyr1, Uint8 lyr2, Uint8 clk, PIXEL col1, PIXEL col2, PIXEL col3, int small );
void draw_cloud( float ang, int orx, int ory, float r, float scale, PIXEL col, Uint8 lyr, Uint8 clk );
void draw_volume( void );
