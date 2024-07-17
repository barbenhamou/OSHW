# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -W -g -std=c++20

# Source files
SRCS = BoundedBuffer.cpp \
       CoEditor.cpp \
       Dispatcher.cpp \
       Main.cpp \
       Producer.cpp \
       ScreenManager.cpp

# Header files
 HDRS = BoundedBuffer.h \
        CoEditor.h \
        Dispatcher.h \
        Producer.h \
        ScreenManager.h

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = program

# Default target
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)

# Phony targets
.PHONY: all clean
