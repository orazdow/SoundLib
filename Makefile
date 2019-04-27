TARGET := out.exe

SRCDIR_2 = lib

# SRC := $(wildcard *.cpp)
SRC := main.cpp
SRC += $(wildcard $(SRCDIR_2)/*.cpp)

# comment out to use basedir
builddir = build/

INCLUDE = -ID:/libraries/portaudio/include
LIBS = -LD://libraries/portaudio/build 
LIBS += -lportaudio

CXX := g++
# FLAGS := -Wall

OBJECTS :=  $(addprefix $(builddir), $(addsuffix .o, $(basename $(notdir $(SRC)))))
DEPS = $(wildcard *.h)
DEPS += $(wildcard $(SRCDIR_2)/*.h)


$(builddir)%.o: %.cpp $(DEPS)
	$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo $@

$(builddir)%.o: $(SRCDIR_2)/%.cpp $(DEPS)
	$(CXX) $(INCLUDE) $(FLAGS) -c $< -o $@ $(LIBS)
	@echo $@

all: make_dir $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

make_dir : $(builddir)

$(builddir):
	mkdir -p $(builddir)

clean:
	rm -f $(OBJECTS) $(TARGET)

run:
	@./$(TARGET)