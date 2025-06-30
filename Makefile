all:
	cmake -S . -B build
	cmake --build build

run:
	./build/bin/pricer --S0 100 --K 100 --T 1 --r 0.05 --sigma 0.2 --paths 100000 --verbose

clean:
	rm -rf build