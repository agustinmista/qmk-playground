ifndef KBD
$(error flag KBD is not set)
endif

include keyboards/$(KBD)/env

USER ?= qmk_playground

all: compile

# Initialize the qmk_firmware repo if the clone wasn't recursive
.PHONY: setup
setup:
	git submodule update --init --recursive

# Build the keyboard firmware
.PHONY: compile
compile: setup
	ln -snf $(shell pwd)/keyboards/$(KBD) qmk_firmware/keyboards/$(KBD_PATH)/keymaps/$(USER)
	cd qmk_firmware; qmk lint -kb $(KBD_VARIANT) -km $(USER) --strict
	make BUILD_DIR=$(shell pwd)/build -j1 -C qmk_firmware $(KBD_VARIANT):$(USER)
	rm -f qmk_firmware/keyboards/$(KBD_PATH)/keymaps/$(USER)
