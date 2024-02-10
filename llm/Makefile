OUTDIR = ./bin
SRC = $(shell find src -type f -name '*.cpp')
TEST = $(shell find test -type f -name '*.cpp')
INC = $(shell find . -type f -name '*.h' -o -name '*.hpp')
SRC_OBJ = $(SRC:cpp=o)
TEST_OBJ = $(TEST:cpp=o)

CXX_INCLUDES = -I src/ \
	-DFMT_HEADER_ONLY -I third_party/fmt/include \
	-I third_party/llama.cpp \
	-I third_party/spdlog/include
CXX_FLAGS = $(CXX_INCLUDES) \
	-mmacosx-version-min=11.0 \
	-D_XOPEN_SOURCE=600 -D_DARWIN_C_SOURCE -DNDEBUG \
	-DGGML_USE_ACCELERATE -DACCELERATE_NEW_LAPACK \
	-DACCELERATE_LAPACK_ILP64 -DGGML_USE_METAL \
	-DSPDLOG_FMT_EXTERNAL \
	-Wall -Wextra -pedantic -Wcast-qual -Wno-unused-function \
	-Wmissing-declarations -Wmissing-noreturn -pthread \
	-Wunreachable-code-break -Wunreachable-code-return \
	-Wmissing-prototypes -Wextra-semi -std=c++14 -fPIC

LDFLAGS = -L third_party/llama.cpp -lllama \
	-framework Foundation -framework Metal \
	-framework MetalKit -framework Accelerate

ifdef NDEBUG
	CXX_FLAGS += -O3
else
	CXX_FLAGS += -g -O0
endif

all: libllm.a

.PHONY: clean
clean:
	@make -C third_party/llama.cpp clean
	@find src \( -name "*.o" -o -name "*.d" \) -delete
	@rm -rf bin

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

.PHONY: test
test: bin/test libcatch2 $(SRC_OBJ) $(TEST_OBJ)
	$(OUTDIR)/test

bin/test: libllama libcatch2 $(SRC_OBJ) $(TEST_OBJ)
	mkdir -p $(OUTDIR)
	$(CXX) \
		-I third_party/Catch2/src -I third_party/Catch2/build/generated-includes \
		$(CXX_FLAGS) $(LDFLAGS) -L third_party/Catch2/build/src -lCatch2 -lCatch2Main \
		-o $(OUTDIR)/test $(SRC_OBJ) $(TEST_OBJ)

libllama:
	@make -C third_party/llama.cpp libllama.a

libllm.a: libllama $(SRC_OBJ)
	@ar rcs libllm.a $(SRC_OBJ)

libcatch2:
	@mkdir -p third_party/Catch2/build
	@cd third_party/Catch2/build && cmake .. && cmake --build .

src/%.o: src/%.cpp $(INC)
	$(CXX) $(CXX_FLAGS) -MMD -o $@ -c $<

test/%.o: test/%.cpp $(INC)
	$(CXX) -I third_party/Catch2/src -I third_party/Catch2/build/generated-includes $(CXX_FLAGS) -I src/ -MMD -o $@ -c $<

-include $(SRC:.cpp=.d)
-include $(TEST:.cpp=.d)
