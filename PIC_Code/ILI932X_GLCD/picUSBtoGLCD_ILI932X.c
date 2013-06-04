#include <18F4550.h>
#fuses NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,PUT
#use delay(crystal=20M, clock=48M, USB_FULL)
#use rs232(baud=57200, UART1)

///////// config options //////////////////////////////////////////////////////
#define USB_CONFIG_VID        0x0461  //0x0461 
#define USB_CONFIG_PID        0x475C  //0x475C
#define USB_CONFIG_VERSION    0x0033            //01.00  (range is 00.00 to 99.99)
#define USB_CONFIG_BUS_POWER  200               //100mA  (range is 0..500)
//////// end config ///////////////////////////////////////////////////////////
#define USB_HID_DEVICE        FALSE
#define USB_EP1_TX_ENABLE     USB_ENABLE_BULK   //turn on EP1 for IN bulk transfers 
#define USB_EP1_RX_ENABLE     USB_ENABLE_BULK   //turn on EP1 for OUT bulk transfers
#define USB_EP1_TX_SIZE       64                //size to allocate for the tx endpoint 1 buffer
#define USB_EP1_RX_SIZE       64                //size to allocate for the rx endpoint 1 buffer

#nolist
#include <pic18_usb.h>
#include "usb_desc_bulk.h"
#include <usb.c>
#list

#define RX_LED PIN_B5
#define led_on output_low
#define led_off output_high 

//#define DEBUG


#if defined(DEBUG)
   #define debug_printf printf
   #warning In DEBUG mode.....
#else
   #define debug_printf(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z)
#endif

int8 datardy; //FIXME Item: 190


#nolist
#define TFTFAST
#define TFTTEXT
#include <ILI932X\ILI932X.c>
#include <ILI932X\glcd_gfx.c>
//#include <ILI932X\4W_TouchScreen.c>
#include <ILI932X\process_USBtoGLCD.c>
#list



void main( void )
{
   int8 in[USB_EP1_RX_SIZE];  //PC to PIC 
   int8 out[USB_EP1_TX_SIZE]; //PIC to PC
   
   datardy = 0x00;
 
   printf("\033[2J\033[H");   // Clear VT100 Screen.... I use PuTTY for my serial port mon program
   delay_ms( 50 );
   printf("picUSBtoGLCD V%x.%x PIC: %s\r\n", USB_CONFIG_VERSION >>8 & 0xff, USB_CONFIG_VERSION  & 0xff, getenv("DEVICE"));

#if defined(DEBUG)
   printf("CCS C: V%s  COMPILE DATE/TIME: %s %s DEBUG: TRUE\r\n", getenv("VERSION_STRING"), __DATE__, __TIME__ );
#else
   printf("CCS C: V%s  COMPILE DATE/TIME: %s %s DEBUG: FALSE\r\n", getenv("VERSION_STRING"), __DATE__, __TIME__ );
#endif

   
   delay_ms( 50 );


  picUSBtoGLCDinfo.pversion = 0x3E;
  picUSBtoGLCDinfo.drivertype = 0x5F;
  
  
   glcd_init(); 
   setRotation( 3 );
   
   glcd_ShowSplashScreen();
   
   usb_init();
   
   
   
      while( TRUE )
      {
      
         if( usb_enumerated() )
         {
            usb_task();


               if( datardy )
               {   
                  debug_printf("DR: 0x%02X", datardy );
                  out[0] = datardy;
                  memset( &out[1], 0x00, USB_EP1_TX_SIZE - 1 );
                  
                  process_USBtoGLCD_outpackets( out );
                  usb_put_packet( 1, out, USB_EP1_TX_SIZE, USB_DTS_TOGGLE );
                  datardy = 0x00;
               }
            
               if( usb_kbhit( 1 ) )
               { 
                  led_on( RX_LED );
                  usb_get_packet( 1, in, USB_EP1_RX_SIZE );
                  process_USBtoGLCD_inpackets( in );
                  led_off( RX_LED );
               }

         }
               
      }
}
