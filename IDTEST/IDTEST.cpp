#include <stdio.h>
#include<iostream>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#pragma comment(lib,"Setupapi.lib")

#define INTERFACE_DETAIL_SIZE (1024)


int main()
{

	GUID USB_Device_GUID = { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
	//{ 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
	//GUID testguid = {0x4d36e96b,0xe325,0x11ce,{0xbf,0xc1,0x08,0x00,0x2b,0xe1,0x03,0x18}};
	//GUID testguid = {0x4d36e96b,0xe325,0x11ce,{0xbf,0xc1,0x08,0x00,0x2b,0xe1,0x03,0x18}};
	//GUID testguid = {0x4D1E55B2,0xF16F,0x11CF,{0x88,0xCB,0x00,0x11,0x11,0x00,0x00,0x30}};
	//{0x36FC9E60,0xC465,0x11CF,{0x80,0x56,0x44,0x45,0x53,0x54,0x00,0x00}};
	//GUID testguid = GUID_DEVINTERFACE_USB_DEVICE;

	HDEVINFO                            hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVINFO_DATA                     devinfo;
	PSP_DEVICE_INTERFACE_DETAIL_DATA    DeviceInterfaceDetailData = NULL;
	SP_DEVICE_INTERFACE_DATA            DeviceInterfaceData;

	ULONG                               predictedLength = 0;
	ULONG                               requiredLength = 0;
	//ULONG                               bytes;
    BOOLEAN                             pSuccess = false;
	int                                 icount = 0;
	char                                driverkey[255];
	char                                PrinterPathName[255];    //´òÓ¡»úÂ·¾¶
	HANDLE                              hHandle = INVALID_HANDLE_VALUE;


//	char* interfacename;
//	interfacename = (char*)malloc(2048);


	hDevInfo = SetupDiGetClassDevs (&USB_Device_GUID, NULL, NULL, (DIGCF_PRESENT | DIGCF_INTERFACEDEVICE));
    if ( hDevInfo == INVALID_HANDLE_VALUE ) 
	{
		printf("Oh-no,SetupDiGetClassDevs failed, the wrong is showing: %x\n", GetLastError());
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return false;
    }

	std::cout<<"I find the handle: "<<hDevInfo<<std::endl;

	//memset(&ifdata,0,sizeof(SP_DEVICE_INTERFACE_DATA));
	DeviceInterfaceData.cbSize=sizeof(SP_DEVICE_INTERFACE_DATA);
	DeviceInterfaceData.Flags = 0;
	
	while ( SetupDiEnumDeviceInterfaces(hDevInfo, NULL,&USB_Device_GUID, icount, &DeviceInterfaceData) )
	{
		icount++;

		SetupDiGetInterfaceDeviceDetail(hDevInfo, &DeviceInterfaceData, 0, 0, &requiredLength, 0);
		predictedLength = requiredLength; 

		devinfo.cbSize = sizeof (SP_DEVINFO_DATA);
		DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(predictedLength);

		if (DeviceInterfaceDetailData)
		{
			DeviceInterfaceDetailData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);

			if (!SetupDiGetInterfaceDeviceDetail(hDevInfo, &DeviceInterfaceData, DeviceInterfaceDetailData, predictedLength, 0, &devinfo) )
			{
				printf("You must do sth Error in SetupDiGetInterfaceDeviceDetail!\n");
				free (DeviceInterfaceDetailData);
				return false;
			}

			memset(driverkey, 0, 50);
			DWORD dataType;

			if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &devinfo, SPDRP_HARDWAREID, &dataType, (LPBYTE)driverkey, predictedLength, 0))
			{
				SetupDiDestroyDeviceInfoList(hDevInfo);
				return false;
			}

			_strupr_s(driverkey);

			if (strstr(driverkey, "VID_03F0") != NULL)
			{
				printf("I find the printer_path!\n");
				strcpy_s(PrinterPathName, DeviceInterfaceDetailData->DevicePath);
				free (DeviceInterfaceDetailData);
				SetupDiDestroyDeviceInfoList(hDevInfo);
				pSuccess = true;
			}
		}
		else
		{
			printf("I couldn't allocatr %d bytes for device interface details.\n", predictedLength);
			free (DeviceInterfaceDetailData);
			SetupDiDestroyDeviceInfoList(hDevInfo);
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (pSuccess)
	{
		printf("I'm opening %s\n",PrinterPathName);

		hHandle = CreateFile(PrinterPathName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hHandle != INVALID_HANDLE_VALUE)
		{
			printf("I open printer_port success!\n\n");
		}
		else
		{
			printf("I'm sorry!'CreateFile' can't open printer_port!\n\n");
		}

		printf("I'm doing a Write test...\n");

		BYTE* pOut = new BYTE[100];
		char* buf;
		buf = (char*)malloc(100);
		DWORD number;

		strcpy(buf,"\27E");
		strcat(buf,"\27&l1Xhehe");


		for (int i=0; i<14; i++)
			pOut[i] = 'Y';
		pOut[14] = 0x0D;
		pOut[15] = 0x0A;

		printf("Now, write start!\n");

		if (!WriteFile(hHandle, buf, strlen(buf), &number, NULL) )
		{
			printf("I'm sorry! write error!\n");
			return false;
		}
		printf("YES,write data ok!\n");
	}
	else
	{
		printf("Sorry! No device is exist!\nI'm living now...\ngoodbye sir!\n");
	}
	CloseHandle(hHandle);
	return 0;
}








		





