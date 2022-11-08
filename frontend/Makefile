
SUB_DIR := $(filter-out ./, $(shell find ./ -maxdepth 1 -type d))

all: ${SUB_DIR}

SUB_DIR-clean := $(SUB_DIR:%=%-clean)
.PHONY: ${SUB_DIR} clean ${SUB_DIR-clean}

${SUB_DIR}:
	make -C $@

clean: ${SUB_DIR-clean}

${SUB_DIR-clean}:
	cd $(patsubst %-clean,%, $@) && make clean
