
#include <ILI932X\logo_320x240x1.h>

#inline
void process_USBtoGLCD_inpackets( int8 * packet );
#inline
void process_USBtoGLCD_outpackets( int8 * packet );

void glcd_clearScreenGfx( void );
void glcd_clearScreenText( void );

void glcd_Text_gotoxy( int16 x, int16 y );
void glcd_Gfx_gotoxy( int16 x, int16 y );
void print_tftinfo_t( void );
void showSplashScreen( void );

struct picUSBtoGLCDinfo_t  //FIXME: Item 192
{
   int8 pversion;
   int8 drivertype;
}picUSBtoGLCDinfo;


void process_USBtoGLCD_inpackets( int8 * packet )
{
   
   if( packet[0] == 0xFE ) // Command Packet header
   {
      switch( packet[1] ) // GLCD commands
      {
      
         case 0x01:           // Clear text buffer of GLCD                 0x01 = clearTextBuffer GLCD cmd         
            debug_printf("\n\r\n\rCase == 0x01\n\r"); 
            glcd_clearScreenText();
            break;
            
         case 0x02:           // Goto x,y char location on GLCD 32x16      0x02 = textGotoxy GLCD cmd                           
            debug_printf("\n\r\n\rCase == 0x02\n\r");
            glcd_Text_gotoxy( make16( packet[2], packet[3] ),
                              make16( packet[4], packet[5] ));
            break;
                      
         case 0x07:           // Clear gfx buffer of GLCD                  0x07 = clearGfxBuffer GLCD cmd
            debug_printf("\n\r\n\rCase == 0x07\n\r");
            glcd_clearScreenGfx();
            break;            
            
         case 0x08:           //Goto x,y pixel location on GLCD 320x240    0x08 = gfxGotoxy GLCD cmd
            debug_printf("\n\r\n\rCase == 0x08\n\r");
            glcd_Gfx_gotoxy( make16( packet[2], packet[3] ),
                             make16( packet[4], packet[5] ));
            break;
            
         case 0x09:           //draw bitmap to screen                      0x09 = gfxGotoxy GLCD cmd
            debug_printf("\n\r\n\rCase == 0x09\n\r");
            CS_ACTIVE;
            CD_COMMAND;
  
            write8(0x00); // High byte of GRAM register...
            write8(0x22); // Write data to GRAM            
            CD_DATA;           
            break;
         case 0x22:           //Get device info packet                     0x22 = GetDeviceinfo cmd
            debug_printf("\n\r\n\rCase == 0x22\n\r");
            datardy = 0x01;
            break;            
         case 0x58:           
            debug_printf("\n\r\n\rCase == 0x58\n\r");
            print_tftinfo_t( );            
            break;
            
         case 0x85:           
            debug_printf("\n\r\n\rCase == 0x85\n\r");
            setTextColor( BLACK, WHITE );
            printf(glcd_putc, "%s %s PCH: V%s  ", __DATE__, __TIME__, __PCH__);          
            break;

            
      }
   }
   if( packet[0] == 0xEF ) // Text Data Packet header
   {
      
      int8 psize = packet[1]; //packet size
      
      packet++; //skip Data Packet header
      packet++; //skip packet size
      //printf("    Got 0xEF\n\r");
      
            while( psize-- > 0 )   
            {  
              glcd_putc( *packet++ );              
            }               
   }
   if( packet[0] == 0xEE ) // gfx Data Packet header
   {
      
      int8 psize = packet[1]; //packet size
      
      packet++; //skip Data Packet header
      packet++; //skip packet size
      //printf("    Got 0xEE\n\r");      

            while( psize > 0 )
            {
               write8( *packet++ ); psize--;
               write8( *packet++ ); psize--;               
            }               
   }   
 
}

void process_USBtoGLCD_outpackets( int8 * packet )
{
      switch( packet[0] ) // FIXME: 
      {
      
         case 0x01:           // Get device info               0x01 = GetdeviceInfo
            debug_printf("SEND: 0x01 SIZE: %i", sizeof( picUSBtoGLCDinfo )); 
            packet[1] = sizeof( picUSBtoGLCDinfo );
            memcpy( &packet[2], &picUSBtoGLCDinfo, sizeof( picUSBtoGLCDinfo ) );
            break;
            
         case 0x02:                                      
            break;
                      
         case 0x07:           
            break;            
            
         case 0x08:           
            break;
            
         case 0x09:           
            break;            
      }
}


void glcd_clearScreenGfx( void )
{
   fillScreen( tftinfo.textbgcolor );
}

void glcd_clearScreenText( void )
{
   setCursor( 0, 0 );
}

void glcd_Text_gotoxy( int16 x, int16 y )
{
   setCursor(  x, y);
}

void glcd_Gfx_gotoxy( int16 x, int16 y )
{

}

void glcd_ShowSplashScreen( void )
{
   
   drawLogo();
   
   setTextSize( 1 );
   setCursor( 1, 24);

#if defined(DEBUG
   printf(glcd_putc, "%s %s PCH: V%s DEBUG: TRUE", __DATE__, __TIME__, __PCH__);
#else
   printf(glcd_putc, "%s %s PCH: V%s DEBUG: FALSE", __DATE__, __TIME__, __PCH__);
#endif

   setCursor(1, 28);   
   printf(glcd_putc, "Firmware: V%x.%x", USB_CONFIG_VERSION >>8 & 0xff, USB_CONFIG_VERSION  & 0xff );


}

void print_tftinfo_t( void )
{   
   printf( "\n\r\n\rtftinfo\n\rWIDTH: %Ld HEIGHT %Ld\n\r", tftinfo.WIDTH, tftinfo.HEIGHT );
   printf( "_width: %Lu _height: %Lu\n\r", tftinfo._width, tftinfo._height );
   printf( "cursor_x: %Ld cursor_y: %Ld\n\r", tftinfo.cursor_x, tftinfo.cursor_y);
   printf( "textcolor: %Lu textbgcolor %Lu\n\r", tftinfo.textcolor, tftinfo.textbgcolor );
}
