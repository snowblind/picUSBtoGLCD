//// TFTEXT
//// Define TFTTEXT before includeing gfxlib.c if you want to use the  
//// text funtions. The text functions would not be included if you didnt   
//// call them but the font file would be and it uses program memory that  
//// you may need.
/////////////////////////////////////////////////////////////////////////
#ifdef TFTTEXT 

   #include "glcdfont.c"

   #inline
   void glcd_putc(unsigned int8 c);
   void setTextWrap(boolean w);
   void setCursor(int16 x, int16 y);
   void setTextSize(unsigned int8 s);   
   void setTextColor(unsigned int16 c);
   void setTextColor(unsigned int16 c, unsigned int16 b);
   void drawChar( signed int16 x, signed int16 y, unsigned char c, unsigned int16 color, unsigned int16 bg, unsigned int8 size);

#endif

void drawLine( signed int16 x0, signed int16 y0, signed int16 x1, signed int16 y1, unsigned int16 color);

void drawCircle( signed int16 x0, signed int16 y0, signed int16 r, unsigned int16 color);

void drawRect( signed int16 x, signed int16 y, signed int16 w, signed int16 h, unsigned int16 color );
void fillRect( signed int16 x1, signed int16 y1, signed int16 w, signed int16 h, unsigned int16 fillcolor);
void drawFastVLine( signed int16 x, signed int16 y, signed int16 h, unsigned int16 color );
void drawFastHLine( signed int16 x, signed int16 y, signed int16 w, unsigned int16 color );

void drawTriangle( signed int16 x0, signed int16 y0,
            signed int16 x1, signed int16 y1, 
            signed int16 x2, signed int16 y2,
            unsigned int16 color);

void fillTriangle ( signed int16 x0, signed int16 y0,
                    signed int16 x1, signed int16 y1, 
                    signed int16 x2, signed int16 y2, 
                    unsigned int16 color);
                    
void drawRoundRect( signed int16 x, signed int16 y, signed int16 w,
                    signed int16 h, signed int16 r, unsigned int16 color);                    

void fillRoundRect( signed int16 x, signed int16 y, signed int16 w,
                    signed int16 h, signed int16 r, unsigned int16 color);
                    
void fillCircleHelper( signed int16 x0, signed int16 y0, signed int16 r,
                unsigned int8 cornername, signed int16 delta, unsigned int16 color);

void drawCircleHelper( signed int16 x0, signed int16 y0,
                       signed int16 r, unsigned int8 cornername, 
                       unsigned int16 color);
                    

void fillCircle( signed int16 x0, signed int16 y0, signed int16 r, 
                 unsigned int16 color );

void fillCircleHelper( signed int16 x0, signed int16 y0, signed int16 r, 
                       unsigned int8 cornername, signed int16 delta, 
                       unsigned int16 color);

#define swap(a, b) { int16 t = a; a = b; b = t; }


#ifdef TFTTEXT
void glcd_putc(unsigned int8 c) 
{

  if (c == '\n') 
  {
    tftinfo.cursor_y += tftinfo.textsize*8;
    tftinfo.cursor_x = 0;
  } 
  else if (c == '\r')
  {
    return; // skip em
  } 
  else 
  {
    drawChar(tftinfo.cursor_x, tftinfo.cursor_y, c, tftinfo.textcolor, tftinfo.textbgcolor, tftinfo.textsize);
    tftinfo.cursor_x += tftinfo.textsize*6;
    if( tftinfo.wrap && ( tftinfo.cursor_x > ( tftinfo._width - tftinfo.textsize*6 )) )
    {
      tftinfo.cursor_y += tftinfo.textsize*8;
      tftinfo.cursor_x = 0;
    }
  }

}


void drawChar( signed int16 x, signed int16 y, unsigned char c,
             unsigned int16 color, unsigned int16 bg, unsigned int8 size)
{

//!  if((x >= tftinfo._width)            || // Clip right
//!     (y >= tftinfo._height)           || // Clip bottom
//!     ((x + 5 * size - 1) < 0) || // Clip left
//!     ((y + 8 * size - 1) < 0))   // Clip top
//!    return;

int8 i,j;
unsigned int8 line;
unsigned int16 test;

  for (i=0; i<6; i++ ) 
  {
    if (i == 5) 
      line = 0x0;
    else
    {
      test = ((unsigned int8)c*(unsigned int16)5)+i; //FORCE 16bit math
      line = font[ test ]; // ((int16)((char)c*5)+i)
     // printf("line: %c %Lu\r\n", c, test );
    }
    for (j = 0; j<8; j++) 
    {
      if (line & 0x1) 
      {
        if (size == 1) // default size
          drawPixel(x+i, y+j, color);
        else 
        {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, color);
        } 
      } 
      else if (bg != color) 
      {
        if (size == 1) // default size
          drawPixel(x+i, y+j, bg);
        else 
        {  // big size
          fillRect(x+(i*size), y+(j*size), size, size, bg);
        }    
      }
      line >>= 1;
    }
  }
}


void setCursor(int16 x, int16 y) 
{
  tftinfo.cursor_x = (tftinfo.textsize*6) * x;
  tftinfo.cursor_y = (tftinfo.textsize*8) * y;
}


void setTextSize(unsigned int8 s) 
{
  tftinfo.textsize = (s > 0) ? s : 1;
}


void setTextColor(unsigned int16 c) 
{
  tftinfo.textcolor = c;
  tftinfo.textbgcolor = c; 
  // for 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
}

void setTextColor(unsigned int16 c, unsigned int16 b) 
 {
   tftinfo.textcolor = c;
   tftinfo.textbgcolor = b; 
 }

