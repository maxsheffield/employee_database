TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
DBH = "\x44\x41\x4c\x4c\x01\x00\x00\x00\x0c\x00"
TDB = ./mynewdb.db
TESTS_WITH_DB = $(wildcard testdb/*/*)
EMP = "Timmy H.,123 Sheshire Ln.,120"
EMP2 = "Benji Mahmood,555 Tuson Az.,520"
EMPN = "Timmy H."
EMP2N = "Benji Mahmood"

# work out how make parses shell commands

run: clean default tests

tests: $(TESTS_WITH_DB) bootstraptests

bootstraptests: bootstraptest1 bootstraptest2

bootstraptest1: FORCE
	@echo $@
	@rm -f $(TDB)
	@./$(TARGET) -f $(TDB) -n
	@echo "listing empty database" 
	@./$(TARGET) -f $(TDB) -l

bootstraptest2: FORCE
	@echo $@	
	@rm -f $(TDB)
	@./$(TARGET) -f $(TDB) -n
	@echo "adding 2 employees"
	@./$(TARGET) -f $(TDB) -l -a $(EMP2)
	@./$(TARGET) -f $(TDB) -l -a $(EMP)
	@echo "removing 2 employees"
	@./$(TARGET) -f $(TDB) -l -r $(EMP2N)
	@./$(TARGET) -f $(TDB) -l -r $(EMPN)
	@echo "removing non existant employee"
	@./$(TARGET) -f $(TDB) -l -r $(EMPN)


testdb/inandout/%: FORCE
	@echo $@
	@cp -f $@ $(TDB)
	@./$(TARGET) -f $(TDB) 
	@diff $@ $(TDB)

testdb/addingemployees/%: FORCE
	@echo $@
	@cp -f $@ $(TDB)
	@./$(TARGET) -f $(TDB) -l
	@./$(TARGET) -f $(TDB)  -a $(EMP) -l
	@./$(TARGET) -f $(TDB)  -r $(EMPN) -l


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



