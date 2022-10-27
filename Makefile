
INCLUDES=include
CPU=cpu
ASM=asm
DISASM=disasm

asm: asm_main.o assembler.o
	gcc -o asm_bin $^ -llogger -lonegin -lstack

asm_main.o: ${ASM}/asm_main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR

assembler.o: ${ASM}/assembler.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR


cpu: cpu_main.o processor.o
	gcc -o cpu_bin $^ -llogger -lonegin -lstack -lm

cpu_main.o: $(CPU)/cpu_main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR

processor.o: $(CPU)/processor.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR


disasm: disasm_main.o disassembler.o
	gcc -o disasm_bin $^ -llogger -lonegin -lstack -lm

disasm_main.o: $(DISASM)/disasm_main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR

disassembler.o: $(DISASM)/disassembler.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DCHAR

# asm-sanitize: asm_main.o assembler.o
# 	gcc -o asm_bin $< -fsanitize=address -g

# cpu-sanitize: cpu_main.o processor.o
# 	gcc -o cpu_bin $< -fsanitize=address -g

# disasm-sanitize: disasm_main.o disassembler.o
# 	gcc -o disasm_bin $< -fsanitize=address -g

asm-valgrind: asm
	valgrind --leak-check=yes ./asm $(ARGS)

cpu-valgrind: cpu
	valgrind --leak-check=yes ./cpu $(ARGS)

disasm-valgrind: disasm
	valgrind --leak-check=yes ./disasm $(ARGS)

.PHONY: clean
clean:
	rm asm cpu disasm asm_main.o assembler.o
