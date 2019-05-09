target := out

sources := main.mm sl/lib/osio.mm sl/lib/pa.cpp sl/expr/lexer.cpp sl/expr/symtable.cpp sl/expr/parser.cpp

cxx := g++ -std=c++1z

frameworks := -framework Foundation -framework Cocoa 
libs := -lobjc $(frameworks)

includes := -I ../libraries/portaudio/include
libs += -L /usr/local/lib
libs += -lportaudio

headers := $(wildcard sl/*.h) $(wildcard sl/lib/*.h) $(wildcard sl/expr/*.h)
objects := $(addsuffix .o, $(basename $(notdir $(sources))))

%.o: %.mm $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

%.o: sl/lib/%.mm $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

%.o: sl/lib/%.cpp $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

%.o: sl/expr/%.cpp $(headers)
	$(cxx) $(flags) $(includes) -c $< -o $@ 
	@echo $@

all: $(target)

$(target): $(objects) 
	$(cxx) $(objects) -o $(target) $(libs)

clean:
	rm -f $(objects) $(target)

run:
	@./$(target)