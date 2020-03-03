all:
	rm -rf build
	mkdir build
	cd build; \
		cmake ..
	${MAKE} -C build codec_test

arm:
	rm -rf build_arm
	mkdir build_arm
	cd build_arm; \
		cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_PATH}/Toolchain-arm.cmake
	${MAKE} -C build_arm codec_test

# Use the Nlohmann json library. Steal only whats needed.
.PHONY:json
json:
	rm -rf ./tmp
	rm -rf lib/json
	
	git clone https://github.com/nlohmann/json.git ./tmp

	mkdir -p lib/json/include
	cp -r ./tmp/single_include/* lib/json/include
	cp ./tmp/LICENSE.MIT lib/json
	cp ./tmp/README.md lib/json

	rm -rf ./tmp
clean:
	rm -rf build