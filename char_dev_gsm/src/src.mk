SRC_DIR := ${PWD}/src
$(info pwd : ${PWD} )
${TARGET}-objs += GSM.o src/device/device.o
#${TARGET}-objs += src/file_ops/file_ops.o

#${TARGET}-objs += ${SRC_DIR}/file_ops/file_ops.o