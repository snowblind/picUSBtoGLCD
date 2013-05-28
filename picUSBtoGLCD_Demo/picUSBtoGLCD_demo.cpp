//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//																				//
//				             picUSBtoGLCD Demo v0.02							//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//																				//
//     This program is a small demo of how to talk to the picUSBtoGLCD proto	//
//	board using the libusb-win32  API it prints "hello all" to the				//
//	connected GLCD, then it loops to show the status of the RB0 pin				//
//  of the PIC18F4550.															//
//																				//
//																				//
//																				//
//																				//
//////////////////////////////////////////////////////////////////////////////////
//																				//
//	CHANGES:																	//
// 																				//
//			Please see the CHANGES file inculded in this dist for a list of		// 
//		program changes															//
//																				//
//////////////////////////////////////////////////////////////////////////////////
//																				//
//	CURRENT LIMITATIONS:														//
//																				//
//			The current picUSBtoGLCD proto board FIRMWARE is text only 			//
//      and so is this demo 	 												//									
//																				//
//////////////////////////////////////////////////////////////////////////////////
//******************************************************************************//
//*																			   *//
//* NOTES:																	   *//
//*                                                                            *//
//*			This code uses libusb-win32 ( http://libusb-win32.sourceforge.net )*//
//*		and and needs to link with libusb.lib. Please see the				   *//
//*		libusb-win32_license.txt file inculded with this dist for license info *//
//*																			   *//
//*			About using goto in C. The current docs for libusb-win32 do not	   *//
//*		seem to show functions returning a usefull error codes yet therefor	   *//
//*		I use goto since all return codes are the same. I have not looked at   *//
//*		the usb.h to see if the docs lag behind.							   *//
//*																			   *//
//******************************************************************************//
//////////////////////////////////////////////////////////////////////////////////
//																				//
//                      Copyright 2010 Michael Pounders							//
//																				//	
//    This program is free software: you can redistribute it and/or modify		//
//    it under the terms of the GNU General Public License as published by		//
//    the Free Software Foundation, either version 3 of the License, or			//
//    (at your option) any later version.										//
//																				//
//    This program is distributed in the hope that it will be useful,			//
//    but WITHOUT ANY WARRANTY; without even the implied warranty of			//
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//    GNU General Public License for more details.								//
//																				//
//    You should have received a copy of the GNU General Public License			//
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.		//
//																				//
//////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "usb.h"

#define Device_VID      0x0461  // defualt CCS USB stack VID microchip?
#define Device_PID      0x474C  // picUSBtoGLCD proto board PID 

static usb_dev_handle *UsbDevInstance;

usb_dev_handle *LibUSBGetDevice (unsigned short vid, unsigned short pid)
{

        struct usb_bus *UsbBus = NULL;
        struct usb_device *UsbDevice = NULL;

        usb_dev_handle *ret;

        usb_find_busses();
        usb_find_devices();

        for (UsbBus = usb_get_busses(); UsbBus; UsbBus = UsbBus->next)
		{
                for (UsbDevice = UsbBus->devices; UsbDevice; UsbDevice = UsbDevice->next)
				{
                        if (UsbDevice->descriptor.idVendor == vid && UsbDevice->descriptor.idProduct== pid)
						{
                                break;
                        }
                }
        }

        if (!UsbDevice) 
			return NULL;

        ret = usb_open(UsbDevice);

        if (usb_set_configuration(ret, 1) < 0)
		{
                usb_close(ret);
                return NULL;
        }

        if (usb_claim_interface(ret, 0) < 0)
		{
                usb_close(ret);
                return NULL;
        }

        return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{

int rst;
BYTE reply[8];
BYTE cmd[8];
BYTE data[8];

	printf("\r\npicUSBtoGLCD Demo  v0.01\r\n\r\n");


        usb_init();

        UsbDevInstance = NULL;
        while ((UsbDevInstance = LibUSBGetDevice(Device_VID, Device_PID)) == NULL)
		{
                printf ("picUSBtoGLCD device NOT detected, waiting...\r");
                Sleep (3000);
        }
        printf ("\r\npicUSBtoGLCD device found!\n");


		   cmd[0] = 0xFE; // Command Packet header
		   cmd[1] = 0x01; // Clear text buffer of GLCD								 0x01 = clearTextBuffer GLCD cmd

		   rst = usb_interrupt_write(UsbDevInstance, 0x01, (char *)cmd, 2, 1000);
		   if ( rst <= 0)
			   goto done;


		   cmd[0] = 0xFE; // Command Packet header
		   cmd[1] = 0x02; // Goto x,y char location on GLCD 32x16 = 512 chars total  0x02 = textGotoxy GLCD cmd
		   cmd[2] = 0x01; // arg x 0-31
		   cmd[3] = 0x01; // arg y 0-15

		   rst = usb_interrupt_write(UsbDevInstance, 0x01, (char *)cmd, 4, 1000);
		   if ( rst <= 0)
			   goto done;

		   
		   cmd[0] = 0xFE; // Command Packet header
		   cmd[1] = 0x03; // writeTextToScreen   									 0x03 = writeTextToScreen GLCD cmd

		   rst = usb_interrupt_write(UsbDevInstance, 0x01, (char *) cmd, 2, 1000);
		   if ( rst <= 0)
			   goto done;


		   data[0] = 'h';  // After writeTextToScreen GLCD cmd is sent you can send as many as 512 chars if you started 
		   data[1] = 'e';  // at gotoxy(0,0) it will keep wraping to the next line as the lines fill up.
		   data[2] = 'l'; 
		   data[3] = 'l';  // You can issue at any time a textGotoxy GLCD cmd packet, writeTextToScreen GLCD cmd packet, and
		   data[4] = 'o';  // then packets like this one to start printing chars in a new area
		   data[5] = ' '; 
		   data[6] = 'a';  // The interface is limited to sending 7 chars that are to be printed on the GLCD at a time.
		   data[7] = 0x00; // All packets that are to be printed on the GLCD must be termnated with a 0x00 byte.

		   rst = usb_interrupt_write(UsbDevInstance, 0x01, (char *) data, 8, 1000);
		   if ( rst <= 0)
			   goto done;


		   data[0] = 'l';  // If you need to send less than 7 chars to be printed on the GLCD
		   data[1] = 'l';  // this is how you do it.
		   data[2] = 0x00; // All packets that are to be printed on the GLCD must be termnated with a 0x00 byte.

		   rst = usb_interrupt_write(UsbDevInstance, 0x01, (char *) data, 8, 1000);
		   if ( rst <= 0)
			   goto done;


// The above 3 steps ( textGotoxy GLCD cmd packet, writeTextToScreen GLCD cmd packet, and then packets printed to GLCD)
// could be turned into a function but I wanted to show the cmd and print packet structure. I hope to keep it smiple and
// easy to understand. 
		   
// Maybe the next version will inculde a printToGLCD( int x, int y, char *text ) function that will write up to 512 chars to the 
// GLCD screen if the function call will not go past the text memory area of the GLCD


		while( TRUE ) // loop to show RB0 status
		{			  // I should wait on a key press here also as there is no way to 
					  // exit program except with a CTRL-C but that skips the clean up usb_close(UsbDevInstance) 	
		   rst = usb_interrupt_read(UsbDevInstance, 0x81, (char *) reply, sizeof(reply), 1000);
		   if ( rst <= 0)
			   goto done;
		   printf("\r %03hu", reply[0] );

		}

done:
	usb_close(UsbDevInstance);
	return rst;
}

