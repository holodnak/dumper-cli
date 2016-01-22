-- dump script for mmc3/tlrom
-- by James Holodnak

function dump_setup()
	set_prg_bank_size(0x2000)
	set_chr_bank_size(0x1000)
	set_mapper(4)
end

function dump_cpu()
	banks = 32 - 3
	for i=0,banks do
		cpu_write(0x8000, 6)
		cpu_write(0x8001, i)
		cpu_read(0x8000,0x2000)
	end
	cpu_read(0xC000,0x4000)
end

function dump_ppu()
	banks = 128 - 2
	for i=0,banks,2 do
		cpu_write(0x8000, 0);
		cpu_write(0x8001, i);
		cpu_write(0x8000, 1);
		cpu_write(0x8001, i + 2);
		ppu_read(0x0000, 0x800);
	end
end
