CFLAGS = -std=c++20 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
bscRND: src/legacy/vulkan-tutor.cc
	g++ $(CFLAGS) -o build/bscRND src/legacy/vulkan-tutor.cc src/swapchainpm.h  $(LDFLAGS)


.PHONY: test clean
test: bscRND
	./bscRND

clean:
	rm -f build
