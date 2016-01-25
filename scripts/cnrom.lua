-- dump script for cnrom
-- by James Holodnak

function dump_setup()
	set_prg_bank_size(0x4000)
	set_chr_bank_size(0x2000)
	set_mapper(3)
end

function dump_cpu()
	cpu_read(0x8000,0x4000)
	cpu_read(0xC000,0x4000)
end

function dump_ppu()
	security = 0x30
	banks = 4 - 1
	for i=0,banks do
		cpu_write(0x8000,security | i)
		ppu_read(0x0000,0x2000)
	end
end
