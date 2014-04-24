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
//��������ڵ�ǰλ�õĴ�ӡλ��
//��λ1/96Ӣ��
void OffSetPosXY(HANDLE hCom ,int x , int y);

//
//��ֽ����
//ע�⣬��ֽ�����ֵ�趨���ܴ��ڵ�ǰ��Y����
//Ҳ����˵���ı�ҳ��������£���ֽ�������˵���һҳ��ȥ
//��λ1/96Ӣ��
void BackScrollPosY(HANDLE hCom , int y);

//
//��ҳ
//
void NextPage(HANDLE hCom);

//
//����ҳ�߶�
//
//����3��ȡֵ0 �� 1 �� 2���ֱ��ʾ��Ӣ�磬�У��Զ��嵥λ����ҳ��
//0��ʾӢ��Ϊ��λ(0~22)		
//1��ʾ��Ϊ��λ(0~127)	
//2��ʾ�Զ��嵥λ, 
//������3Ϊ2ʱ������4��Ч
//����4ȡֵ10 ��20 �� 30 ��40 �� 50 �� 60
void SetPageHeight(HANDLE hCom ,int pHeight , char unit , int scale);

//
//��ӡ�ַ���
//
void PrintStr(HANDLE hCom , LPCTSTR str);


//
//��ʼ�������������,�ָ�Ĭ������
//
void PurgePrintSetting(HANDLE hCom);

//
//����
//
void NextLine(HANDLE hCom);

//
//�����趨
//0Ϊ���壬1Ϊ����
void SetFont(HANDLE hCom , BOOL han);

//
//����Ŵ�2��
//
void SetWordSizeTwice(HANDLE hCom );

//
//���2������
//
void RemoveWordSizeTwice(HANDLE hCom );

//
//�趨�ַ���ָ࣬����1B 20 n(0~127)����λ1/180Ӣ�硣Ĭ��0
//��ǰ���ǵ�ǰ��Ӣ��ģʽ
void SetCHSpace(HANDLE hCom ,int nSpace);

//
//���û�Ĭ���ַ���ָ࣬����1B 20 n(0~127)����λ1/180Ӣ�硣Ĭ��0
//�������ַ����Ϊ0�������û�����ģʽ
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
	char                                PrinterPathName[255];    //��ӡ��·��
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
//��������ڵ�ǰλ�õĴ�ӡλ��
//
void OffSetPosXY(HANDLE hCom ,int x , int y)//��λ1/60Ӣ��
{
	unsigned long cnt = 0;

	//�ȴ���Y���꣬��X����
	char YPos[3] = {0x1B , 0x4A , 0x00};
	int LineNum = 0; //��Ҫ�����ٸ�85/60Ӣ��		//����
	int LineRem = 0; //��Ҫ�����ٸ�1/60Ӣ��			//С�鲽
	y = y * 3;

	//�����ǰ����������255����λ1/180Ӣ�磩������Ҫ���λ��λ���
	if (y > 255)
	{
		LineNum = y / 255;
		LineRem = y % 255;
	}
	else//û����һ����
	{
		LineRem = y;
	}
	for (int i = 0 ; i < LineNum ; i++)//����������
	{
		YPos[2] = 255;
		::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);
	}
	YPos[2] = LineRem;					//��С�鲽
	::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);

	//X����
	char XPos[4] = {0x1B , 0x24 , 0x00 , 0x00};
	XPos[3] = x / 255;
	XPos[2] = x % 255;
	::WriteFile(hCom , XPos , sizeof(XPos) ,&cnt , 0);
}

//
//��ֽ����
//ע�⣬��ֽ�����ֵ�趨���ܴ��ڵ�ǰ��Y����
//Ҳ����˵���ı�ҳ��������£���ֽ�������˵���һҳ��ȥ
//
void BackScrollPosY(HANDLE hCom , int y)
{
	unsigned long cnt = 0;

	//�ȴ���Y���꣬��X����
	char YPos[3] = {0x1B , 0x6A , 0x00};
	int LineNum = 0; //��Ҫ�����ٸ�85/60Ӣ��		//����
	int LineRem = 0; //��Ҫ�����ٸ�1/60Ӣ��			//С�鲽
	y = y * 3;

	//�����ǰ����������255����λ1/180Ӣ�磩������Ҫ���λ��λ���
	if (y > 255)
	{
		LineNum = y / 255;
		LineRem = y % 255;
	}
	else//û����һ����
	{
		LineRem = y;
	}
	for (int i = 0 ; i < LineNum ; i++)//����������
	{
		YPos[2] = 255;
		::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);
	}
	YPos[2] = LineRem;					//��С�鲽
	::WriteFile(hCom , YPos , sizeof(YPos) ,&cnt , 0);

}

