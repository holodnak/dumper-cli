-- dump script for vrc7
-- by James Holodnak

function dump_setup()
	set_prg_bank_size(0x2000)
	set_chr_bank_size(0x0000)
	set_mapper(85)
end

function dump_cpu()
	cpu_write(0x8000, 1)
	cpu_write(0x8008, 2)
	cpu_write(0x8010, 2)
	cpu_write(0x9000, 0)

	banks = (512 / 8) - 2
	for i=0,banks do
		cpu_write(0x9000, i)
		cpu_read(0xC000, 0x2000)
	end
	cpu_read(0xE000, 0x2000)
end

function dump_ppu()

end
