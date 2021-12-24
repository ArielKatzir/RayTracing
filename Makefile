
INC_DIR = includes
SRC_DIR = src
OBJ_DIR = object

CXXFLAGS  = -Wall -I. -I./includes -I./includes/kd-master/src 

CXX = g++ -O2

OBJECTS = ${OBJ_DIR}/main.o ${OBJ_DIR}/framebuffer.o ${OBJ_DIR}/polymesh.o ${OBJ_DIR}/scene.o ${OBJ_DIR}/utils.o ${OBJ_DIR}/photon_set.o
EXECUTABLE = img_output


${OBJ_DIR}/%.o : ${SRC_DIR}/%.cpp ${INC_DIR}/*.h
	$(CXX) -c $< $(CXXFLAGS) -o $@

all: $(EXECUTABLE)

${EXECUTABLE} : ${OBJECTS}
	$(CXX) ${OBJECTS} -o ${EXECUTABLE} 

clean:
	-rm $(OBJ_DIR)/*.o




