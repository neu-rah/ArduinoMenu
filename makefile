#  Makefile template for Static library.
# addpted from https://gist.github.com/basharam/9511906


CC = g++
OUT_FILE_NAME = am4.a

CFLAGS= -fPIC -O0 -g -Wall -c -fpermissive -Wno-endif-labels
# CFLAGS= -fpermissive -Wno-endif-labels

INC = -I/usr/include/c++/7

OBJ_DIR=./obj
OUT_DIR=./out

# Enumerating of every *.cpp as *.o and using that as dependency.
# filter list of .c files in a directory.
# FILES =dump_l.c \
#	kter.c \
#
# $(OUT_FILE_NAME): $(patsubst %.c,$(OBJ_DIR)/%.o,$(wildcard $(FILES)))


# Enumerating of every *.cpp as *.o and using that as dependency
# crap! it leaces output files there!
# subdirs := src/ src/naf/
# sources := $(wildcard $(addsuffix *.cpp,$(subdirs)))
# objects := $(patsubst %.cpp,%.o,$(sources))
# $(OUT_FILE_NAME): $(objects)
# 	ar -v -r -o $(OUT_DIR)/$@ $^

# outs := $(wildcard $(addsuffix *.o,$(OBJ_DIR)/)) $(wildcard $(addsuffix *.cpp,./src/))
# $(OUT_FILE_NAME): $(outs)
# 	ar -r -o $(OUT_DIR)/$@ $^

$(OUT_FILE_NAME):	 $(wildcard src/*.* src/naf/*.*)
	ar -v -r -o $(OUT_DIR)/$@ $^


#Compiling every *.cpp to *.o
# $(OBJ_DIR)/%.o: %.cpp dirmake
# 	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/items.o: src/items.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/itemsTemplates.o: src/itemsTemplates.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/menuBase.o: src/menuBase.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/menu.o: src/menu.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/menuIo.o: src/menuIo.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/nav.o: src/nav.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/Print.o: src/naf/Print.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/Stream.o: src/naf/Stream.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<
$(OBJ_DIR)/WString.o: src/naf/WString.cpp $(wildcard src/*.h src/naf/*.*)
	$(CC) -v -c $(INC) $(CFLAGS) -o $@  $<

dirmake:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(OUT_FILE_NAME) Makefile.bak

rebuild: clean build
