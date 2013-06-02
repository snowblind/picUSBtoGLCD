#define PLUGINGUID "{5FE84FE3-FA02-45be-AA34-3E55F3063E47}"
#define PLUGINNAME "picUSBtoGLCD v0.1 alpha"


#include "sdk\component.h"
#include "sdk\plugin.h"
#include "sdk\displaydriver.h"
//#include "..\sdk\configure.h"

#include <windows.h>
#include <stdio.h>

#include "stdafx.h"
#include "usb.h"

#define Device_VID      0x0461  // CCS VID
#define Device_PID      0x474C  // CCS HID DEMO defualt PID

static usb_dev_handle *UsbDevInstance;


class Plugin : public IPlugin
{

virtual void __stdcall onCreate(void)
{
	
}
virtual void __stdcall onDestroy(void)
{

}
};


class picUSBtoGLCD : public IDisplayDriver
{
public:


usb_dev_handle * LibUSBGetDevice (unsigned short vid, unsigned short pid);

virtual bool __stdcall initialize(void)
{
//                xpixels = readSettingInt("WIDTH");
//                ypixels = readSettingInt("HEIGHT");
//                font_width = readSettingInt("FONTSIZE");
	xpixels = 256;
	ypixels = 128;
	fontsize = 8;
	xpixels = ( xpixels==0 ) ? 240 : xpixels;
	ypixels = ( ypixels==0 ) ? 128 : ypixels;
	fontsize = ( fontsize==0 ) ? 8 : fontsize;
	character_per_row = xpixels/fontsize;
	if( (xpixels)%fontsize != 0 )
	character_per_row++;
	bytecount = character_per_row * ypixels;
	charactercount = ypixels/8 * character_per_row;
	T_BASE = 0x0000;
	G_BASE = 0x1000;
	driverinfo.XPixels = xpixels;
	driverinfo.YPixels = ypixels;
	driverinfo.XChars = character_per_row;
	driverinfo.YChars = ypixels/8;




        usb_init ();
        UsbDevInstance = NULL;
        while ((UsbDevInstance = LibUSBGetDevice (Device_VID, Device_PID)) == NULL) {
                OutputDebugString("picUSBtoGLCD device NOT detected, waiting...\n");
                Sleep (2000);
        }
		OutputDebugString("picUSBtoGLCD device found!\n");

	clearText();
	clearGraph();
	return true;
}
virtual void __stdcall unInitialize(void)
{
		usb_close(UsbDevInstance);
}
virtual int __stdcall configure(void)
{
// launch configuration dialog here if needed
/*
                int ret = DialogBox(GetModuleHandle("usb_sed133x_driver.dll"), MAKEINTRESOURCE(IDD_CONFIGURATION), 
                                NULL, ConfigurationDlgProc);
                if(ret == -1)
                {
                        MessageBox(NULL, "Failed loading configuration dialog!", "Error", MB_OK | MB_ICONINFORMATION);
                }
*/
	return -1;
}
virtual DriverInfo __stdcall getDriverInfo(void)
{
	return driverinfo;
}

virtual void __stdcall clearText(void)
{
	unsigned char cmdbuffer[] = { 0xFE, 0x01 };
	usb_interrupt_write(UsbDevInstance, 0x01, (char *) cmdbuffer, 2, 1000);
}

virtual void __stdcall clearGraph(void)
{
	//gfx not supported yet in the picUSBtoGLCD PIC FIRMWARE
}

virtual void __stdcall writeText(char *string, int x, int y)
{

	int i = 0;


	unsigned char packet[8];
	unsigned char cmdbuffer[] = { 0xFE, 0x02, x, y };

	usb_interrupt_write(UsbDevInstance, 0x01, (char *)cmdbuffer, 4, 1000);


	unsigned char cmdbuffer2[] = { 0xFE, 0x03 };
	usb_interrupt_write(UsbDevInstance, 0x01, (char *)cmdbuffer2, 2, 1000);


	do
	{
		packet[i] = *string;

		if( packet[i] == '\0')
		{
			i = 0;
			usb_interrupt_write(UsbDevInstance, 0x01, (char *) packet, 8, 1000);
			continue;
		}
		if( i == 6 )
		{
			i = 0;
			packet[7] = 0x00;
			usb_interrupt_write(UsbDevInstance, 0x01, (char *) packet, 8, 1000);
			continue;
		}
	
		i++;

	} while( *string++ != '\0' );
	
}

virtual void __stdcall writeGraphics(unsigned char *LcdData[], int x, int y, int sizex, int sizey)
{
   //gfx not supported yet in the picUSBtoGLCD PIC FIRMWARE
}

private:
	DriverInfo driverinfo;

	int charactercount;
	int bytecount;
	int T_BASE;
	int G_BASE;
	int fontsize;
	int buffersize;
	int ypixels;
	int xpixels;
	int buffer_bytes_per_row;
	int character_per_row;






};

usb_dev_handle * picUSBtoGLCD::LibUSBGetDevice (unsigned short vid, unsigned short pid) {
        struct usb_bus *UsbBus = NULL;
        struct usb_device *UsbDevice = NULL;
        usb_dev_handle *ret;

        usb_find_busses ();
        usb_find_devices ();

        for (UsbBus = usb_get_busses(); UsbBus; UsbBus = UsbBus->next) {
                for (UsbDevice = UsbBus->devices; UsbDevice; UsbDevice = UsbDevice->next) {
                        if (UsbDevice->descriptor.idVendor == vid && UsbDevice->descriptor.idProduct== pid) {
                                break;
                        }
                }
        }
        if (!UsbDevice) return NULL;
        ret = usb_open (UsbDevice);

        if (usb_set_configuration (ret, 1) < 0) {
                usb_close (ret);
                return NULL;
        }

        if (usb_claim_interface (ret, 0) < 0) {
                usb_close (ret);
                return NULL;
        }

        return ret;
}


// IMPORTANT: without this your class won't be included in the plugin
// SYNTAX: RegisterFactory<Interface, YourDerivedClass> something;
static RegisterFactory<IPlugin, Plugin> plugin;
static RegisterFactory<IDisplayDriver, picUSBtoGLCD> displaydriver;