//
//��ҳ
//
void NextPage(HANDLE hCom)
{
	unsigned long cnt = 0;
	char chNextPage[] = {0x0C};
	::WriteFile(hCom , chNextPage , (DWORD)1L , &cnt , 0);
}

//
//����ҳ�߶�
//
//����3��ȡֵ0 �� 1 �� 2���ֱ��ʾ��Ӣ�磬�У��Զ��嵥λ����ҳ��
//0��ʾӢ��Ϊ��λ(0~22)		
//1��ʾ��Ϊ��λ(0~127)	
//2��ʾ�Զ��嵥λ, 
//������3Ϊ2ʱ������4��Ч
//����4ȡֵ10 ��20 �� 30 ��40 �� 50 �� 60
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
//��ӡ�ַ���
//
void PrintStr(HANDLE hCom , LPCTSTR str)
{
	unsigned long cnt = 0;
	//char* pPrint = new char[str.GetLength()];
	int len = _tcslen(str);
	char* pPrint = new char[len];
	pPrint = (char* )str;
	::WriteFile(hCom , pPrint , (DWORD)len , &cnt , 0);

	unsigned char chEnterCode[] = { 0x0D };//������һ���س�����ʼ��ӡ
	::WriteFile(hCom , chEnterCode , sizeof(chEnterCode) ,&cnt , 0);
}


//
//��ʼ�������������,�ָ�Ĭ������
//
void PurgePrintSetting(HANDLE hCom)
{
	unsigned long cnt = 0;
	unsigned char chInitCode[] = { 0x0D , 0x1B , 0x40 , 0x18};//��ӡ����ʼ��,���������
	::WriteFile(hCom , chInitCode , sizeof(chInitCode) ,&cnt , 0);
}

//
//����
//
void NextLine(HANDLE hCom)
{
	unsigned long cnt = 0;
	unsigned char chNextLine[] = { 0x0A };//����
	::WriteFile(hCom , chNextLine , sizeof(chNextLine) ,&cnt , 0);
}

//
//�����趨
//
void SetFont(HANDLE hCom , BOOL han)//0Ϊ���壬1Ϊ����
{
	unsigned long cnt = 0;
	if (0 == han)//����
	{
		unsigned char chFontSong[] = { 0x1C , 0x6B , 0x00 };
		::WriteFile(hCom , chFontSong , sizeof(chFontSong) ,&cnt , 0);
	}
	else//����
	{
		unsigned char chFontHei[] = { 0x1C , 0x6B , 0x01 };
		::WriteFile(hCom , chFontHei , sizeof(chFontHei) ,&cnt , 0);
	}

}

//
//����Ŵ�2��
//
void SetWordSizeTwice(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chFontDouble[] = { 0x1B , 0x49 , 0x44 ,0x0D};
	::WriteFile(hCom , chFontDouble , sizeof(chFontDouble) ,&cnt , 0);
}

//
//���2������
//
void RemoveWordSizeTwice(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chFontDouble[] = { 0x1B , 0x49 , 0x41 ,0x0D};
	::WriteFile(hCom , chFontDouble , sizeof(chFontDouble) ,&cnt , 0);
}

//
//�趨�ַ���ָ࣬����1B 20 n(0~127)����λ1/180Ӣ�硣Ĭ��0
//�����ó�Ӣ��ģʽ
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
//���û�Ĭ���ַ���ָ࣬����1B 20 n(0~127)����λ1/180Ӣ�硣Ĭ��0
//�������ַ����Ϊ0�������û�����ģʽ
void SetDefaultCHSpace(HANDLE hCom )
{
	unsigned long cnt = 0;
	unsigned char chSpaceCmd[] = { 0x1B , 0x20 , 0x0};
	::WriteFile(hCom , chSpaceCmd , sizeof(chSpaceCmd) ,&cnt , 0);

	unsigned char HanMode[] = {0x1C , 0x26};
	::WriteFile(hCom , HanMode , sizeof(HanMode) ,&cnt , 0);
}