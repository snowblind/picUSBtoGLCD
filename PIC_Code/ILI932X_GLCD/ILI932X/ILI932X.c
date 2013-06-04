/////////////////////////////////////////////////////////////////////////
////                           ILI932X.c                             ////
/////////////////////////////////////////////////////////////////////////
////            Driver for ILI932X controller based GLCDs            ////
////                                                                 ////
////           https://github.com/snowblind/picUSBtoGLCD/            ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////                       Copyright                                 ////
/////////////////////////////////////////////////////////////////////////
////  MIT license                                                    ////
////                                                                 ////
////                                                                 ////
////  Code coverted to CCS C By Michael J. Pounders                  ////
////                                                                 ////
////                                                                 ////            
////  Based on code by ladyada/adafruit with init code from Rossum   ////
////      https://github.com/adafruit/TFTLCD-Library/                ////   
////      https://github.com/adafruit/Adafruit-GFX-Library/          ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                                                                 ////
////  glcd_init()                                                    ////
////     * Must be called before any other function.                 ////
////                                                                 ////
////                                                                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////

//// TFTFAST
//// Define TFTFAST if you want speed and have enough program memory 
/////////////////////////////////////////////////////////////////////////
#ifdef TFTFAST
   #warning "Using TFT fast mode"
   #inline
   void write8(int8 data);
   #inline
   void writeRegister16( unsigned int16 a, unsigned int16 d);
#else
   #warning "Using program memory saving mode #define TFTFAST if you have lots PGM on your PIC"
   void write8(int8 data);
   void writeRegister16( unsigned int16 a, unsigned int16 d);
#endif

void glcd_init( void );
void setupIO( void );
void initTftinfo( void );
void resetILI932x( void );
void initILI932x_regValues( void );

void ILI932x_setRotation( unsigned int8 x );

unsigned int8 read8( void ); 
void setAddrWindow( int16 x1, int16 y1, int16 x2, int16 y2 );
void setWriteDir( void );
void setReadDir( void );
unsigned int16 color565( unsigned int8 r, unsigned int8 g, unsigned int8 b );

void drawPixel( int16 x, int16 y, unsigned int16 color );
void flood( unsigned int16 color, unsigned int32 len );
void fillScreen( unsigned int16 color );
void fillRect( signed int16 x1, signed int16 y1, signed int16 w, signed int16 h, unsigned int16 fillcolor );




unsigned int8 getRotation(void);
void setRotation(unsigned int8 x);
int16 width(void);
int16 height(void);


#ifndef TFTTOUCH
#define TFTTOUCH 

#use fast_io(d)
#use fast_io(b)

#ifndef RD_PIN
   #bit RD_PIN = getenv( "SFR:PORTB" ).1
#endif

#ifndef WR_PIN
   #bit WR_PIN = getenv( "SFR:PORTB" ).2
#endif

#ifndef CD_PIN
   #bit CD_PIN = getenv( "SFR:PORTB" ).3
#endif

#ifndef CS_PIN
   #bit CS_PIN = getenv( "SFR:PORTB" ).4
#endif



#define RD_ACTIVE       RD_PIN = 0   // Control signals are ACTIVE LOW (idle is HIGH)
#define RD_IDLE         RD_PIN = 1    
#define WR_ACTIVE       WR_PIN = 0
#define WR_IDLE         WR_PIN = 1
#define CS_ACTIVE       CS_PIN = 0
#define CS_IDLE         CS_PIN = 1
#define CD_COMMAND      CD_PIN = 0  // Command/Data: LOW = command, HIGH = data
#define CD_DATA         CD_PIN = 1

#define RST_ACTIVE      output_high(PIN_A0);                                     //FIXME: Item 191
#define RST_INACTIVE    output_low(PIN_A0);                                      //FIXME: Item 191

#define RD_STROBE { RD_ACTIVE; RD_IDLE; }
#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

/////////////////////////////////////////////////////////////////////////
//Basic Colors
#define BLACK        0x0000
#define BLUE         0x001F
#define RED          0xF800
#define GREEN        0x07E0
#define YELLOW       0xFFE0
#define WHITE        0xFFFF

//Other Colors
#define CYAN         0x07FF
#define MAGENTA      0xF81F
#define BRIGHT_RED   0xF810   
#define GRAY1        0x8410  
#define GRAY2        0x4208  

