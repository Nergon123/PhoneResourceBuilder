# =====================
# Platform selection
# =====================
PLATFORM ?= linux

# =====================
# Compilers
# =====================
ifeq ($(PLATFORM),windows)
    WINDRES := x86_64-w64-mingw32-windres
    SYSROOT    := /usr/x86_64-w64-mingw32
    CXX        := x86_64-w64-mingw32-g++
    CC         := x86_64-w64-mingw32-gcc
    WX_CONFIG  := /opt/wxwidgets-win/bin/wx-config
    WX_INCLUDE := /opt/wxwidgets-win/include/wx-3.3
    TARGET     := image_editor.exe
    RUN_CMD    := wine $(TARGET)
    STATIC     := -static -static-libgcc -static-libstdc++ -mwindows
    CXXFLAGS   := -Wall -g -I$(SYSROOT)/include
    CFLAGS     := -Wall -g -I$(SYSROOT)/include
    LIBS       :=   # no zlib needed, using standalone crc32
else
    CXX        := g++
    CC         := gcc
    WX_CONFIG  := wx-config
    TARGET     := image_editor
    RUN_CMD    := ./$(TARGET)
    STATIC     :=
    CXXFLAGS   := -Wall -g
    CFLAGS     := -Wall -g
    LIBS       :=
endif

# =====================
# Flags from wx-config
# =====================
WX_CXXFLAGS := $(shell $(WX_CONFIG) --cxxflags)
WX_LIBS     := $(shell $(WX_CONFIG) --libs)

# =====================
# Sources
# =====================
SRCDIR := src
OBJDIR := obj/$(PLATFORM)

# C++ sources (relative to SRCDIR)
SRC := \
    main.cpp \
    ImageCanvas.cpp \
    FileProcessor.cpp \
    MainFrame.cpp \
    MyProjectBase.cpp \
    SpriteFormBase.cpp \
    SpriteForm.cpp

# C sources (relative to SRCDIR)
SRC_C := \
    External/crc32.c

# Object files
OBJ   := $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
OBJ_C := $(addprefix $(OBJDIR)/, $(SRC_C:.c=.o))
ALL_OBJ := $(OBJ) $(OBJ_C)

# Source files with full paths
SRC_FULL   := $(addprefix $(SRCDIR)/, $(SRC))
SRC_C_FULL := $(addprefix $(SRCDIR)/, $(SRC_C))

# =====================
# Targets
# =====================
.PHONY: all linux windows run debug clean

all: $(TARGET)

linux:
	$(MAKE) PLATFORM=linux

windows:
	$(MAKE) PLATFORM=windows

run: $(TARGET)
	$(RUN_CMD)

debug: $(TARGET)
ifeq ($(PLATFORM),windows)
	wine gdb $(TARGET)
else
	gdb --args ./$(TARGET)
endif

# =====================
# Build rules
# =====================
# Compile the resource
ifeq (PLATFORM, windows)
APP_RC := $(SRCDIR)/app.rc
APP_RC_OBJ := $(OBJDIR)/app_rc.o


$(APP_RC_OBJ): $(APP_RC) | $(OBJDIR)
	$(WINDRES) -I$(WX_INCLUDE) $< -O coff  -o $@ 
endif

# Create object directories
$(OBJDIR):
	mkdir -p $@

# Compile C++ sources
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling C++: $<"
	$(CXX) $(CXXFLAGS) $(WX_CXXFLAGS) -c $< -o $@

# Compile C sources (for files in subdirectories)
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	@echo "Compiling C: $<"
	$(CC) $(CFLAGS) -c $< -o $@

ifeq (PLATFORM, windows)
# Link final executable
$(TARGET): $(ALL_OBJ) $(APP_RC_OBJ)
	@echo "Linking $@"
	$(CXX) $(ALL_OBJ) $(APP_RC_OBJ) -o $@ $(WX_LIBS) $(LIBS) $(STATIC)
else
$(TARGET): $(ALL_OBJ)
	@echo "Linking $@"
	$(CXX) $(ALL_OBJ) -o $@ $(WX_LIBS) $(LIBS) $(STATIC)
endif
# =====================
# Cleanup
# =====================
clean:
	rm -rf obj $(TARGET) image_editor image_editor.exe

# Debug target to print variables
print-%:
	@echo '$*=$($*)'
