BUILD_DIR:=build
EXE=$(BUILD_DIR)/trace

CFLAGS:=-fPIC -Wall -Wextra -ggdb -fsanitize=address -fsanitize=undefined

.PHONY: run
run: $(EXE)
	$(EXE)

.PHONY: configure
configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_CXX_FLAGS="$(CFLAGS)"
	cp $(BUILD_DIR)/compile_commands.json .

.PHONY: build
build:
	cmake --build $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
