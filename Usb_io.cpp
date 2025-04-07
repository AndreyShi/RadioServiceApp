#include "stdafx.h"
#include <winerror.h>
//ERROR_IO_PENDING
GUID Guid;
wchar_t DevicePath[MAX_PATH];

struct PIPE_ID
{
	UCHAR  PipeInId;
	UCHAR  PipeOutId;
};
WINUSB_INTERFACE_HANDLE InterfaceHandle;


#define UNKNOWN_DEVICE    L"vid_0483"
#define STM32_USBVID      L"vid_03eb"
#define CYPRESS_BOARD     L"vid_04b4"

bool get_path(const wchar_t* device)
{
	char	Product[253];
	std::string	Prod;
	bool result = 0;

	HDEVINFO info;
	//HidD_GetHidGuid(&Guid);
	//{A5DCBF10 - 6530 - 11D2 - 901F - 00C04FB951ED} GUID_DEVINTERFACE_USB_DEVICE and Cypress board
	Guid.Data1 = 0xA5DCBF10;
	Guid.Data2 = 0x6530;
	Guid.Data3 = 0x11D2;

	Guid.Data4[0] = 0x90;
	Guid.Data4[1] = 0x1F;

	Guid.Data4[2] = 0x00;
	Guid.Data4[3] = 0xC0;
	Guid.Data4[4] = 0x4F;
	Guid.Data4[5] = 0xB9;
	Guid.Data4[6] = 0x51;
	Guid.Data4[7] = 0xED;
	//ClassGUID = { 36FC9E60 - C465 - 11CF - 8056 - 444553540000 } check
	Guid.Data1 = 0x36FC9E60;
	Guid.Data2 = 0xC465;
	Guid.Data3 = 0x11CF;

	Guid.Data4[0] = 0x80;
	Guid.Data4[1] = 0x56;

	Guid.Data4[2] = 0x44;
	Guid.Data4[3] = 0x45;
	Guid.Data4[4] = 0x53;
	Guid.Data4[5] = 0x54;
	Guid.Data4[6] = 0x00;
	Guid.Data4[7] = 0x00;
	//CYUSB3.GUID = "{AE18AA60-7F6A-11d4-97DD-00010229B959}" check
	Guid.Data1 = 0xAE18AA60;
	Guid.Data2 = 0x7F6A;
	Guid.Data3 = 0x11d4;

	Guid.Data4[0] = 0x97;
	Guid.Data4[1] = 0xDD;

	Guid.Data4[2] = 0x00;
	Guid.Data4[3] = 0x01;
	Guid.Data4[4] = 0x02;
	Guid.Data4[5] = 0x29;
	Guid.Data4[6] = 0xB9;
	Guid.Data4[7] = 0x59;
	//fbb76182-a06d-4f02-9ac4-791289eaf32c RsUSB.inf
	//this GUID i took from end of RsUSB.inf file, it is not class GUID 
	Guid.Data1 = 0xfbb76182;
	Guid.Data2 = 0xa06d;
	Guid.Data3 = 0x4f02;

	Guid.Data4[0] = 0x9a;
	Guid.Data4[1] = 0xc4;

	Guid.Data4[2] = 0x79;
	Guid.Data4[3] = 0x12;
	Guid.Data4[4] = 0x89;
	Guid.Data4[5] = 0xea;
	Guid.Data4[6] = 0xf3;
	Guid.Data4[7] = 0x2c;






	info = SetupDiGetClassDevs(&Guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (info != INVALID_HANDLE_VALUE)
	{
		DWORD devIndex;
		SP_INTERFACE_DEVICE_DATA ifData;
		ifData.cbSize = sizeof(ifData);

		for (devIndex = 0; SetupDiEnumDeviceInterfaces(info, NULL, &Guid, devIndex, &ifData); ++devIndex)
		{
			DWORD needed;
			SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &needed, NULL);

			PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
			detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			SP_DEVINFO_DATA did = { sizeof(SP_DEVINFO_DATA) };

			if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, needed, NULL, &did))
			{
				wprintf(L"%s\n", detail->DevicePath);
				if (wcsstr(detail->DevicePath, UNKNOWN_DEVICE) || wcsstr(detail->DevicePath, device))
				{
					memcpy(DevicePath, detail->DevicePath, MAX_PATH * 2);
					result = 1;
				}
			}

			if (wcsstr(detail->DevicePath, UNKNOWN_DEVICE) != NULL)
			{
				if (SetupDiGetDeviceRegistryProperty(info, &did, SPDRP_DEVICEDESC, NULL, (PBYTE)Product, 253, NULL))
					Prod = Product;
				else
					Prod = "(Unnamed HID device)";
			}
			delete[](PBYTE)detail;
		}
		SetupDiDestroyDeviceInfoList(info);
	}
	return result;
}

BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
	ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

	WINUSB_PIPE_INFORMATION  Pipe;
	ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

	bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

	if (bResult)
	{
		for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
		{
			bResult = WinUsb_QueryPipe(hDeviceHandle, 0, index, &Pipe);

			if (bResult)
			{
				if (Pipe.PipeType == UsbdPipeTypeControl)
				{
					printf("Endpoint index: %d Pipe type: Control Pipe %d ID: 0x%02X MaxPktSize: %d.\n", index, Pipe.PipeType, Pipe.PipeId, Pipe.MaximumPacketSize);
				}

				if (Pipe.PipeType == UsbdPipeTypeIsochronous)
				{
					printf("Endpoint index: %d Pipe type: Isochronous Pipe %d ID: 0x%02X MaxPktSize: %d.\n", index, Pipe.PipeType, Pipe.PipeId, Pipe.MaximumPacketSize);
				}

				if (Pipe.PipeType == UsbdPipeTypeBulk)
				{
					if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe IN  %d ID: 0x%02X MaxPktSize: %d.\n", index, Pipe.PipeType, Pipe.PipeId, Pipe.MaximumPacketSize);
						pipeid->PipeInId = Pipe.PipeId;
					}

					if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe OUT %d ID: 0x%02X MaxPktSize: %d.\n", index, Pipe.PipeType, Pipe.PipeId, Pipe.MaximumPacketSize);
						pipeid->PipeOutId = Pipe.PipeId;
					}
				}

				if (Pipe.PipeType == UsbdPipeTypeInterrupt)
				{
					printf("Endpoint index: %d Pipe type: Interrupt Pipe %d ID: 0x%02X MaxPktSize: %d.\n", index, Pipe.PipeType, Pipe.PipeId, Pipe.MaximumPacketSize);
				}
			}
			else
			{
				continue;
			}
		}
	}


	return bResult;
}

int c;

int InitUsb(void){
	setlocale(LC_ALL, "en_US.UTF-8");
	const char* p[] = { "Test WinUsb\n" };
	const wchar_t* pj[] = { _T("Привет!\n") };

	printf("%s", *p);
	int gp = get_path(CYPRESS_BOARD);
	printf("get_path: %d\n", gp);
	if (gp != 0){
		wprintf(L"DevicePath %s\n", DevicePath);
	}
	HANDLE DeviceHandle;
	DeviceHandle = CreateFile((LPCTSTR)DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("InvalidHandle\n");
		//c = _getch();
		return 0;
	}
	printf("DeviceHandle: %d\n", DeviceHandle);
	//BYTE data[] = { 0x01, 0x02 };
	//DWORD NumberOfBytesWriten = 0;
	//OVERLAPPED	Overlapped;
	//WriteFile(DeviceHandle, data, 2, &NumberOfBytesWriten, &Overlapped);
	//printf("NumberOfBytesWriten: %d\n", NumberOfBytesWriten);
	Sleep(1000);


	WinUsb_Initialize(DeviceHandle, &InterfaceHandle);
	int er = GetLastError();
	/*
	i have got ERROR_NOT_ENOUGH_MEMORY when Cypress driver installed in system, WinUsb driver install resolve it.
	If we use WinUsb API we need install WinUsb driver with current VIDPID device,
	if we use CyUsb API we need install Cypress Usb driver with current VIDPID device
	*/
	printf("InterfaceHandle: %d  LastError: %d\n", InterfaceHandle, er);
	PIPE_ID pipeid;
	QueryDeviceEndpoints(InterfaceHandle, &pipeid);
	//c = _getch();
	return 0;
}

