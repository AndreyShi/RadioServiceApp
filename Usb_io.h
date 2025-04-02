/*
WinUSB
*/
int InitUsb(void);
void SendInitUsbPacket(void);
void RecvInitUsbPacket(void);
void SendEmptyUsbPacket(void);
void SendSetupUsbPacket(UINT16 start_freq, UINT16 end_freq, UINT8 attenua);
UINT XferLoop(LPVOID params);
void abort_pipe(void);

void write_usb(void);
void read_sync(void);
void read_async(void);