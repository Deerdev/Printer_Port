#include "stdafx.h"
#include "windows.h"
#include "Setupapi.h"
#include "stdio.h"

static GUID GUID_DEVINTERFACE_USB_DEVICE =
{ 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };



int main(int argc, char* argv[])
{
char szTraceBuf[256];
// Get device interface info set handle for all devices attached to system
HDEVINFO hDevInfo = SetupDiGetClassDevs(
&GUID_DEVINTERFACE_USB_DEVICE,
NULL,
NULL,
DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
);

if (hDevInfo == INVALID_HANDLE_VALUE)
{
sprintf(szTraceBuf, "SetupDiClassDevs() failed. GetLastError() " \
"returns: 0x%x\n", GetLastError());
OutputDebugString(szTraceBuf);
printf("SetupDiClassDevs() failed. GetLastError() " \
"returns: 0x%x\n", GetLastError());
return 1;
}

sprintf(szTraceBuf, "Device info set handle for all devices attached to " \
"system: 0x%x\n", hDevInfo);
OutputDebugString(szTraceBuf);
printf("Device info set handle for all devices attached to " \
"system: 0x%x\n", hDevInfo);

// Retrieve a context structure for a device interface of a device
// information set.
DWORD dwIndex = 0;
SP_DEVICE_INTERFACE_DATA devInterfaceData;
ZeroMemory(&devInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
BOOL bRet = FALSE;
while(TRUE)
{
bRet = SetupDiEnumDeviceInterfaces(
hDevInfo,
NULL,
&GUID_DEVINTERFACE_USB_DEVICE,
dwIndex,
&devInterfaceData
);
if (!bRet)
{
sprintf(szTraceBuf, "SetupDiEnumDeviceInterfaces failed " \
"GetLastError() returns: 0x%x\n", GetLastError());
OutputDebugString(szTraceBuf);
printf("SetupDiEnumDeviceInterfaces failed " \
"GetLastError() returns: 0x%x\n", GetLastError());

if (GetLastError() == ERROR_NO_MORE_ITEMS)
{
break;
}
}

dwIndex++;
}

sprintf(szTraceBuf, "Number of device interface sets representing all " \
"devices attached to system: 0x%x\n", dwIndex);
OutputDebugString(szTraceBuf);
printf("Number of device interface sets representing all " \
"devices attached to system: 0x%x\n", dwIndex);

SetupDiDestroyDeviceInfoList(hDevInfo);

return 0;
}

