
TOP_DIR ?= $(abspath $(shell pwd)/../../)
CUR_DIR := $(shell pwd)

BASE_ASSET := web_assets
BASE_FRONT := $(TOP_DIR)/frontend/

RANDOM := $(shell /bin/bash -c "echo $$RANDOM")

# get sub-folders in frontend folder.
ALL_SUB_DIRS := $(shell find $(BASE_FRONT) -maxdepth 1 -type d)
ALL_ASSETS := $(strip $(subst $(dir $(BASE_FRONT)),,$(ALL_SUB_DIRS)))

ALL_WEB_ASSETS_H := $(foreach sub_folder, $(ALL_ASSETS), $(BASE_ASSET)_$(sub_folder).h)

.PHONY: all clean

all: $(ALL_WEB_ASSETS_H)

clean: 
	@rm -f $(ALL_WEB_ASSETS_H)

# convert char to upper
define UPPER
    $(shell echo $1 | tr 'a-z' 'A-Z')
endef

# output assets h header.
define assets_h_header 
    echo -e "#ifndef _WEB_ASSETS_$(strip $(call UPPER, $1))_H_\n#define _WEB_ASSETS_$(strip $(call UPPER, $1))_H_\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\nstatic unsigned char web_assets_zip_$(strip $1)[] = {"
endef

# output assets h tail
define assets_h_tail
    echo -e "\nvoid get_web_assets_$(strip $1)(const unsigned char** buf, unsigned int* buf_len)\n{\n    if (0 != buf)       *buf = web_assets_zip_$(strip $1);\n    if (0 != buf_len)   *buf_len = sizeof(web_assets_zip_$(strip $1));\n}\n\n#ifdef __cplusplus\n};\n#endif\n\n#endif\n"
endef

# build one sub folder
define BUILD_SUB_FOLDER
ASSETS_DIRS_$1 := $$(shell find $$(BASE_FRONT)$1 -type d)
ASSETS_FILES_$1 := $$(foreach dir, $$(ASSETS_DIRS_$1), $$(wildcard $$(dir)/*.*))

$$(BASE_ASSET)_$1.h: $$(ASSETS_FILES_$1)
	@pushd $$(BASE_FRONT)$1/dist > /dev/null; zip -q -r $$(CUR_DIR)/$$(BASE_ASSET).zip ./* && popd > /dev/null;
	@xxd -i $$(BASE_ASSET).zip $(CUR_DIR)/$$@_$$(RANDOM)
	@rm -f  $$(BASE_ASSET).zip
	@echo "generate $(CUR_DIR)/$$@"
	@$$(call assets_h_header, $1) > $(CUR_DIR)/$$@
	@head -n -1 $(CUR_DIR)/$$@_$$(RANDOM) | tail -n +2 >> $(CUR_DIR)/$$@
	@$$(call assets_h_tail, $1)  >>  $(CUR_DIR)/$$@
	@rm -f  $(CUR_DIR)/$$@_$$(RANDOM)
endef

# recurse to build all sub folders.
$(foreach sub_folder, $(strip $(ALL_ASSETS)), $(eval $(call BUILD_SUB_FOLDER,$(strip $(sub_folder)))))
