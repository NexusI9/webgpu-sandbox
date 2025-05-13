INPUT ?= ../../assets/obj
OUTPUT ?= ../../assets/mbin

TARGET := obj2min
FLAGS := -O

.PHONY: all run clean

all:
	@make compile
	@make run
	@make clean

compile:
	@echo "Compiling"
	@gcc $(FLAGS) -o $(TARGET) main.c

run:
	@echo "Converting"
	@mkdir -p $(OUTPUT)
	@for file in $(INPUT)/*.obj; do \
		./$(TARGET) $$file $(OUTPUT); \
	done


clean:
	@echo "Cleaning"
	@rm -f $(TARGET)
	@echo "done"

