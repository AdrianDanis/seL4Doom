# Targets
TARGETS := seL4Doom.bin

# Source files required to build the target
CFILES   := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.c))

ASMFILES := $(patsubst $(SOURCE_DIR)/%,%,$(wildcard $(SOURCE_DIR)/src/*.S))

# We want to run C99 and pretend to be unix for doom
CFLAGS += -std=gnu99
NK_CFLAGS += --std=gnu99 -DNORMALUNIX

# Libraries required to build the target
LIBS-y							= muslc sel4 sel4muslcsys \
								  sel4platsupport sel4simple sel4simple-stable sel4vka sel4allocman sel4vspace sel4vka sel4utils utils
LIBS = $(LIBS-y)

include $(SEL4_COMMON)/common.mk
