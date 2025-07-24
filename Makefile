# Makefile for wxWidgets image editor

# Compiler and flags
CXX := g++
WX_FLAGS := `wx-config --cxxflags --libs`
CXXFLAGS := -Wall -g

# Source and target
SRC := main.cpp \
	   ImageCanvas.cpp \
	   FileProcessor.cpp \
       MainFrame.cpp \
       MyProjectBase.cpp \
       SpriteFormBase.cpp \
       SpriteForm.cpp
SRCDIR := src
OBJDIR := obj
OBJ := $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
TARGET := image_editor

# Default rule
all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)
	gdb --args ./$(TARGET)

# Ensure obj directory exists before compiling
$(OBJ): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# Link
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(WX_FLAGS)

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(WX_FLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)