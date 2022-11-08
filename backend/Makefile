
SUB_DIR := src inc

all: ${SUB_DIR}

SUB_DIR-clean := $(SUB_DIR:%=%-clean)
.PHONY: ${SUB_DIR} clean ${SUB_DIR-clean}

src: inc

${SUB_DIR}:
	make -C $@

clean: ${SUB_DIR-clean}

${SUB_DIR-clean}:
	cd $(patsubst %-clean,%, $@) && make clean
