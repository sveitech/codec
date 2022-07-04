
.PHONY:bootstrap_cmake
bootstrap_cmake:
	rm -rf build
	mkdir -p build

	conan install ./conanfile.txt \
		--install-folder=build \
		--build=missing;
		
	. ./build/activate.sh; \
		cmake . \
			-B build \
			-DCMAKE_MODULE_PATH=$(shell pwd)/build \
			-DCMAKE_BUILD_TYPE=Debug \
	. ./build/deactivate.sh

	make -C build
