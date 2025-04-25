CC = g++
CFLAGS = -std=c++11 -Wall
TARGET = a.out

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

${BUILD_DIR}/${TARGET}: ${BUILD_DIR}/Metro.o ${BUILD_DIR}/Onibus.o ${BUILD_DIR}/Passageiro.o ${BUILD_DIR}/Transporte.o ${BUILD_DIR}/Trem.o ${BUILD_DIR}/main.o
	${CC} ${CFLAGS} ${BUILD_DIR}/*.o -o ${TARGET}

${BUILD_DIR}/Metro.o: ${INCLUDE_DIR}/Metro.hpp ${SRC_DIR}/entidades/Metro.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/entidades/Metro.cpp -o ${BUILD_DIR}/Metro.o

${BUILD_DIR}/Onibus.o: ${INCLUDE_DIR}/Onibus.hpp ${SRC_DIR}/entidades/Onibus.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/entidades/Onibus.cpp -o ${BUILD_DIR}/Onibus.o

${BUILD_DIR}/Passageiro.o: ${INCLUDE_DIR}/Passageiro.hpp ${SRC_DIR}/entidades/Passageiro.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/entidades/Passageiro.cpp -o ${BUILD_DIR}/Passageiro.o

${BUILD_DIR}/Transporte.o: ${INCLUDE_DIR}/Transporte.hpp ${SRC_DIR}/entidades/Transporte.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/entidades/Transporte.cpp -o ${BUILD_DIR}/Transporte.o

${BUILD_DIR}/Trem.o: ${INCLUDE_DIR}/Trem.hpp ${SRC_DIR}/entidades/Trem.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/entidades/Trem.cpp -o ${BUILD_DIR}/Trem.o

${BUILD_DIR}/main.o: ${SRC_DIR}/main.cpp
	${CC} ${CFLAGS} -I ${INCLUDE_DIR} -c ${SRC_DIR}/main.cpp -o ${BUILD_DIR}/main.o

clean:
	rm -f ${BUILD_DIR}/*.o ${TARGET}
