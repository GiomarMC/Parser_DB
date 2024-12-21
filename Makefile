#Definir el compilador
CXX = g++

#Definir las banderas del compilador
CXXFLAGS = -Wall -std=c++17

#Nombre del ejecutable
EXEC = programa

#Archivos de origen
SRCS = main.cpp Scanner.cpp Parser.cpp StorageManager.cpp MetadataManager.cpp FileManager.cpp Executor.cpp

#Archivos objeto
OBJS = $(SRCS:.cpp=.o)

#Regla por defecto: compilar todo
all: $(EXEC)

#Regla para enlazar el ejecutable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

#Regla para compilar los archivos .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#Regla para ejecutar el programa
run: $(EXEC)
	./$(EXEC)

#Limpiar los archivos compilados
clean:
	rm -f $(OBJS) $(EXEC)