void SetupUsbOUT_init(void){

	WINUSB_SETUP_PACKET wxb = { 0 };
	wxb.RequestType = 0x40;   //byte
	wxb.Request     = 0xae;   //byte
	wxb.Value       = 0;      //uint16
	wxb.Index       = 0x06;   //uint16
	wxb.Length      = 13;     //uint16
	UCHAR buf_s[13] = { 0x05, 0x0D, 0x09, 0x07, 0x05, 0x07, 0x22, 0x08, 0x40, 0x1F, 0x6b, 0x00, 0xDE };
	ULONG transferred_s = 0;

	printf("sending packet OUT init, ReqT:0x%02x Req:0x%02x Index:%d Value:%d Length:%d\n", wxb.RequestType, wxb.Request, wxb.Index, wxb.Value, wxb.Length);
	for (int i = 0; i < 13; i++)
	    { printf(" 0x%02x", buf_s[i]);}
	printf("\n");

	BOOL res_s = WinUsb_ControlTransfer(InterfaceHandle, wxb, buf_s, 13, &transferred_s, NULL);
	if (res_s == TRUE)
	{
		printf("WinUsb_ControlTransfer transferred: %d\n", transferred_s);
	}
	else
	{
		printf("WinUsb_ControlTransfer GetLastError: %d\n", GetLastError());
	}
}

void SetupUsbIN(UINT8 ReqT, UINT8 Req, UINT16 Value, UINT16 Index, UINT16 Length) {

	WINUSB_SETUP_PACKET wxb = { 0 };
	wxb.RequestType = ReqT;       //byte
	wxb.Request     = Req;        //byte
	wxb.Value       = Value;      //uint16
	wxb.Index       = Index;      //uint16
	wxb.Length      = Length;     //uint16
	UCHAR buf_s[64] = { 0 };
	ULONG rcv = 0;

	printf("sending packet IN, ReqT:0x%02x Req:0x%02x Index:%d Value:%d Length:%d\n", wxb.RequestType, wxb.Request, wxb.Index, wxb.Value, wxb.Length);
	BOOL res_s = WinUsb_ControlTransfer(InterfaceHandle, wxb, buf_s, Length, &rcv, NULL);
	if (res_s == TRUE)
	{
		printf("WinUsb_ControlTransfer recieved: %d byt:", rcv);
		for (int i = 0; i < rcv; i++)
		     { printf(" 0x%02x", buf_s[i]);}
		printf("...\n");
	}
	else
	{
		printf("WinUsb_ControlTransfer GetLastError: %d\n", GetLastError());
	}
}

void SetupUsbOUT_empty(void){
	WINUSB_SETUP_PACKET wxb = { 0 };
	wxb.RequestType = 0x40;   //byte
	wxb.Request =     0xaa;   //byte
	wxb.Index =       0;      //uint16
	wxb.Value =       0;      //uint16
	wxb.Length =      0;      //uint16
	UCHAR buf_s = 0;
	ULONG transferred_s = 0;

	printf("sending packet OUT empty, ReqT:0x%02x Req:0x%02x Index:%d Value:%d Length:%d\n", wxb.RequestType, wxb.Request, wxb.Index, wxb.Value, wxb.Length);

	BOOL res_s = WinUsb_ControlTransfer(InterfaceHandle, wxb, &buf_s, 0, &transferred_s, NULL);
	if (res_s == TRUE)
	{
		printf("WinUsb_ControlTransfer transferred: %d\n", transferred_s);
	}
	else
	{
		printf("WinUsb_ControlTransfer GetLastError: %d\n", GetLastError());
	}
}

void SetupUsbOUT_settings(device_data dt){

	WINUSB_SETUP_PACKET wxb = { 0 };
	wxb.RequestType = 0x40;   //byte
	wxb.Request     = 0xab;   //byte
	wxb.Value       = 0x06;   //uint16
	wxb.Index       = 0;      //uint16
	wxb.Length      = 10;     //uint16

	UINT8 lbst = dt.start_freq;
	UINT8 hbst = dt.start_freq >> 8;

	UINT8 lben = dt.end_freq;
	UINT8 hben = dt.end_freq >> 8;

	UCHAR buf_s[10] = { 0x00, 0x01, lbst, hbst, lben, hben, 0x01, dt.attenua, 0x3F, 0x3F };
	ULONG transferred_s = 0;

	printf("sending packet OUT settings, ReqT:0x%02x Req:0x%02x Index:%d Value:%d Length:%d\n", wxb.RequestType, wxb.Request, wxb.Index, wxb.Value, wxb.Length);
	for (int i = 0; i < 10; i++)
	    { printf(" 0x%02x", buf_s[i]);}
	printf("\n");

	BOOL res_s = WinUsb_ControlTransfer(InterfaceHandle, wxb, buf_s, 10, &transferred_s, NULL);
	if (res_s == TRUE)
	{
		printf("WinUsb_ControlTransfer transferred: %d\n", transferred_s);
	}
	else
	{
		printf("WinUsb_ControlTransfer GetLastError: %d\n", GetLastError());
	}
}

