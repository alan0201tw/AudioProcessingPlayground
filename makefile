# Compile Info
CC := clang++
SRCEXT := cpp
CFLAGS := -O3 -std=gnu++17 -pthread -g -Wall
# use -isystem on vender headers to suppress warning
INCDIR := -I include -I vendor/include
LINKS := -ldl -lrt -lm -lasound -lportaudio -lsndfile

# Source Info, target = cpplox, entry should be in cpplox.cpp
SRCDIR := src
BINDIR := bin
TARGET := main
SOURCES := $(shell find $(SRCDIR) -type f -iname "*.$(SRCEXT)" ! -iname "$(TARGET).$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BINDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Compile main
$(TARGET): $(OBJECTS) $(BINDIR)/$(TARGET).o
	# setup environment variable
	@echo "export LD_LIBRARY_PATH=/usr/local/lib"; export LD_LIBRARY_PATH=/usr/local/lib
	@echo "Linking..."
	@echo "$(CC) $^ $(CFLAGS) -o $(TARGET)" $(LINKS); $(CC) $^ $(CFLAGS) -o $(TARGET) $(LINKS)

$(BINDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BINDIR)
	@echo "$(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<"; $(CC) $(CFLAGS) $(INCDIR) -c -o $@ $<

# Clean all binary files
clean:
	@echo " Cleaning..."; 
	@echo "$(RM) -r $(BINDIR) $(TARGET)"; $(RM) -r $(BINDIR) $(TARGET)
	@echo "$(RM) -r $(TESTBINDIR)"; $(RM) -r $(TESTBINDIR)

# Declare clean as utility, not a file
.PHONY: clean