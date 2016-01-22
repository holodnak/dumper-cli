-- dump script for mmc1/slrom
-- by James Holodnak

function mmc1_write(addr,data)
	cpu_write(addr,(data >> 0) & 1)
	cpu_write(addr,(data >> 1) & 1)
	cpu_write(addr,(data >> 2) & 1)
	cpu_write(addr,(data >> 3) & 1)
	cpu_write(addr,(data >> 4) & 1)
end

function dump_setup()
	set_prg_bank_size(0x2000)
	set_chr_bank_size(0x2000)
	set_mapper(1)

	cpu_write(0x8000, 0x80)
	mmc1_write(0x8000, 0x1c)
	mmc1_write(0xA000, 0x10) --SNROM + MMC1A disable W-RAM
	mmc1_write(0xE000, 0x10) --MMC1B disable W-RAM
	mmc1_write(0x8000, 0x0C) --enable bank at $8000 switching, $C000 is fixed

end

function dump_cpu()
	banks = 8 - 2
	for i=0,banks do
		mmc1_write(0xE000, i + 0x10)
		cpu_read(0x8000, 0x4000)
	end
	cpu_read(0xC000, 0x4000)
end

function dump_ppu()
	banks = 32 - 2
	for i=0,banks,2 do
		mmc1_write(0xA000, i)
		mmc1_write(0xC000, i + 1)
		ppu_read(0x0000, 0x2000)
	end
end
