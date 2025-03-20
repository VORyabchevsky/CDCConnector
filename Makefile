# Компилятор и флаги (main)
CXX		 := g++
CXX_FLAGS   := -std=c++17 -Wall -fpic
INCLUDE	 := -Iinclude
LIBUSB_LIB  := -lusb-1.0  # Флаг для линковки libusb

# Директории
BIN		 := build
DOC		 := docs
SRC		 := src
LIB		 := lib
LIBNAME	 := cdcc

# Цели
EXECUTABLE  := console_example

# Все цели
all: clean dirs $(BIN)/$(EXECUTABLE) #docs

# Создание необходимых директорий
dirs:
	mkdir -p $(BIN)
	mkdir -p $(LIB)

# Запуск примера
run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

# Сборка исполняемого файла
$(BIN)/$(EXECUTABLE): examples/$(EXECUTABLE).cpp $(LIB)/lib$(LIBNAME).so $(LIB)/lib$(LIBNAME).a
	$(CXX) $(CXX_FLAGS) $(INCLUDE) examples/$(EXECUTABLE).cpp -o $(BIN)/$(EXECUTABLE) $(LIBUSB_LIB) -L$(LIB) -l$(LIBNAME)

# Сборка объектного файла из исходного кода
$(BIN)/$(LIBNAME).o: $(SRC)/cdcc.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -c -o $(BIN)/$(LIBNAME).o $(SRC)/$(LIBNAME).cpp

# Сборка динамической библиотеки (.so)
$(LIB)/lib$(LIBNAME).so: $(BIN)/$(LIBNAME).o
	$(CXX) -shared -o $(LIB)/lib$(LIBNAME).so $(BIN)/$(LIBNAME).o $(LIBUSB_LIB)
	cp $(LIB)/lib$(LIBNAME).so .

# Сборка статической библиотеки (.a)
$(LIB)/lib$(LIBNAME).a: $(BIN)/$(LIBNAME).o
	ar rcs $(LIB)/lib$(LIBNAME).a $(BIN)/$(LIBNAME).o
	cp $(LIB)/lib$(LIBNAME).a .

# Генерация документации
doc:
	doxygen Doxyfile

# Очистка временных файлов
clean:
	rm -f $(BIN)/*.o
	rm -f $(LIB)/lib$(LIBNAME).so $(LIB)/lib$(LIBNAME).a
	rm -f lib$(LIBNAME).so lib$(LIBNAME).a
	rm -rf $(DOC)/*