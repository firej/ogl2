# Обёртка над CMake для сборки и запуска движка OGL2.
#
#   make            — конфигурация + сборка
#   make run        — собрать и запустить движок
#   make test       — собрать и прогнать GL smoke-тест
#   make clean      — удалить каталог сборки
#   make rebuild    — пересобрать с нуля

BUILD_DIR ?= build
JOBS      ?= 4
CMAKE     ?= cmake

# macOS: GLFW/FFmpeg из Homebrew не всегда лежат в стандартном пути pkg-config
BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
ifneq ($(BREW_PREFIX),)
export PKG_CONFIG_PATH := $(BREW_PREFIX)/lib/pkgconfig:$(BREW_PREFIX)/share/pkgconfig:$(PKG_CONFIG_PATH)
endif

.PHONY: all build run test clean rebuild

all: build

# Конфигурация (с тестами) + сборка движка и теста
build:
	$(CMAKE) -S . -B $(BUILD_DIR) -DOGL2_BUILD_TESTS=ON
	$(CMAKE) --build $(BUILD_DIR) -j$(JOBS)

# Запуск движка — обязательно из $(BUILD_DIR), там лежит data/
run: build
	cd $(BUILD_DIR) && ./OGL2

# Регрессионный smoke-тест GL-бэкенда (offscreen, печатает PASS/FAIL)
test: build
	./$(BUILD_DIR)/gl_smoke

clean:
	rm -rf $(BUILD_DIR)
	rm -rf OGL2/Debug Font_creator/Debug

rebuild: clean build