//TFT resolution     320x240
#define MIN_X        0
#define MIN_Y        0
#define MAX_X        239
#define MAX_Y        319
#define TFTWIDTH     240
#define TFTHEIGHT    320
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//Register names from Peter Barrett's Microtouch code  
#define ILI932X_START_OSC          0x00
#define ILI932X_DRIV_OUT_CTRL      0x01
#define ILI932X_DRIV_WAV_CTRL      0x02
#define ILI932X_ENTRY_MOD          0x03
#define ILI932X_RESIZE_CTRL        0x04
#define ILI932X_DISP_CTRL1         0x07
#define ILI932X_DISP_CTRL2         0x08
#define ILI932X_DISP_CTRL3         0x09
#define ILI932X_DISP_CTRL4         0x0A
#define ILI932X_RGB_DISP_IF_CTRL1  0x0C
#define ILI932X_FRM_MARKER_POS     0x0D
#define ILI932X_RGB_DISP_IF_CTRL2  0x0F
#define ILI932X_POW_CTRL1          0x10
#define ILI932X_POW_CTRL2          0x11
#define ILI932X_POW_CTRL3          0x12
#define ILI932X_POW_CTRL4          0x13
#define ILI932X_GRAM_HOR_AD        0x20
#define ILI932X_GRAM_VER_AD        0x21
#define ILI932X_RW_GRAM            0x22
#define ILI932X_POW_CTRL7          0x29
#define ILI932X_FRM_RATE_COL_CTRL  0x2B
#define ILI932X_GAMMA_CTRL1        0x30
#define ILI932X_GAMMA_CTRL2        0x31
#define ILI932X_GAMMA_CTRL3        0x32
#define ILI932X_GAMMA_CTRL4        0x35
#define ILI932X_GAMMA_CTRL5        0x36
#define ILI932X_GAMMA_CTRL6        0x37
#define ILI932X_GAMMA_CTRL7        0x38
#define ILI932X_GAMMA_CTRL8        0x39
#define ILI932X_GAMMA_CTRL9        0x3C
#define ILI932X_GAMMA_CTRL10       0x3D
#define ILI932X_HOR_START_AD       0x50
#define ILI932X_HOR_END_AD         0x51
#define ILI932X_VER_START_AD       0x52
#define ILI932X_VER_END_AD         0x53
#define ILI932X_GATE_SCAN_CTRL1    0x60
#define ILI932X_GATE_SCAN_CTRL2    0x61
#define ILI932X_GATE_SCAN_CTRL3    0x6A
#define ILI932X_PART_IMG1_DISP_POS 0x80
#define ILI932X_PART_IMG1_START_AD 0x81
#define ILI932X_PART_IMG1_END_AD   0x82
#define ILI932X_PART_IMG2_DISP_POS 0x83
#define ILI932X_PART_IMG2_START_AD 0x84
#define ILI932X_PART_IMG2_END_AD   0x85
#define ILI932X_PANEL_IF_CTRL1     0x90
#define ILI932X_PANEL_IF_CTRL2     0x92
#define ILI932X_PANEL_IF_CTRL3     0x93
#define ILI932X_PANEL_IF_CTRL4     0x95
#define ILI932X_PANEL_IF_CTRL5     0x97
#define ILI932X_PANEL_IF_CTRL6     0x98

#define TFTLCD_DELAY               0xFF

