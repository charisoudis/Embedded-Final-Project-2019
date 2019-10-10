CC := gcc
TARGET_EXEC ?= final

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INCLUDE_DIR ?= include

SRCS := $(shell find $(SRC_DIRS) -name '*.c')
SRCS := $(SRCS) main.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(INCLUDE_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDFLAGS ?= -lpthread

# Link all
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	$(RM) -r $(OBJS) $(DEPS)
	rmdir $(BUILD_DIR)/$(SRC_DIRS)

# Compile a single C source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

.PHONY: print-%
print-%  : ; @echo $* = $($*)