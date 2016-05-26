################################################################################
# Makefile for librcb4
#
# Alfonso Arbona Gimeno - Nagoya Institute of Technology
# 2015-10, 2015-11
################################################################################

# USAGE:
# make -> Compile the library.
# make samples -> Compile the samples (and the library).
# make docs -> Create the documentation.
# make clean -> Delete all the compiled files (library and samples).
# make doc_clean -> Delete the documentation.
# make vim_setup -> Recreates the ctags for vim.


# Compiler
CC = gcc
LD = ld
AR = ar

# Binaries
LIB_STATIC := librcb4.a

# Directory of the source code, include files, object files and where to place
# our library once compiled
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
LIB_DIR := lib
# Samples
SAMPLE_DIR := samples

# Linker and compiler flags
LDFLAGS := -lm
CFLAGS := -DLIBRARY_BUILD -Wall -Wextra -g -Iinc
SAMPLES_CFLAGS := -Wall -g -Iinc
ARFLAGS := rcs

# Name of the common source files
H_FILES :=  $(wildcard $(INC_DIR)/*.h)
C_FILES :=  $(wildcard $(SRC_DIR)/*.c)
CPP_FILES :=  $(wildcard $(SRC_DIR)/*.cpp)
SAMPLE_C_FILES :=  $(wildcard $(SAMPLE_DIR)/*.c)
SAMPLE_BINS := $(patsubst %.c,%,$(SAMPLE_C_FILES))

# Compiled object files
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.c.o,$(C_FILES))
#$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_FILES))

SAMPLE_OBJ_FILES := $(patsubst $(SAMPLE_DIR)/%.c,$(SAMPLE_DIR)/%.c.o,$(SAMPLE_C_FILES))
LIB_STATIC_FULL := $(LIB_DIR)/$(LIB_STATIC)

LIB_LD_NAME := $(patsubst lib%.a,%,$(LIB_STATIC))


# RULES

all: $(LIB_STATIC_FULL)
samples: $(LIB_STATIC_FULL) $(SAMPLE_BINS)

$(LIB_STATIC_FULL): $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(H_FILES) $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c $(H_FILES) $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(SAMPLE_BINS): % : %.c
	$(CC) -static $(SAMPLES_CFLAGS) $< $(LDFLAGS) -L./$(LIB_DIR) -l$(LIB_LD_NAME) -o $@

#$(SAMPLE_DIR)/%.c.o: $(SAMPLE_C_FILES)
#	$(CC) $(SAMPLES_CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

docs:
	doxygen doxygen.cfg

vim_setup:
	ctags -R --fields=+l *

clean:
	rm -rf $(OBJ_FILES) $(OBJ_DIR) $(LIB_STATIC_FULL) $(SAMPLE_BINS) $(SAMPLE_OBJ_FILES)
	
doc_clean:
	rm -rf doc/*

.PHONY: clean
