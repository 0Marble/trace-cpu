BUILD_DIR:=build
EXE=$(BUILD_DIR)/trace

.PHONY: run
run: $(EXE)
	$(EXE)

.PHONY: perf
perf: $(EXE)
	perf record -F 99 -g -s $(EXE) 
	perf script report flamegraph 

.PHONY: configure
configure:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=OFF -DCMAKE_BUILD_TYPE=Debug -DBVH=ON -DNO_PROGRESS_REPORT=ON
	cp $(BUILD_DIR)/compile_commands.json .

configure-release:
	cmake -S . -B $(BUILD_DIR) -DPARALLEL=ON -DCMAKE_BUILD_TYPE=Release -DBVH=ON -DNO_PROGRESS_REPORT=OFF
	cp $(BUILD_DIR)/compile_commands.json .

.PHONY: build
build:
	cmake --build $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: video
video:
	ffmpeg -stream_loop 10 -framerate 30 -start_number 0 -i out/frame-%d.png -c:v libx264 -pix_fmt yuv420p out.mp4
