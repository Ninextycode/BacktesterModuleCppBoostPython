CC = g++ -std=c++14 -fpic
LINK_FLAGS = -shared -lboost_python-py35
TEST_LINK_FLAGS = -lboost_python-py35 -lboost_system -lboost_unit_test_framework 
SOURCES_DIR = BoostPythonBacktesterModuleCpp/SourceFiles
SOURCES = $(shell find $(SOURCES_DIR) -name *.cpp)
OBJECTS_DIR = objects
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(subst $(SOURCES_DIR),$(OBJECTS_DIR),$(OBJECTS))
LIB = x64/backtester.so
TEST_SRC = Test/SourceFiles/Tests.cpp

all: tests $(LIB)
	

$(LIB): $(OBJECTS)
	$(CC) -o $(LIB) $(OBJECTS) $(LINK_FLAGS) 


tests: $(OBJECTS:$(OBJECTS_DIR)/Main.o=) $(OBJECTS_DIR)/Tests.o
	$(CC) -o tests $^ $(TEST_LINK_FLAGS)
	./tests
	rm -rf tests 

$(OBJECTS_DIR)/Tests.o: $(OBJECTS)
	mkdir -p $(OBJECTS_DIR)
	$(CC) -c $(TEST_SRC) -o $(OBJECTS_DIR)/Tests.o -fpermissive
	
$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp
	mkdir -p $(OBJECTS_DIR)  
	$(CC) -c $< -o $@ -I/usr/include/python3.5  

clean:
	rm -rf tests $(OBJECTS_DIR) $(LIB)


