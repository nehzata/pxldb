SRC = $(shell find . -type f -name '*.go' -not -path "./database/*")
REGEN_DIRS = $(shell find . -name '*.go' | xargs grep -l //go:generate | xargs dirname | sort | uniq)

all: pxldb

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-15s\033[0m %s\n", $$1, $$2}'

.PHONY: clean
clean:
	$(MAKE) -C llm clean
	@rm -rf build/bin
	@rm -f ggml-metal.metal

.PHONY: regen
regen: $(patsubst %, %/.go-generate, $(REGEN_DIRS)) ## Regenerate all code.
	$(MAKE) -C database regen
	$(MAKE) -C llm libllm.a

.PHONY:
dev: llm/libllm.a ## Run hot reload dev server
	@cp llm/third_party/llama.cpp/ggml-metal.metal .
	@wails dev -extensions go,sql

.PHONY:
test:
	@make -C llm test

.PHONY:
build: regen llm/libllm.a
	wails build
	@cp llm/third_party/llama.cpp/ggml-metal.metal build/bin/PXLDB.app/Contents/Resources/

.PHONY:
dist: build
	codesign -s DE5BEEA73C8841570B1C2055B01B78DD04627B10 -v --timestamp --deep --options runtime build/bin/PXLDB.app
	rm -f build/bin/PXLDB.dmg
	bash tools/create-dmg/create-dmg \
		--volname PXLDB \
		--window-pos 200 120 \
		--window-size 500 300 \
		--icon-size 100 \
		--icon "PXLDB.app" 150 100 \
		--hide-extension "PXLDB.app" \
		--app-drop-link 350 100 \
		--notarize "notarytool-password" \
		build/bin/PXLDB.dmg \
		build/bin/PXLDB.app

.PHONY: lint
lint: ## Lint the code.
	@golangci-lint run

.PHONY:
fmt: ## Format all code.
	@gofmt -l -w $(SRC)

llm/libllm.a:
	@make -C llm libllm.a

# Generate rules for each directory that has files with go:generate directives.
define GO_GENERATE_RULE
$1/.go-generate: $1/*.go
	go generate -x $1
	touch $1/.go-generate
endef

$(foreach dir,$(REGEN_DIRS),$(eval $(call GO_GENERATE_RULE,$(dir))))