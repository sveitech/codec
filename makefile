all:
	rm -rf build
	mkdir build
	cd build; \
		cmake ..
	cd build && ${MAKE} codec_test

clean:
	rm -rf build