/*Summary
The looping Bulk Xfer Thread function

The looping xfers need to run in a separate thread so as to be stoppable
via a click on the "Stop" button.  Otherwise, that click will never get
serviced and the app will loop indefinitely.

The "Start" button on click handler just calls this routine, passing a pointer
to the dialog (main window) object.  That way, this routine has access to all
the public members of the CBulkLoopDlg class.
*/

UINT XferLoop(LPVOID params) {

	CRadioServiceAppDlg *dlg = (CRadioServiceAppDlg *)params;
	printf("start XferLoop\n");
	for (int i = 0; dlg->bLooping && i < dlg->loops; i++) {
		UCHAR buf[1024] = { 0 };
		read_usb_sync(0x82, buf, 1024);
	}
	printf("end XferLoop\n");
	dlg->XferThread = NULL;
	return true;
}

void abort_pipe(UINT8 pipeId){
	WinUsb_AbortPipe(InterfaceHandle, pipeId);
}

void reset_pipe(UINT8 pipeId){
	WinUsb_ResetPipe(InterfaceHandle, pipeId);
}

void write_usb(UINT8 pipeId, UINT8* buf, int max_cnt){

	ULONG transferred = 0;
	BOOL res = WinUsb_WritePipe(InterfaceHandle, pipeId, buf, max_cnt, &transferred, NULL);
	if (res == TRUE)
	{
		printf("WinUsb_WritePipe transferred: %d\n", transferred);
	}
	else
	{
		printf("WinUsb_WritePipe GetLastError: %d\n", GetLastError());
	}
}
void read_usb_sync(UINT8 pipeId, UINT8* buf, int max_cnt){
	ULONG recieved = 0;
	BOOL res_r = WinUsb_ReadPipe(InterfaceHandle, pipeId, buf, max_cnt, &recieved, NULL);
	if (res_r == TRUE)
	{
		printf("WinUsb_ReadPipe recieved: %d byt rev:", recieved);
		int print_view_bytes = 10;
		if (max_cnt < print_view_bytes)
		    { print_view_bytes = max_cnt;}
		for (int i = 0; i < print_view_bytes; i++)
		    { printf(" 0x%02x", buf[i]);}
		printf("...\n");
	}
	else
	{
		printf("WinUsb_ReadPipe GetLastError: %d\n", GetLastError());
		WinUsb_AbortPipe(InterfaceHandle, pipeId);
	}
}

void read_usb_async(UINT8 pipeId, UINT8* buf, int max_cnt) {

	ULONG recieved = 0;
	DWORD res_wait;
	OVERLAPPED over = { 0 };
	over.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (over.hEvent == NULL)
	{
		printf("CreateEvent GetLastError(): %d\n", GetLastError());
	}
	else
	{
		BOOL res_r = WinUsb_ReadPipe(InterfaceHandle, pipeId, buf, max_cnt, NULL, &over);
		res_wait = WaitForSingleObject(over.hEvent, 1000);

		if (res_wait == WAIT_OBJECT_0) //object was signaled
		{
			printf("event occured res_wait: %d\n", res_wait);
			BOOL res_over = WinUsb_GetOverlappedResult(InterfaceHandle, &over, &recieved, TRUE);//get recieved cnt bytes
			if (res_over == 0)
			{
				printf("WinUsb_GetOverlappedResult GetLastError: %d  recieved %d\n", GetLastError(), recieved);
			}
			else
			{
				printf("WinUsb_ReadPipe recieved: %d byt rev:", recieved);
				int print_view_bytes = 10;
				if (max_cnt < print_view_bytes)
				    { print_view_bytes = max_cnt;}
				for (int i = 0; i < print_view_bytes; i++)
				    { printf(" 0x%02x", buf[i]);}
				printf("...\n");
			}
		}
		else
		{
			//if we catch TIMEOUT usb is bagged until unplug/plug device(testin in VirtualBox WinXp Usb Cypress Board)
			printf("WaitForSingleObject TIMEOUT res_wait: %d  GetLastError(): %d \n", res_wait, GetLastError());
			WinUsb_AbortPipe(InterfaceHandle, pipeId);
		}
		CloseHandle(over.hEvent);
	}
}