all:
	rm -rf build
	mkdir build
	cd build; \
		cmake ..
	cd build && ${MAKE} codec_test

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