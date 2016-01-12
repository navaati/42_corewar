BUILD_DIR ?= build
CMAKE_TREE = $(BUILD_DIR)/CMakeFiles/
BUILD_TYPE ?= Release
GENERATOR ?= "Unix Makefiles"

.PHONY: all clean re fclean cmake

all: $(CMAKE_TREE)
	cmake --build $(BUILD_DIR)

clean: $(CMAKE_TREE)
	cmake --build $(BUILD_DIR) --target clean

re: $(CMAKE_TREE)
	cmake --build $(BUILD_DIR) --clean-first

fclean:
	rm -rf $(BUILD_DIR)

cmake: $(CMAKE_TREE)

$(CMAKE_TREE): $(BUILD_DIR)
	cd $< && cmake -G $(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CURDIR)

$(BUILD_DIR):
	mkdir $@
