# Путь к компилятору
COMPILER=g++

# Флаги компиляции
FLAGS=-Wno-unused-parameter -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_

# Папка с объектами
BIN_DIR=binary

# Папка с исходниками и заголовками
SRC_DIR=source


# Зависимости ассемблера
ASM_DPD = command cmd assert parser hash asm_cmd_list asm_func_list text


# Зависимости процессора
CPU_DPD = command cmd assert parser stack dsl cpu_cmd_list cpu_func_list


all: processor assembler


# Завершает сборку ассемблера
assembler: $(BIN_DIR)/assembler.o
	$(COMPILER) $^ -o asm.exe -Llib -ltext -lparser


# Завершает сборку процессора
processor: $(BIN_DIR)/processor.o
	$(COMPILER) $^ -o cpu.exe -Llib -lstack -lparser


# Предварительная сборка ассемблера
$(BIN_DIR)/assembler.o: $(SRC_DIR)/assembler.cpp $(addprefix $(SRC_DIR)/, $(addsuffix .hpp, $(ASM_DPD)))
	$(COMPILER) $(FLAGS) -c $< -o $@


# Предварительная сборка процессора
$(BIN_DIR)/processor.o: $(SRC_DIR)/processor.cpp $(addprefix $(SRC_DIR)/, $(addsuffix .hpp, $(CPU_DPD)))
	$(COMPILER) $(FLAGS) -c $< -o $@
