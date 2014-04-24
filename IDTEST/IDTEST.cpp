#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <atlstr.h>
#pragma comment(lib,"Setupapi.lib")

#define INTERFACE_DETAIL_SIZE (1024)



//
//设置相对于当前位置的打印位置
//单位1/96英寸
void OffSetPosXY(HANDLE hCom ,int x , int y);

//
//退纸命令
//注意，退纸的最大值设定不能大于当前的Y坐标
//也就是说不改变页长的情况下，退纸不可能退到上一页中去
//单位1/96英寸
void BackScrollPosY(HANDLE hCom , int y);

//
//换页
//
void NextPage(HANDLE hCom);

//
//设置页高度
//
//参数3的取值0 ， 1 ， 2，分别表示按英寸，行，自定义单位设置页长
//0表示英寸为单位(0~22)		
//1表示行为单位(0~127)	
//2表示自定义单位, 
//当参数3为2时，参数4有效
//参数4取值10 ，20 ， 30 ，40 ， 50 ， 60
void SetPageHeight(HANDLE hCom ,int pHeight , char unit , int scale);

//
//打印字符串
//
void PrintStr(HANDLE hCom , LPCTSTR str);


//
//初始化并清除缓冲区,恢复默认设置
//
void PurgePrintSetting(HANDLE hCom);

//
//换行
//
void NextLine(HANDLE hCom);

//
//字体设定
//0为宋体，1为黑体
void SetFont(HANDLE hCom , BOOL han);

//
//字体放大2倍
//
void SetWordSizeTwice(HANDLE hCom );

//
//解除2倍字体
//
void RemoveWordSizeTwice(HANDLE hCom );

//
//设定字符间距，指令是1B 20 n(0~127)，单位1/180英寸。默认0
//但前提是当前是英文模式
void SetCHSpace(HANDLE hCom ,int nSpace);

//
//设置回默认字符间距，指令是1B 20 n(0~127)，单位1/180英寸。默认0
//即设置字符间距为0，在设置回中文模式
void SetDefaultCHSpace(HANDLE hCom );


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
	char                                PrinterPathName[255];    //打印机路径
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
/*
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
*/








		PurgePrintSetting(hHandle);
		SetPageHeight(hHandle,127,1,0);
		SetDefaultCHSpace(hHandle);
		SetFont(hHandle,1);
		PrintStr(hHandle,"i want to contact you HP");
		printf("YES,write data ok!\n");
	}
	else
	{
		printf("Sorry! No device is exist!\nI'm living now...\ngoodbye sir!\n");
	}
	CloseHandle(hHandle);
	return 0;
}



//
//设置相对于当前位置的打印位置
//
void OffSetPosXY(HANDLE hCom ,int x , int y)//单位1/60英寸
{
	unsigned long cnt = 0;

	//先处理Y坐标，在X坐标
	char YPos[3] = {0x1B , 0x4A , 0x00};
	int LineNum = 0; //需要换多少个85/60英寸		//整步
	int LineRem = 0; //需要换多少个1/60英寸			//小碎步
	y = y * 3;

	//如果当前换行数超过255（单位1/180英寸），则需要两次或多次换行
	if (y > 255)
	{
		LineNum = y / 255;
		LineRem = y % 255;
	}
	else//没超过一整步
	{
		LineRem = y;
	}
	for (int i = 0 ; i < LineNum ; i++)//换几个整步
	{
		YPos[2] = 255;
		::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);
	}
	YPos[2] = LineRem;					//换小碎步
	::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);

	//X坐标
	char XPos[4] = {0x1B , 0x24 , 0x00 , 0x00};
	XPos[3] = x / 255;
	XPos[2] = x % 255;
	::WriteFile(hCom , XPos , sizeof(XPos) ,&cnt , 0);
}

//
//退纸命令
//注意，退纸的最大值设定不能大于当前的Y坐标
//也就是说不改变页长的情况下，退纸不可能退到上一页中去
//
void BackScrollPosY(HANDLE hCom , int y)
{
	unsigned long cnt = 0;

	//先处理Y坐标，在X坐标
	char YPos[3] = {0x1B , 0x6A , 0x00};
	int LineNum = 0; //需要换多少个85/60英寸		//整步
	int LineRem = 0; //需要换多少个1/60英寸			//小碎步
	y = y * 3;

	//如果当前换行数超过255（单位1/180英寸），则需要两次或多次换行
	if (y > 255)
	{
		LineNum = y / 255;
		LineRem = y % 255;
	}
	else//没超过一整步
	{
		LineRem = y;
	}
	for (int i = 0 ; i < LineNum ; i++)//换几个整步
	{
		YPos[2] = 255;
		::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);
	}
	YPos[2] = LineRem;					//换小碎步
	::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);

}

//
//换页
//
void NextPage(HANDLE hCom)
{
	unsigned long cnt = 0;
	char chNextPage[] = {0x0C};
	::WriteFile(hCom , chNextPage , (DWORD)1L , &cnt , 0);
}

