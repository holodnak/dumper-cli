#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hidapi/hidapi/hidapi.h"
#include "device.h"

#define VID 0x0416
#define PID 0xCAFE

enum {
	ID_SETUP_TRANSFER = 1,
	ID_CPU_READ,
	ID_CPU_WRITE,
	ID_PPU_READ,
	ID_PPU_WRITE,

	ID_FIRMWARE_UPDATE = 0x10,
	ID_RESET,
	ID_VERSION,
};

hid_device *handle = NULL;
char DeviceName[256];
int Version;
int VendorID, ProductID;
static uint8_t hidbuf[256];

bool device_open()
{
	struct hid_device_info *devs, *dev;

	//ensure device isnt open
	device_close();

	//get list of available usb devices
	devs = hid_enumerate(VID, PID);

	//search for device in all the usb devices found
	for (dev = devs; dev != NULL; dev = dev->next) {
		if (dev->vendor_id == VID && dev->product_id == PID)
			break;
	}

	//device found, try to open it
	if (dev) {
		handle = hid_open_path(dev->path);
	}

	//device opened successfully, try to communicate with the flash chip
	if (handle) {

		//save device informations
		wcstombs(DeviceName, dev->product_string, 256);
		VendorID = dev->vendor_id;
		ProductID = dev->product_id;
		Version = dev->release_number;
	}
	else {
		printf("Device not found.\n");
	}
	hid_free_enumeration(devs);
	return !!handle;
}

void device_close()
{
	hid_close(handle);
	handle = NULL;
}

bool device_read(uint8_t reportid, uint8_t *data, int len)
{
	int ret;

	if (len > 63) {
		printf("device_read:  read too big.\n");
		return(false);
	}
	hidbuf[0] = reportid;
	ret = hid_get_feature_report(handle, hidbuf, 64);
	if (ret < 0)
		return(false);
	memcpy(data, hidbuf + 1, len);
	//	printf("hid_get_feature_report: returned %d\n", ret);
	return(true);
}

bool device_write(uint8_t reportid, uint8_t *data, int len)
{
	int ret;

	if (len > 63) {
		printf("device_write:  write too big.\n");
		return(false);
	}
	hidbuf[0] = reportid;
	if (len)
		memcpy(hidbuf + 1, data, len);
	ret = hid_send_feature_report(handle, hidbuf, len + 1);
	if (ret == -1) {
		wprintf(L"device_write:  error: %s\n", hid_error(handle));
	}
	return(ret >= 0);
}

bool device_setup_transfer(uint32_t addr, uint32_t size)
{
	uint8_t buf[4];

	buf[0] = (uint8_t)(addr >> 0);
	buf[1] = (uint8_t)(addr >> 8);
	buf[2] = (uint8_t)(size >> 0);
	buf[3] = (uint8_t)(size >> 8);
	return(device_write(ID_SETUP_TRANSFER, buf, 4));
}

bool device_read_cpu(uint32_t addr, int len, uint8_t *buf)
{
	device_setup_transfer(addr, len);
	while (len > 0) {
		if (device_read(ID_CPU_READ, buf, (len > 63) ? 63 : len) == false) {
			return(false);
		}
		buf += 63;
		len -= 63;
	}
	return(true);
}

bool device_read_ppu(uint32_t addr, int len, uint8_t *buf)
{
	device_setup_transfer(addr, len);
	while (len > 0) {
		if (device_read(ID_PPU_READ, buf, (len > 63) ? 63 : len) == false) {
			return(false);
		}
		buf += 63;
		len -= 63;
	}
	return(true);
}

bool device_write_cpu(uint32_t addr, uint8_t data)
{
	uint8_t buf[4];

	buf[0] = addr;
	buf[1] = addr >> 8;
	buf[2] = data;
	return(device_write(ID_CPU_WRITE, buf, 3));
}

bool device_write_ppu(uint32_t addr, uint8_t data)
{
	uint8_t buf[4];

	buf[0] = addr;
	buf[1] = addr >> 8;
	buf[2] = data;
	return(device_write(ID_PPU_WRITE, buf, 3));
}
