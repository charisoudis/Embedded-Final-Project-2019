CC_DIR    = /home/argiris/Desktop/Argiris/EmbeddedSystems/openwrt-zsun-zsun/staging_dir/toolchain-mips_mips32_gcc-4.8-linaro_uClibc-0.9.33.2/bin
CC        = $(CC_DIR)/mips-openwrt-linux-gcc -std=c99 -g -Iinclude
XFLAGS    = -lpthread

# Final target executable
LINK_TARGET = ./out/final.out

# Partial Object codes
OBJS =  \
	client.o \
	server.o \
	utils.o \
	main.o

# Clean-up rule
clean :
	rm -f $(OBJS) $(LINK_TARGET)

# Search for .c files in "src" directory; .h files in "include" directory
# The pattern matching character '%' matches filename without the extension
vpath %.c src
vpath %.h include

# Entry point of compilation
$(LINK_TARGET) : $(OBJS)
	mkdir -p out
	$(CC) -o $@ $^ $(XFLAGS)

# Here is a Pattern Rule, often used for compile-line.
# It says how to create a file with a .o suffix, given a file with a .c suffix.
# The rule's command uses some built-in Make Macros:
# $@ for the pattern-matched target
# $< for the pattern-matched dependency
%.o : %.c
	$(CC) -o $@ -c $<

# Dependency Rules are often used to capture header file dependencies.
client.o : client.h
server.o : server.h
utils.o : utils.h
main.o : client.h server.h utils.h

# Make rule
all : $(LINK_TARGET)
	rm $(OBJS) && sshpass -p 01041960 scp $(LINK_TARGET) root@192.168.1.1:/root