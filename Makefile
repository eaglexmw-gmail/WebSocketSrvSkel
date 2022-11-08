export TOP_DIR := ${shell pwd}
export OS_NAME := $(shell uname -s)
export CPU_TYPE := $(shell uname -m)
export BUILD_TYPE ?= Release

SUB_DIR := frontend backend

all: ${SUB_DIR}

SUB_DIR-clean := $(SUB_DIR:%=%-clean)
.PHONY: ${SUB_DIR} clean ${SUB_DIR-clean}

${SUB_DIR}:
	make -C $@

clean: ${SUB_DIR-clean}

${SUB_DIR-clean}:
	cd $(patsubst %-clean,%, $@) && make clean
