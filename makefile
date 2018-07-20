TARGET := out.exe
SRC := main.cpp

# INCLUDE = -ID:/Libraries/portaudio/include
# LIBS = -LD:/Libraries/portaudio/build 
# LIBS += -lportaudio

CXX := g++
FLAGS := -Wall

OBJECTS := $(addsuffix .o, $(basename $(notdir $(SRC))))
DEPS = $(wildcard *.h)


%.o: %.cpp $(DEPS)
	$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo $@

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

run:
	@./$(TARGET)