BUILD_DIR:=build
EXE=$(BUILD_DIR)/trace

.PHONY: run
run: $(EXE)
	$(EXE)

.PHONY: configure
configure:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=OFF -DCMAKE_BUILD_TYPE=Debug
	cp $(BUILD_DIR)/compile_commands.json .

configure-release:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=ON -DCMAKE_BUILD_TYPE=Release

.PHONY: build
build:
	cmake --build $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
