ifndef KBD
$(error flag KBD is not set)
endif

include keyboards/$(KBD)/env

USER ?= qmk-playground

all: compile

# Initialize the qmk_firmware repo if the clone wasn't recursive
.PHONY: setup
setup:
	git submodule update --init --recursive

# Link the keymap and compile the keyboard firmware
.PHONY: compile
compile: setup
	ln -snf $(shell pwd)/keyboards/$(KBD) qmk_firmware/keyboards/$(KBD_PATH)/keymaps/$(USER)
	cd qmk_firmware && \
		qmk lint -kb $(KBD_VARIANT) -km $(USER) && \
		qmk compile -e BUILD_DIR=$(shell pwd)/build -kb $(KBD_VARIANT) -km $(USER)
	rm -f qmk_firmware/keyboards/$(KBD_PATH)/keymaps/$(USER)
