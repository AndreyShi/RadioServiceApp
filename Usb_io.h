/*
WinUSB
*/
struct device_data{
	UINT16 start_freq;
	UINT16 end_freq;
	UINT8 attenua;
};


int InitUsb(void);
void SetupUsbOUT_init(void);
void SetupUsbIN(UINT8 ReqT, UINT8 Req, UINT16 Value, UINT16 Index, UINT16 Length);
void SetupUsbOUT_empty(void);
void SetupUsbOUT_settings(device_data dt);
UINT XferLoop(LPVOID params);
void abort_pipe(UINT8 pipeId);
void reset_pipe(UINT8 pipeId);

void write_usb(UINT8 pipeId, UINT8* buf, int max_cnt);
void read_usb_sync(UINT8 pipeId, UINT8* buf, int max_cnt);
void read_usb_async(UINT8 pipeId, UINT8* buf, int max_cnt);