static const unsigned int16 ILI932x_regValues[] = 
{
  ILI932X_START_OSC        , 0x0001, // Start oscillator
  TFTLCD_DELAY             , 50,     // 50 millisecond delay
  ILI932X_DRIV_OUT_CTRL    , 0x0100,
  ILI932X_DRIV_WAV_CTRL    , 0x0700,
  ILI932X_ENTRY_MOD        , 0x1030,
  ILI932X_RESIZE_CTRL      , 0x0000,
  ILI932X_DISP_CTRL2       , 0x0202,
  ILI932X_DISP_CTRL3       , 0x0000,
  ILI932X_DISP_CTRL4       , 0x0000,
  ILI932X_RGB_DISP_IF_CTRL1, 0x0000,
  ILI932X_FRM_MARKER_POS   , 0x0000,
  ILI932X_RGB_DISP_IF_CTRL2, 0x0000,
  ILI932X_POW_CTRL1        , 0x0000,
  ILI932X_POW_CTRL2        , 0x0007,
  ILI932X_POW_CTRL3        , 0x0000,
  ILI932X_POW_CTRL4        , 0x0000,
  TFTLCD_DELAY             , 200,
  ILI932X_POW_CTRL1        , 0x1690,
  ILI932X_POW_CTRL2        , 0x0227,
  TFTLCD_DELAY             , 50,
  ILI932X_POW_CTRL3        , 0x001A,
  TFTLCD_DELAY             , 50,
  ILI932X_POW_CTRL4        , 0x1800,
  ILI932X_POW_CTRL7        , 0x002A,
  TFTLCD_DELAY             , 50,
  ILI932X_GAMMA_CTRL1      , 0x0000,
  ILI932X_GAMMA_CTRL2      , 0x0000,
  ILI932X_GAMMA_CTRL3      , 0x0000,
  ILI932X_GAMMA_CTRL4      , 0x0206,
  ILI932X_GAMMA_CTRL5      , 0x0808,
  ILI932X_GAMMA_CTRL6      , 0x0007,
  ILI932X_GAMMA_CTRL7      , 0x0201,
  ILI932X_GAMMA_CTRL8      , 0x0000,
  ILI932X_GAMMA_CTRL9      , 0x0000,
  ILI932X_GAMMA_CTRL10     , 0x0000,
  ILI932X_GRAM_HOR_AD      , 0x0000,
  ILI932X_GRAM_VER_AD      , 0x0000,
  ILI932X_HOR_START_AD     , 0x0000,
  ILI932X_HOR_END_AD       , 0x00EF,
  ILI932X_VER_START_AD     , 0X0000,
  ILI932X_VER_END_AD       , 0x013F,
  ILI932X_GATE_SCAN_CTRL1  , 0xA700, // Driver Output Control (R60h)
  ILI932X_GATE_SCAN_CTRL2  , 0x0003, // Driver Output Control (R61h)
  ILI932X_GATE_SCAN_CTRL3  , 0x0000, // Driver Output Control (R62h)
  ILI932X_PANEL_IF_CTRL1   , 0X0010, // Panel Interface Control 1 (R90h)
  ILI932X_PANEL_IF_CTRL2   , 0X0000,
  ILI932X_PANEL_IF_CTRL3   , 0X0003,
  ILI932X_PANEL_IF_CTRL4   , 0X1100,
  ILI932X_PANEL_IF_CTRL5   , 0X0000,
  ILI932X_PANEL_IF_CTRL6   , 0X0000,
  ILI932X_DISP_CTRL1       , 0x0133, // Main screen turn on
};




static struct tftinfo_t
{
  signed int16  WIDTH, HEIGHT;               // this is the 'raw' display w/h - never changes
  unsigned int16  _width, _height;           // dependent on rotation
  int16  cursor_x, cursor_y;
  unsigned int16 textcolor, textbgcolor;
  unsigned int8  textsize;
  unsigned int8  rotation;
  int1  wrap;                                // If set, 'wrap' text at right edge of display
};


static struct tftinfo_t tftinfo;

void glcd_init( void )
{     
   setupIO();
   initTftinfo();
   
   setup_ccp1(CCP_PWM);
   setup_timer_2(T2_DIV_BY_1,128,1);   // kHz square wave on CCP1
   
   set_pwm1_duty( 64 );
   
   resetILI932x();
   debug_printf( "ICID: %LX \r\n", readID() );
   initILI932x_regValues();
}

void setupIO( void )
{

   CS_IDLE; // Set all control bits to idle state
   WR_IDLE;
   RD_IDLE;
   CD_DATA;

   RST_ACTIVE; // Reset line

   SET_TRIS_B( 0x00 );                                             //FIXME
   setWriteDir(); // Set up LCD data port for WRITE operations
    
}

void initTftinfo(void)
{ 
   tftinfo.wrap          = TRUE;
   tftinfo.rotation      = 0;
   tftinfo.textsize      = 1;
   tftinfo.textcolor     = BLACK;
   tftinfo.textbgcolor   = WHITE;
   tftinfo._width        = TFTWIDTH;
   tftinfo._height       = TFTHEIGHT;
   tftinfo.cursor_x      = 0;
   tftinfo.cursor_y      = 0;
}

