TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
DBH = "\x44\x41\x4c\x4c\x01\x00\x00\x00\x0c\x00"
TDB = ./mynewdb.db
TESTS_WITH_DB = $(wildcard testdb/*/*)
EMP = "Timmy H.,123 Sheshire Ln.,120"

run: clean default 

tests: $(TESTS_WITH_DB) bootstraptests

bootstraptests: bootstraptest1 bootstraptest2

bootstraptest1: FORCE
	@echo $@
	@rm -f $(TDB)
	@./$(TARGET) -f $(TDB) -n 
	@./$(TARGET) -f $(TDB) 

bootstraptest2: FORCE
	@echo $@
	@rm -f $(TDB)
	@./$(TARGET) -f $(TDB) -n
	@./$(TARGET) -f $(TDB) -a $(EMP)
	@./$(TARGET) -f $(TDB) -l -a $(EMP)

testdb/inandout/%: FORCE
	@echo $@
	@cp -f $@ $(TDB)
	@./$(TARGET) -f $(TDB)
	@diff $@ $(TDB)

testdb/addingemployees/%: FORCE
	@echo $@
	@cp -f $@ $(TDB)
	@./$(TARGET) -f $(TDB) 
	@./$(TARGET) -f $(TDB)  -a $(EMP)


default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $? -g

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude -g

FORCE: 



