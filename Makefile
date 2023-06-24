.PHONY: xx

"":
	if [ -d "build" ]; then \
		cd build && make; \
	else \
		mkdir build; \
		cd build && cmake ..; \
	fi