void initILI932x_regValues( void )
{

  int8 i = 0;
  int16 a, d;
  
  CS_ACTIVE;
  
  while( i < sizeof(ILI932x_regValues) / sizeof(unsigned int16) ) 
  {
    a = ILI932x_regValues[i++];
    d = ILI932x_regValues[i++];
    if(a == TFTLCD_DELAY) delay_ms(d);
    else  writeRegister16(a, d);
  }
    setRotation(tftinfo.rotation);
    setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
    
  CS_IDLE;

  setRotation(0);
  setAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1); 
}

void resetILI932x(void)
{

  unsigned int8 i;
 
  CS_IDLE;
  CD_DATA;
  WR_IDLE;
  RD_IDLE;

  RST_INACTIVE;
  delay_ms(2);
  RST_ACTIVE;

  CS_ACTIVE;
  CD_DATA;
  write8(0x00);
  for( i=0; i<7; i++) 
      WR_STROBE;
  CS_IDLE;
delay_ms(100);
}


void setAddrWindow(int16 x1, int16 y1, int16 x2, int16 y2)
{

  CS_ACTIVE;
 
    // Values passed are in current (possibly rotated) coordinate
    // system.  932X requires hardware-native coords regardless of
    // MADCTL, so rotate inputs as needed.  The address counter is
    // set to the top-left corner -- although fill operations can be
    // done in any direction, the current screen rotation is applied
    // because some users find it disconcerting when a fill does not
    // occur top-to-bottom.
    int16 x, y, t;
    switch(tftinfo.rotation) 
    {
     case 1:
      t  = y1;
      y1 = x1;
      x1 = TFTWIDTH  - 1 - y2;
      y2 = x2;
      x2 = TFTWIDTH  - 1 - t;
      x  = x2;
      y  = y1;
      break;
     case 2:
      t  = x1;
      x1 = TFTWIDTH  - 1 - x2;
      x2 = TFTWIDTH  - 1 - t;
      t  = y1;
      y1 = TFTHEIGHT - 1 - y2;
      y2 = TFTHEIGHT - 1 - t;
      x  = x2;
      y  = y2;
      break;
     case 3:
      t  = x1;
      x1 = y1;
      y1 = TFTHEIGHT - 1 - x2;
      x2 = y2;
      y2 = TFTHEIGHT - 1 - t;
      x  = x1;
      y  = y2;
      break;
     default:
      x  = x1;
      y  = y1;
      break;    
     }
    writeRegister16(0x0050, x1); // Set address window
    writeRegister16(0x0051, x2);
    writeRegister16(0x0052, y1);
    writeRegister16(0x0053, y2);
    writeRegister16(0x0020, x ); // Set address counter to top left
    writeRegister16(0x0021, y );

  CS_IDLE;
}

unsigned int16 readID(void) 
{

  unsigned int16  id;

  CS_ACTIVE;
  CD_COMMAND;
  write8(0x00);
  WR_STROBE;         // Extra strobe because high, low bytes are the same
  setReadDir();      // Set up LCD data port(s) for READ operations
  CD_DATA;
  id   = read8();       
  id <<= 8;             
  delay_ms( 2 );        
  id  |= read8();       
  CS_IDLE;
  setWriteDir();     // Restore LCD data port(s) to WRITE configuration

  return id;
}


void write8( int8 data ) 
{
   output_d( data );
   WR_STROBE;
}

unsigned int8 read8(void) 
{
   RD_STROBE; 
   return input_d();
}


void writeRegister16( unsigned int16 a, unsigned int16 d)
{
  unsigned int8 hi, lo; 
  hi = (a) >> 8; lo = (a); CD_COMMAND; write8(hi); write8(lo); 
  hi = (d) >> 8; lo = (d); CD_DATA   ; write8(hi); write8(lo); 
}



