#include <18F4550.h>
#fuses NOWDT,NOPROTECT,NOLVP,NODEBUG,VREGEN,PUT
#use delay(crystal=20M, clock=48M, USB_FULL)
#use rs232(baud=57200, UART1)

///////// config options //////////////////////////////////////////////////////
#define USB_CONFIG_VID        0x0461  //0x0461 
#define USB_CONFIG_PID        0x474C  //0x475C
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

#define GLCD_WIDTH         256
#define GLCD_HEIGHT        128
#define GLCD_CHAR_WIDTH    8
#define GLCD_CHAR_HEIGHT   8
#define GLCD_RST           PIN_A5
#define GLCD_RD            PIN_E2
#define GLCD_WR            PIN_E1
#define GLCD_CS            PIN_E0
#define GLCD_A0            PIN_A4

#nolist
#include <SED1335\sed1335.c>
#include <SED1335\process_USBtoGLCD.c>

#list

//#define debug

#ifndef debug
#define printf 
#endif

int8 datardy; //FIXME put in function as static and set there?





void main( void )
{
   int8 in[USB_EP1_RX_SIZE];  //PC to PIC 
   int8 out[USB_EP1_TX_SIZE]; //PIC to PC
   
   datardy = 0x00;
 
   printf("\033[2J\033[H");                        // Clear VT100 Screen....
   delay_ms( 50 );
   printf("picUSBtoGLCD V0.11 PIC: 18F4550\r\n");
   printf("PICCV: %s  COMPILE DATE/TIME: %s %s\r\n", getenv("VERSION_STRING"), __DATE__, __TIME__ );
   delay_ms( 50 );

  picUSBtoGLCDinfo.pversion = 0x3E;
  picUSBtoGLCDinfo.drivertype = 0x5F;
  
  
   glcd_init(ON); //glcd_init(ON);
   
   
   glcd_ShowSplashScreen();
   
   usb_init();
   
   
   
      while( TRUE )
      {
      
         if( usb_enumerated() )
         {
            usb_task();


               if( datardy )
               {   
                  printf("DR: 0x%02X", datardy );
                  out[0] = datardy;
                  memset( &out[1], 0x00, USB_EP1_TX_SIZE - 1 );
                  
                  process_USBtoGLCD_outpackets( out );
                  usb_put_packet( 1, out, USB_EP1_TX_SIZE, USB_DTS_TOGGLE );
                  datardy = 0x00;
               }
            
               if( usb_kbhit( 1 ) )
               { 
                  led_on( RX_LED );
                  //memset( in, 0x00, sizeof( in ) );
                  usb_get_packet( 1, in, USB_EP1_RX_SIZE );
                  process_USBtoGLCD_inpackets( in );
                  led_off( RX_LED );
               }

         }
               
      }
}
