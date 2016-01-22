#pragma once

bool device_open();
void device_close();
bool device_read(uint8_t reportid, uint8_t *data, int len);
bool device_write(uint8_t reportid, uint8_t *data, int len);
bool device_setup_transfer(uint32_t addr, uint32_t size);
bool device_read_cpu(uint32_t addr, int len, uint8_t *buf);
bool device_read_ppu(uint32_t addr, int len, uint8_t *buf);
bool device_write_cpu(uint32_t addr, uint8_t data);
bool device_write_ppu(uint32_t addr, uint8_t data);
