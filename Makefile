# デフォルトのターゲット
all: build

# ビルドターゲット
build:
	mkdir -p build
	@if [ "$(GOOS)" = "windows" ]; then \
		GOOS=windows GOARCH=amd64 go build -o build/qase.exe ./cmd/qase; \
	elif [ "$(GOOS)" = "linux" ]; then \
		GOOS=linux GOARCH=amd64 go build -o build/qase ./cmd/qase; \
	elif [ "$(GOOS)" = "darwin" ]; then \
		GOOS=darwin GOARCH=amd64 go build -o build/qase ./cmd/qase; \
	fi

# Windows 用
windows:
	mkdir -p build
	@echo "Building for Windows"
	GOOS=windows GOARCH=amd64 go build -o build/qase.exe ./cmd/qase

# Linux 用
linux:
	mkdir -p build
	@echo "Building for Linux"
	GOOS=linux GOARCH=amd64 go build -o qase ./cmd/qase

# macOS 用
macos:
	mkdir -p build
	@echo "Building for macOS"
	GOOS=darwin GOARCH=amd64 go build -o qase ./cmd/qase
