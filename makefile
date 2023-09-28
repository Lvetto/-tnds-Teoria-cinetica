COMP_FLAGS := -lSDL2 -lSDL2_ttf
DBG_FLAGS := -O0 -g -Wall --pedantic
FILENAMES := particle graphics/button graphics/graphics-functions functions

HEADER_FILES := $(addsuffix .h, $(FILENAMES))
OBJECT_FILES := $(addsuffix .o, $(FILENAMES))
DBG_OBJECT_FILES := $(addsuffix .o, $(addsuffix .debug,$(FILENAMES)))

# compile generic target to object file (keeps file location)
%.o:	%.cpp
	g++ -c $< -o $@

# compile the program
all:	main.cpp $(OBJECT_FILES) $(HEADER_FILES)
	g++ -o progr main.cpp $(OBJECT_FILES) $(COMP_FLAGS)

# run the program
run:	all
	./progr

# compile generic target to object file with debug flags (keeps file location)
%.debug.o:	%.cpp
	g++ -c $< -o $@ $(DBG_FLAGS)

# compile the program with debug flags
debug:	main.cpp $(DBG_OBJECT_FILES) $(HEADER_FILES)
	g++ -o deb_progr main.cpp $(DBG_OBJECT_FILES) $(COMP_FLAGS) $(DBG_FLAGS)

# run the program with gdb
drun:	debug
	gdbtui ./deb_progr

# delete all object files and compiled programs
clean:
	rm -f *.o
	rm -f graphics/*.o
	rm -f progr
	rm -f deb_progr