unsigned int16 color565( unsigned int8 r, unsigned int8 g, unsigned int8 b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void setWriteDir(void)
{
   SET_TRIS_D( 0x00 );
}

void setReadDir(void) 
{
   SET_TRIS_D( 0xFF );
}


void drawPixel(int16 x, int16 y, unsigned int16 color)
{

  // Clip
  //if((x < 0) || (y < 0) || (x >= tftinfo._width) || (y >= tftinfo._height)) return;

  CS_ACTIVE;

    int16 t;
    switch(tftinfo.rotation) {
     case 1:
      t = x;
      x = TFTWIDTH  - 1 - y;
      y = t;
      break;
     case 2:
      x = TFTWIDTH  - 1 - x;
      y = TFTHEIGHT - 1 - y;
      break;
     case 3:
      t = x;
      x = y;
      y = TFTHEIGHT - 1 - t;
      break;
    }
    writeRegister16(0x0020, x);
    writeRegister16(0x0021, y);
    writeRegister16(0x0022, color);

   CS_IDLE;
}

void flood(unsigned int16 color, unsigned int32 len)
{
  unsigned int16 blocks;
  unsigned int8  i, hi = color >> 8, lo = color;

  CS_ACTIVE;
  CD_COMMAND;
  
  write8(0x00); // High byte of GRAM register...
  write8(0x22); // Write data to GRAM

  // Write first pixel normally, decrement counter by 1
  CD_DATA;
  write8(hi);
  write8(lo);
  len--;

  blocks = (unsigned int16)(len / 64); // 64 pixels/block
  if(hi == lo) 
  {
    // High and low bytes are identical.  Leave prior data
    // on the port(s) and just toggle the write strobe.
    while(blocks--) 
    {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do 
      {
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // 2 bytes/pixel
        WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // x 4 pixels
      } while(--i);
    }
    // Fill any remaining pixels (1 to 64)
    for(i = (unsigned int8)len & 63; i--; ) 
    {
      WR_STROBE;
      WR_STROBE;
    }
  } 
  else 
  {
    while(blocks--) 
    {
      i = 16; // 64 pixels/block / 4 pixels/pass
      do 
      {
        write8(hi); write8(lo); write8(hi); write8(lo);
        write8(hi); write8(lo); write8(hi); write8(lo);
      } while(--i);
    }
    for(i = (unsigned int8)len & 63; i--; ) 
    {
      write8(hi);
      write8(lo);
    }
  }
  CS_IDLE;
}

void fillScreen(unsigned int16 color)
{
  
    // For the 932X, a full-screen address window is already the default
    // state, just need to set the address pointer to the top-left corner.
    // Although we could fill in any direction, the code uses the current
    // screen rotation because some users find it disconcerting when a
    // fill does not occur top-to-bottom.
    unsigned int16 x, y;
    switch(tftinfo.rotation) 
    {
      default: x = 0            ; y = 0            ; break;
      case 1 : x = TFTWIDTH  - 1; y = 0            ; break;
      case 2 : x = TFTWIDTH  - 1; y = TFTHEIGHT - 1; break;
      case 3 : x = 0            ; y = TFTHEIGHT - 1; break;
    }
    CS_ACTIVE;
    writeRegister16(0x0020, x);
    writeRegister16(0x0021, y);

  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}



unsigned int8 getRotation(void) 
{
  tftinfo.rotation %= 4;
  return tftinfo.rotation;
}

void setRotation(unsigned int8 x) 
{
  x %= 4;  // cant be higher than 3
  tftinfo.rotation = x;
  switch(x) 
  {
      case 0:
      case 2:
         tftinfo._width = TFTWIDTH;
         tftinfo._height = TFTHEIGHT;
         break;
      case 1:
      case 3:
         tftinfo._width = TFTHEIGHT;
         tftinfo._height = TFTWIDTH;
      break;
  }
  ILI932X_setRotation(x); // Call ILI932X rotation code
}

void ILI932X_setRotation(unsigned int8 x)
{

  CS_ACTIVE;

    unsigned int16 t;
    switch(tftinfo.rotation) 
    {
     default: t = 0x1030; break;
     case 1 : t = 0x1028; break;
     case 2 : t = 0x1000; break;
     case 3 : t = 0x1018; break;
    }
    writeRegister16(0x0003, t ); // MADCTL
    // For 932X, init default full-screen address window:
    setAddrWindow(0, 0, tftinfo._width - 1, tftinfo._height - 1); // CS_IDLE happens here
  
}


int16 width(void) 
{
  return tftinfo._width; // return the size of the display which depends on the rotation!
}
 
int16 height(void) 
{
  return tftinfo._height; // return the size of the display which depends on the rotation!
}




#endif
