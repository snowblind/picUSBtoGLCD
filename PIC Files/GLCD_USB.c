#include <18F4550.h>
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN
#use delay(clock=48000000)
#use rs232(baud=9600, UART1)


#define USB_EP1_TX_ENABLE USB_ENABLE_INTERRUPT
#define USB_EP1_TX_SIZE 8

#define USB_EP1_RX_ENABLE USB_ENABLE_INTERRUPT
#define USB_EP1_RX_SIZE 8

#define USB_CONFIG_HID_TX_SIZE   8
#define USB_CONFIG_HID_RX_SIZE   8 

#define USB_CONFIG_VID  0x0461
#define USB_CONFIG_PID  0x474C

#include "pic18_usb.h"
#include "usb_desc_hid.h"
#include "usb.c"


#define GLCD_WIDTH         256
#define GLCD_HEIGHT        128
#define GLCD_CHAR_WIDTH    8
#define GLCD_CHAR_HEIGHT   8
#define GLCD_RST           PIN_A5
#define GLCD_RD            PIN_E2
#define GLCD_WR            PIN_E1
#define GLCD_CS            PIN_E0
#define GLCD_A0            PIN_A4

#include "sed1335.c"

void glcd_putc( char c );
void process_USBtoGLCD( int8 * packet );
int16 glcd_Text_gotoxy( int8 x, int8 y);
void showSplashScreen( void );

void main( void )
{

   int8 delay=0;
   int8 out[8];
   int8 in[8];

   
   
  
   glcd_init(ON);
   
   showSplashScreen();
   
   usb_init();
   

   
      while( TRUE )
      {
      
         if( usb_enumerated() )
         {
          
            delay++;
            if( delay>=250 )
            {
               delay = 0;
               
               out[0] = !input( PIN_B0 );
               usb_put_packet( 1, out, 8, USB_DTS_TOGGLE );
            }
            
            if( usb_kbhit( 1 ) )
            { 
               memset( in, 0x00, sizeof( in ) );            
               usb_get_packet( 1, in, 8 );
               process_USBtoGLCD( in );
            }

          delay_ms( 1 );

         }
               
      }
}  
            
         
      
void glcd_putc( char c )
{

   glcd_sendCMD(GLCD_CMD_DISPLAY_WRITE);
   TGLCD_DATA
   glcd_sendByte( c );
  
}

void process_USBtoGLCD( int8 * packet )
{

   static int8 state_m;
   
   if( packet[0] == 0xFE) // Command Packet header
   {
      switch( packet[1] ) // GLCD commands
      {
      
         case 0x01: // Clear text buffer of GLCD                           0x01 = clearTextBuffer GLCD cmd
            state_m = 0x00;
            glcd_fillScreenText(' ');
            break;
            
         case 0x02: // Goto x,y char location on GLCD 32x16                0x02 = textGotoxy GLCD cmd
            state_m = 0x00;
            setCursorAddress( glcd_Text_gotoxy( packet[2], packet[3]) );
            break;
            
         case 0x03:           // Setup state machine                       0x03 = writeTextToScreen GLCD cmd
            state_m = 0x03;   // see below   
            break;
            
      }
   }
   else
   {
      switch( state_m )
      {
      
         case 0x03:                    // Output all incomming packets as text 
            while( *packet != 0x00 )   // to GLCD untill a 0x01 or 0x02 cmd 
            {                          // resets the state machine
               glcd_putc( *packet++ );
            }
            break; 
            
      }
   }
}

int16 glcd_Text_gotoxy( int8 x, int8 y)
{

   int16 addr = GLCD_TEXT_ADDR;
   
   if( y == 16 )
   {
      addr = x * y;
   }
   else 
   { 
      addr = x + ( y * 32);
   }
   
   return addr;

}

void showSplashScreen( void )
{
   // simple SplashScreen for now just shows compile date and time
   setCursorAddress( 0 );   
   printf(glcd_putc, "%s %s PCH: V%s  ", __DATE__, __TIME__, __PCH__);

}
