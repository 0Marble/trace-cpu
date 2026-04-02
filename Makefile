BUILD_DIR:=build
EXE=$(BUILD_DIR)/trace

.PHONY: run
run: $(EXE)
	$(EXE)

.PHONY: configure
configure:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=OFF -DCMAKE_BUILD_TYPE=Debug -DBVH=ON
	cp $(BUILD_DIR)/compile_commands.json .

configure-release:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=ON -DCMAKE_BUILD_TYPE=Release -DBVH=ON

.PHONY: build
build:
	cmake --build $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: video
video:
	ffmpeg -stream_loop 10 -framerate 30 -start_number 0 -i out/frame-%d.png -c:v libx264 -pix_fmt yuv420p out.mp4
