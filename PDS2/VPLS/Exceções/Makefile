CC = g++
CFLAGS = -std=c++11 -Wall
TARGET = a.out

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
ENTITIES_DIR = ${SRC_DIR}/entidades

${TARGET}: ${BUILD_DIR}/Hotel.o ${BUILD_DIR}/Quarto.o ${BUILD_DIR}/Reserva.o ${BUILD_DIR}/main.o
	${CC} ${CFLAGS} ${BUILD_DIR}/*.o -o ${TARGET}

${BUILD_DIR}/Hotel.o: ${INCLUDE_DIR}/Hotel.hpp ${ENTITIES_DIR}/Hotel.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${ENTITIES_DIR}/Hotel.cpp -o ${BUILD_DIR}/Hotel.o

${BUILD_DIR}/Quarto.o: ${INCLUDE_DIR}/Quarto.hpp ${ENTITIES_DIR}/Quarto.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${ENTITIES_DIR}/Quarto.cpp -o ${BUILD_DIR}/Quarto.o

${BUILD_DIR}/Reserva.o: ${INCLUDE_DIR}/Reserva.hpp ${ENTITIES_DIR}/Reserva.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${ENTITIES_DIR}/Reserva.cpp -o ${BUILD_DIR}/Reserva.o

${BUILD_DIR}/main.o: ${SRC_DIR}/main.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/main.cpp -o ${BUILD_DIR}/main.o

clean:
	rm -f ${BUILD_DIR}/*.o ${TARGET}
