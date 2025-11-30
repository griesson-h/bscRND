CFLAGS = -std=c++20 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
bscRND: vulkan.cc
	g++ $(CFLAGS) -o bscRND vulkan.cc swapchainpm.h $(LDFLAGS)


.PHONY: test clean
test: bscRND
	./bscRND

clean:
	rm -f build
