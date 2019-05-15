target := out

cxx := g++ 
flags := -std=c++1z

builddir = build/

sources := main.cpp lib/osio.cpp lib/pa.cpp expr/lexer.cpp expr/symtable.cpp expr/parser.cpp

includes := -I ../libraries/portaudio/include


# Choose one: OS_WIN, OS_MAC, OS_GNU
# OS_HOST = OS_WIN
OS_HOST = OS_MAC
# OS_HOST = OS_GNU

ifeq '$(OS_HOST)' 'OS_WIN'
	flags += -D OS_HOST=OS_WIN
	libs = -L ../libraries/portaudio/build -lportaudio

else ifeq '$(OS_HOST)' 'OS_MAC'
	flags += -D OS_HOST=OS_MAC -x objective-c++
	libs = -L /usr/local/lib -lportaudio
	libs += -lobjc -framework Foundation -framework Cocoa

else ifeq '($(OS_HOST)' 'OS_GNU'
	flags += -D OS_HOST=OS_GNU	
	libs = -L /usr/local/lib -lportaudio

endif

headers := $(wildcard *.h) $(wildcard lib/*.h) $(wildcard expr/*.h)
objects := $(addprefix $(builddir), $(addsuffix .o, $(basename $(notdir $(sources)))))


$(builddir)%.o: %.cpp $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

$(builddir)%.o: lib/%.cpp $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

$(builddir)%.o: expr/%.cpp $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@


all: make_dir $(target)

$(target): $(objects) Makefile
	$(cxx) $(objects) -o $(target) $(libs)

make_dir : $(builddir)
$(builddir):
	mkdir -p $(builddir)

clean:
	rm -f $(objects) $(target)

run:
	@./$(target)