/*	
	
	int i =0;

	while(1)
	{
		//ifdata.cbSize = sizeof(ifdata);
		
		BOOL bResult  = SetupDiEnumDeviceInterfaces(hDevInfo, NULL,&testguid,i, &ifdata);
		if(bResult == 0)
		{
        	SetupDiDestroyDeviceInfoList(hDevInfo);
			if(GetLastError() == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
		}
		else
		{
			length = 0;
			bResult = SetupDiGetInterfaceDeviceDetail(hDevInfo,&ifdata,NULL,0,&length,NULL);
			if(bResult==0)
			{
					if(ERROR_INSUFFICIENT_BUFFER != (GetLastError()))
					{
							i++;
							continue;
					}
			}
			DeviceInfoData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_ZEROINIT,length);
			if(NULL == DeviceInfoData)
			{
					SetupDiDestroyDeviceInfoList(hDevInfo);
					return 0;
			}

			DeviceInfoData->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA);
			if(!SetupDiGetInterfaceDeviceDetail(hDevInfo,&ifdata,DeviceInfoData,length,NULL,NULL))
			{
					LocalFree(DeviceInfoData);
					i++;
					continue;
			}

			std::cout<<DeviceInfoData->DevicePath<<std::endl;
			break;
			}
	}
	 //strcpy(interfacename,DeviceInfoData->DevicePath);
//      strcpy_s(interfacename, sizeof("\\\\.\\USB001"), "\\\\.\\USB001");
     HANDLE BradyUSB = CreateFile(DeviceInfoData->DevicePath,
                                     GENERIC_READ | GENERIC_WRITE,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_OVERLAPPED,
                                     NULL);
//	TRACE(interfacename);
//	TRACE("\n");
 
    if (BradyUSB == INVALID_HANDLE_VALUE)
	{
		char* mybuf;
        mybuf = (char*) malloc(128);
       // wsprintf(mybuf, "Did not open Brady printer");
        //AfxMessageBox(mybuf, MB_OK | MB_ICONEXCLAMATION, 0);
        free(mybuf);
        }
	else 
	{
		// Write to the printer
        OVERLAPPED olWrite = { 0 };
        olWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        char cCommand1, cCommand2, cTermChar, cAllCommand[5];
        DWORD dwBytesWritten;
        DWORD dwBytesToWrite;
        bool writeSuccess;
        cCommand1 = 0x01;
        cCommand2 = '#';
        cTermChar = '\n';

		strcpy(cAllCommand,"\x0D\x1B\x30");

        dwBytesToWrite = 3;
        if(!WriteFile(BradyUSB, cAllCommand, dwBytesToWrite, &dwBytesWritten, &olWrite)) 
		{
			// Wait until write operation is complete
            bool done = false;
            while (!done) 
			{
				WaitForSingleObject(olWrite.hEvent, INFINITE);
                if (!GetOverlappedResult(BradyUSB, &olWrite, &dwBytesWritten, FALSE)) 
				{
					if (!(GetLastError() == ERROR_IO_PENDING)) 
					{
						// Get out of loop and check for bytes written
                        done = true;
					} // IO Pending
				} else // GetOverlappedResult
					done = true;
			} // while loop
		} // write file
        if (dwBytesWritten == 0)
		{
			char* mywriteerrorbuf;
            mywriteerrorbuf = (char*) malloc(64);
           // wsprintf(mywriteerrorbuf, "Write error!");
           // AfxMessageBox(mywriteerrorbuf, MB_OK | MB_ICONEXCLAMATION, 0);
            free(mywriteerrorbuf);
            writeSuccess = false;
		} else
			writeSuccess = true;
		if (writeSuccess) 
		{  // Write was a success, let's read
			// Clear the error
            SetLastError(0);
            ResetEvent(olWrite.hEvent);
            char myreadbuf[10];
            OVERLAPPED olRead = { 0 };
            olRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
            DWORD dwBytesRead;
			myreadbuf[9] = '\0';

            if (!ReadFile(BradyUSB, &myreadbuf, 0x100, &dwBytesRead, &olRead)) 
			{
                    // Wait for read operation to complete
				int retval = WaitForSingleObject(olRead.hEvent,5000);
            }
            if (dwBytesRead == 0)
			{
				// Nothing was read.
                char* myreaderrorbuf;
                myreaderrorbuf = (char*) malloc(64);
               // wsprintf(myreaderrorbuf, "Nothing to read!");
                //AfxMessageBox(myreaderrorbuf, MB_OK | MB_ICONEXCLAMATION, 0);
                free(myreaderrorbuf);
			}
		} // Write success
	}  // Printer handle
    if (BradyUSB != INVALID_HANDLE_VALUE)
		CloseHandle(BradyUSB);
// for loop



	//delete(DeviceInfoData);
	(void)SetupDiDestroyDeviceInfoList(hDevInfo);
	return 0;
}

*/