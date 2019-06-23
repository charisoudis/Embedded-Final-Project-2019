CC        = gcc -g -Iinclude
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
	# sudo aptitude install libsodium-dev || mkdir -p out	# Un-comment if libsodium is not installed in your system
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
	rm $(OBJS)