
int text_init( void );
void text_shut( void );
void textprint( int x, int y, char *str, PIXEL *wpal );
void fade_in( char *str, int immediate );
int do_fade( int fadeadd );
int textlen( char *str );
void render_scrollbox( float scale, Uint8 lyr, Uint8 clk, Uint8 slyr, Uint8 sclk, Uint8 dclk );
void render_textbox( float scale, Uint8 lyr, Uint8 clk, Uint8 slyr, Uint8 sclk, Uint8 dclk, float wobble );
void textbox_layout( char *str );
void textbox_drag( int y );
void logoletter( int x, int y, int frame, float zoom, int l );
