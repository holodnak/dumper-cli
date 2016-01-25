-- dump script for vrc4c
-- by James Holodnak

function dump_setup()
	set_prg_bank_size(0x2000)
	set_chr_bank_size(0x0400)
	set_mapper(21)
	set_submapper(14)
end

function dump_cpu()
	banks = 32 - 3

	cpu_write(0x9080, 0)
	cpu_write(0x8000, 0)
	cpu_write(0xA000, 0)

	for i=0,banks do
		cpu_write(0xA000, i)
		cpu_read(0xA000, 0x2000)
	end
	cpu_read(0xC000, 0x4000)
end

function dump_ppu()
	banks = 256 - 4
	for i=0,banks,4 do
		cpu_write(0xd040, i >> 4)
		cpu_write(0xd000, i)
		cpu_write(0xd0c0, i >> 4)
		cpu_write(0xd080, i | 1)
		cpu_write(0xe040, i >> 4)
		cpu_write(0xe000, i | 2)
		cpu_write(0xe0c0, i >> 4)
		cpu_write(0xe080, i | 3)
		ppu_read(0x1000, 0x1000)
	end
end