void setTextWrap(boolean w) 
{
  tftinfo.wrap = w;
}
#endif

void drawLine( signed int16 x0, signed int16 y0, signed int16 x1, signed int16 y1, unsigned int16 color) 
{
  signed int16 dx, dy, err, ystep, steep; 
  
  steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) 
  {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) 
  {
    swap(x0, x1);
    swap(y0, y1);
  }

  
  dx = x1 - x0;
  dy = abs(y1 - y0);

  err = dx / 2;
  
  if (y0 < y1)
  {
    ystep = 1;
  } 
  else
  {
    ystep = -1;
  }

  for (; x0<=x1; x0++) 
  {
    if (steep) 
    {
      drawPixel(y0, x0, color);
    } else 
    {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) 
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawCircle( signed int16 x0, signed int16 y0, signed int16 r, unsigned int16 color)
{
  signed int16 f = 1 - r;
  signed int16 ddF_x = 1;
  signed int16 ddF_y = -2 * r;
  signed int16 x = 0;
  signed int16 y = r;

  drawPixel(x0, y0+r, color);
  drawPixel(x0, y0-r, color);
  drawPixel(x0+r, y0, color);
  drawPixel(x0-r, y0, color);

  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    
  }
}

// draw a rectangle
void drawRect( signed int16 x, signed int16 y, signed int16 w, signed int16 h, unsigned int16 color )
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, h, w, color);// ORG drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
  //debug_printf("drawRect: %6Ld\r", y+h-1 );
}

void drawFastVLine( signed int16 x, signed int16 y, signed int16 h, unsigned int16 color )
{
  // stupidest version - update in subclasses if desired!
  drawLine(x, y, x, y+h-1, color);
}


void drawFastHLine( signed int16 x, signed int16 y, 
                     signed int16 w, unsigned int16 color )
{
  // stupidest version - update in subclasses if desired!
  drawLine(x, y, x+w-1, y, color);
}

void fillCircle( signed int16 x0, signed int16 y0, signed int16 r, 
                 unsigned int16 color ) 
{
  drawFastVLine(x0, y0-r, 2*r+1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

// used to do circles and roundrects!
void fillCircleHelper( signed int16 x0, signed int16 y0, signed int16 r, 
                       unsigned int8 cornername, signed int16 delta, 
                       unsigned int16 color)
{

  signed int16 f     = 1 - r;
  signed int16 ddF_x = 1;
  signed int16 ddF_y = -2 * r;
  signed int16 x     = 0;
  signed int16 y     = r;

  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) 
    {
      drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) 
    {
      drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}

void drawTriangle( signed int16 x0, signed int16 y0,
            signed int16 x1, signed int16 y1, 
            signed int16 x2, signed int16 y2,
            unsigned int16 color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void fillTriangle ( signed int16 x0, signed int16 y0,
                    signed int16 x1, signed int16 y1, 
                    signed int16 x2, signed int16 y2, 
                    unsigned int16 color)
{

  signed int16 a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) 
  {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) 
  {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) 
  {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2)  // Handle awkward all-on-same-line case as its own thing
  {
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  signed int16
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) 
  {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) 
  {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}

void drawRoundRect( signed int16 x, signed int16 y, signed int16 w,
                    signed int16 h, signed int16 r, unsigned int16 color)
{                    
  
  drawFastHLine( x+r  , y    , w-2*r, color); // Top
  drawFastHLine( x+r  , y+h-1, w-2*r, color); // Bottom
  drawFastVLine( x    , y+r  , h-2*r, color); // Left
  drawFastVLine( x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void fillRoundRect( signed int16 x, signed int16 y, signed int16 w,
                    signed int16 h, signed int16 r, unsigned int16 color) 
{
  
  fillRect(x+r, y, w-2*r, h, color);

  // draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void drawCircleHelper( signed int16 x0, signed int16 y0,
                       signed int16 r, unsigned int8 cornername, 
                       unsigned int16 color)
{
  signed int16    f     = 1 - r;
  signed int16    ddF_x = 1;
  signed int16    ddF_y = -2 * r;
  signed int16    x     = 0;
  signed int16    y     = r;

  while (x<y) 
  {
    if (f >= 0) 
    {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) 
    {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) 
    {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) 
    {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) 
    {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

void fillRect( signed int16 x1, signed int16 y1, signed int16 w, 
                  signed int16 h, unsigned int16 fillcolor )
{
   signed int16  x2, y2;

  // Initial off-screen clipping
      if( (w            <= 0     ) ||  (h             <= 0      ) ||
          (x1           >= tftinfo._width) ||  (y1            >= tftinfo._height) ||
          ((x2 = x1+w-1) <  0     ) || ((y2  = y1+h-1) <  0      )) return;
          
      if(x1 < 0)  // Clip left
      {
         w += x1;
         x1 = 0;
      }

      if(y1 < 0)  // Clip top
      {
         h += y1;
         y1 = 0;
      }
      
      if(x2 >= tftinfo._width)  // Clip right
      {
         x2 = tftinfo._width - 1;
         w  = x2 - x1 + 1;
      }

      if(y2 >= tftinfo._height)  // Clip bottom
      {
         y2 = tftinfo._height - 1;
         h  = y2 - y1 + 1;
      }

  setAddrWindow(x1, y1, x2, y2); 
  flood(fillcolor, (unsigned int32)w * (unsigned int32)h); //FORCE 32bit math    
  setAddrWindow(0, 0, tftinfo._width - 1, tftinfo._height - 1);
  
}
