# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -W -g

# Source files
SRCS = BoundedBuffer.cpp \
       UnBoundedBuffer.cpp \
       CoEditor.cpp \
       Dispatcher.cpp \
       Producer.cpp \
       ScreenManager.cpp \
       Main.cpp 

# Header files
 HDRS = BoundedBuffer.h \
        UnBoundedBuffer.h \
        CoEditor.h \
        Dispatcher.h \
        Producer.h \
        ScreenManager.h

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = ex3.out

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
