#include "stdafx.h"
#include <winerror.h>
//ERROR_IO_PENDING

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

//this GUID 4D1E55B2 - F16F - 11CF - 88CB - 001111000030 lpa350 working as HID
DEFINE_GUID(guid_lpa350board , 0x4D1E55B2, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30);//ok
//fbb76182-a06d-4f02-9ac4-791289eaf32c RsUSB.inf
//this GUID i took from end of RsUSB.inf file, it is not class GUID 
DEFINE_GUID(guid_RsUSBWinUSb , 0xfbb76182, 0xa06d, 0x4f02, 0x9a, 0xc4, 0x79, 0x12, 0x89, 0xea, 0xf3, 0x2c);//ok
//{A5DCBF10 - 6530 - 11D2 - 901F - 00C04FB951ED} Cypress board and working with CyUsb driver
DEFINE_GUID(guid_CYPRESSCyUSb, 0xA5DCBF10, 0x6530, 0x11D2, 0x90, 0x1f, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);//ok

static bool get_path(const wchar_t* device, const GUID Guid)
{
	char	Product[253];
	std::string	Prod;
	bool result = 0;
	HDEVINFO info;

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

static BOOL QueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
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



int InitUsb(void){
	setlocale(LC_ALL, "en_US.UTF-8");
	const char* p[] = { "Test WinUsb\n" };
	const wchar_t* pj[] = { _T("Привет!\n") };

	printf("%s", *p);
	int gp = get_path(CYPRESS_BOARD, guid_RsUSBWinUSb);
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

void SetupUsbOUT_settings(CRadioServiceAppDlg::device_data dt){

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
the public members of the CRadioServiceAppDlg class.
*/

UINT XferLoop(LPVOID params) {

	CRadioServiceAppDlg *dlg = (CRadioServiceAppDlg *)params;
	while (dlg->bLooping){
		SetupUsbOUT_settings(dlg->dt);
		int loops = ceilf((float)(dlg->dt.end_freq - dlg->dt.start_freq) / 2);

		dlg->ptr_usb_data = (uint8_t*)calloc(1024 * loops, sizeof(uint8_t));

		dlg->usbbytescount = 0;
		printf("start XferLoop, packets will recieve: %d %s\n", loops, dlg->get_cur_time().st);
#ifdef DEBUG_WITH_FILE
		dlg->usbbytescount = read_hex_file("30_80.txt", &dlg->ptr_usb_data);
#else		
		for (int i = 0; i < loops; i++) {
			dlg->usbbytescount += read_usb_async(0x82, &dlg->ptr_usb_data[dlg->usbbytescount], 1024);
			if (dlg->usbbytescount == 0 && dlg->bLooping == 0)
			    { break;}
		}
#endif		
		printf("end XferLoop ,bytes recv %d %s\n", dlg->usbbytescount, dlg->get_cur_time().st);

		printf("save bin data to output.txt start %s\n", dlg->get_cur_time().st);
		dlg->save_hex_buffer_to_file(dlg->ptr_usb_data, dlg->usbbytescount, "output.txt");
		printf("save bin data finish %s\n", dlg->get_cur_time().st);
		dlg->data_is_processing = 1;
		//dlg->m_frequencyData.clear();// очистка буфера

		printf("calculate fft start %s\n", dlg->get_cur_time().st);
		calculate_fft_new(dlg);          // рассчет и запись в буфер
		printf("calculate fft finish %s\n", dlg->get_cur_time().st);
		dlg->data_is_processing = 0;
		printf("updating graph\n", dlg->get_cur_time().st);

		//dlg->Invalidate(0);           // обновление графика
		dlg->update_graph_on_display(FALSE);

		// все данные сохранены в m_frequencyData, буфер удаляет в функции calculate_fft
		//free(dlg->ptr_usb_data);     
		dlg->ptr_usb_data = NULL;
		dlg->usbbytescount = 0;
		Sleep(100);
	}

	dlg->XferThread = NULL;
	dlg->GetDlgItem(IDC_PUSK)->SetWindowTextW(dlg->p_str);
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
ULONG read_usb_sync(UINT8 pipeId, UINT8* buf, int max_cnt){
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
	return recieved;
}

ULONG read_usb_async(UINT8 pipeId, UINT8* buf, int max_cnt) {

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
			//get recieved cnt bytes in buf pointer, we passed it in func (WinUsb_ReadPipe) above 
			BOOL res_over = WinUsb_GetOverlappedResult(InterfaceHandle, &over, &recieved, TRUE);
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
	return recieved;
}