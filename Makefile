CXX		  := g++
CXX_FLAGS := -std=c++17 -Wall -fpic

BIN		:= build
DOC		:= docs
INCLUDE	:= include
LIB		:= lib
LIBNAME = cdcc

LIBRARIES	:=
EXECUTABLE	:= console_example


all: dirs $(BIN)/$(EXECUTABLE) docs

dirs:
	-mkdir $(BIN)
	-mkdir $(DOC)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): examples/$(EXECUTABLE).cpp lib$(LIBNAME).so lib$(LIBNAME).a
	$(CXX) $(CXX_FLAGS) examples/$(EXECUTABLE).cpp -o $(BIN)/$(EXECUTABLE) -lusb-1.0 -L$(BIN) -l$(LIBNAME)
	
(libname).o:
	$(CXX) $(CXX_FLAGS) -c -Wall -Werror -fpic -o $(BIN)/$(LIBNAME).o cdcc.cpp

lib$(LIBNAME).so:(libname).o
	$(CXX) -shared -I. -lusb-1.0  -o $(BIN)/lib$(LIBNAME).so $(BIN)/$(LIBNAME).o
	cp $(BIN)/lib$(LIBNAME).so .

lib$(LIBNAME).a:(libname).o
	$(CXX) $(CXX_FLAGS) -c -o $(BIN)/$(LIBNAME).o cdcc.cpp
	ar rcs $(BIN)/lib$(LIBNAME).a $(BIN)/$(LIBNAME).o
	cp $(BIN)/lib$(LIBNAME).a .

doc:
	doxygen Doxyfile

clean:
	-rm libcdcc.so
	-rm $(BIN)/*
	-rm -rf $(DOC)/*