//
//设置页高度
//
//参数3的取值0 ， 1 ， 2，分别表示按英寸，行，自定义单位设置页长
//0表示英寸为单位(0~22)		
//1表示行为单位(0~127)	
//2表示自定义单位, 
//当参数3为2时，参数4有效
//参数4取值10 ，20 ， 30 ，40 ， 50 ， 60
void SetPageHeight(HANDLE hCom ,int pHeight , char unit , int scale)
{
	unsigned long cnt = 0;

	if (0 == unit)//inch
	{
		char pageHeightInch[4] = {0x1B , 0x43 , 0x00 , 0x00};//0x00~0x16(0~22)
		pageHeightInch[3] = pHeight;
		::WriteFile(hCom , pageHeightInch , sizeof(pageHeightInch) ,&cnt , 0);
	}

	else if (1 == unit)//line
	{
		char pageHeightLine[3] = {0x1B , 0x43 , 0x00};//0x00~0x7F(0~127)
		pageHeightLine[2] = pHeight;
		::WriteFile(hCom , pageHeightLine , sizeof(pageHeightLine) ,&cnt , 0);
	}
	else if (2 == unit) //custom
	{
		char pageHeightCustomScale[] = {0x1B , 0x28 , 0x55 , 0x01 , 0 , 0};
		pageHeightCustomScale[5] = scale;
		::WriteFile(hCom , pageHeightCustomScale , sizeof(pageHeightCustomScale) ,&cnt , 0);

		char pageHeighCustom[] = {0x1B , 0x28 , 0x43 , 0x2 , 0 , 0x0 , 0x00};
		pageHeighCustom[5] = pHeight % 255;
		pageHeighCustom[6] = pHeight / 255;
		::WriteFile(hCom , pageHeighCustom , sizeof(pageHeighCustom) ,&cnt , 0);
	}

}

//
//打印字符串
//
void PrintStr(HANDLE hCom , LPCTSTR str)
{
	unsigned long cnt = 0;
	//char* pPrint = new char[str.GetLength()];
	int len = _tcslen(str);
	char* pPrint = new char[len];
	pPrint = (char* )str;
	::WriteFile(hCom , pPrint , (DWORD)len , &cnt , 0);

	unsigned char chEnterCode[] = { 0x0D };//在输入一个回车，开始打印
	::WriteFile(hCom , chEnterCode , sizeof(chEnterCode) ,&cnt , 0);
}


//
//初始化并清除缓冲区,恢复默认设置
//
void PurgePrintSetting(HANDLE hCom)
{
	unsigned long cnt = 0;
	unsigned char chInitCode[] = { 0x0D , 0x1B , 0x40 , 0x18};//打印机初始化,清除缓冲区
	::WriteFile(hCom , chInitCode , sizeof(chInitCode) ,&cnt , 0);
}

//
//换行
//
void NextLine(HANDLE hCom)
{
	unsigned long cnt = 0;
	unsigned char chNextLine[] = { 0x0A };//换行
	::WriteFile(hCom , chNextLine , sizeof(chNextLine) ,&cnt , 0);
}

//
//字体设定
//
void SetFont(HANDLE hCom , BOOL han)//0为宋体，1为黑体
{
	unsigned long cnt = 0;
	if (0 == han)//宋体
	{
		unsigned char chFontSong[] = { 0x1C , 0x6B , 0x00 };
		::WriteFile(hCom , chFontSong , sizeof(chFontSong) ,&cnt , 0);
	}
	else//黑体
	{
		unsigned char chFontHei[] = { 0x1C , 0x6B , 0x01 };
		::WriteFile(hCom , chFontHei , sizeof(chFontHei) ,&cnt , 0);
	}

}

//
//字体放大2倍
//
void SetWordSizeTwice(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chFontDouble[] = { 0x1B , 0x49 , 0x44 ,0x0D};
	::WriteFile(hCom , chFontDouble , sizeof(chFontDouble) ,&cnt , 0);
}

//
//解除2倍字体
//
void RemoveWordSizeTwice(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chFontDouble[] = { 0x1B , 0x49 , 0x41 ,0x0D};
	::WriteFile(hCom , chFontDouble , sizeof(chFontDouble) ,&cnt , 0);
}

//
//设定字符间距，指令是1B 20 n(0~127)，单位1/180英寸。默认0
//先设置成英文模式
void SetCHSpace(HANDLE hCom ,int nSpace)
{
	unsigned long cnt = 0;
	unsigned char EnMode[] = {0x1C , 0x2E};
	::WriteFile(hCom , EnMode , sizeof(EnMode) ,&cnt , 0);

	unsigned char chSpaceCmd[] = { 0x1B , 0x20 , 0x0};
	chSpaceCmd[2] = nSpace;
	::WriteFile(hCom , chSpaceCmd , sizeof(chSpaceCmd) ,&cnt , 0);
}

//
//设置回默认字符间距，指令是1B 20 n(0~127)，单位1/180英寸。默认0
//即设置字符间距为0，在设置回中文模式
void SetDefaultCHSpace(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chSpaceCmd[] = { 0x1B , 0x20 , 0x0};
	::WriteFile(hCom , chSpaceCmd , sizeof(chSpaceCmd) ,&cnt , 0);

	unsigned char HanMode[] = {0x1C , 0x26};
	::WriteFile(hCom , HanMode , sizeof(HanMode) ,&cnt , 0);
}