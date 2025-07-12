CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
LDFLAGS = -lncursesw -lcheck -lsubunit -lm -lpthread

SRC_DIR = .
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = tests
GCOV_DIR = $(BUILD_DIR)/gcov

TETRIS_SRC = $(wildcard $(SRC_DIR)/brick_game/tetris/src/*.c)
TETRIS_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(TETRIS_SRC))
TETRIS_INC = $(SRC_DIR)/brick_game/tetris/include

CLI_SRC = $(wildcard $(SRC_DIR)/gui/cli/src/*.c)
CLI_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CLI_SRC))
CLI_INC = $(SRC_DIR)/gui/cli/include

TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c,$(OBJ_DIR)/tests/%.o,$(TEST_SRC))

TARGET = $(BIN_DIR)/tetris
TEST_TARGET = $(BIN_DIR)/tetris_test

PREFIX = .
BINDIR = $(PREFIX)/usr/local/bin

.PHONY: all install uninstall clean dvi pdf html docs dist test gcov_report

all: clean $(TARGET)

run: all
	$(SRC_DIR)/$(TARGET)

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/tetris

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/tetris

clean:
	rm -rf $(BUILD_DIR)

dvi: $(BUILD_DIR)/doc/tetris.dvi

pdf: dvi
	@echo "Generating PDF documentation..."
	@if command -v dvipdfmx >/dev/null 2>&1 && [ -f $(BUILD_DIR)/doc/tetris.dvi ]; then \
		cd $(BUILD_DIR)/doc && dvipdfmx tetris.dvi -o tetris.pdf > /dev/null 2>&1; \
		echo "PDF file generated: $(BUILD_DIR)/doc/tetris.pdf"; \
		# "$$BROWSER" tetris.pdf; \
	else \
		echo "dvipdfmx not found or DVI file missing. Run 'make dvi' first."; \
	fi

html:
	@echo "Generating HTML documentation with Doxygen..."
	@if command -v doxygen >/dev/null 2>&1; then \
		mkdir -p $(BUILD_DIR)/doc/html; \
		doxygen Doxyfile >/dev/null 2>&1; \
		echo "HTML documentation generated at $(BUILD_DIR)/doc/html/index.html"; \
		echo "To view documentation, open: $(BUILD_DIR)/doc/html/index.html"; \
	else \
		echo "Doxygen not found. Please install doxygen to generate HTML documentation."; \
		exit 1; \
	fi

docs: html pdf
	@echo "All documentation generated successfully!"

dist:
	mkdir -p $(BUILD_DIR)/dist/tetris-1.0
	cp -r brick_game gui tests Makefile $(BUILD_DIR)/dist/tetris-1.0/
	tar -czvf $(BUILD_DIR)/tetris-1.0.tar.gz -C $(BUILD_DIR)/dist tetris-1.0

test: $(TEST_TARGET)
	@echo "\033[1;34mRunning tests...\033[0m"
	$(TEST_TARGET)

gcov_report: CFLAGS += --coverage
gcov_report: LDFLAGS += -lgcov
gcov_report: clean $(TEST_TARGET)
	$(TEST_TARGET)
	@mkdir -p $(GCOV_DIR)
	lcov -t "tetris" -o $(GCOV_DIR)/tetris.info -c -d $(OBJ_DIR)/brick_game/tetris/src
	genhtml -o $(GCOV_DIR)/report $(GCOV_DIR)/tetris.info
	@echo "Coverage report generated at $(GCOV_DIR)/report/index.html"

check: clang cppcheck mem

clang:
	clang-format -style=Google -n $(SRC_DIR)/brick_game/tetris/src/*.c $(SRC_DIR)/gui/cli/src/*.c $(SRC_DIR)/brick_game/tetris/include/*.h $(SRC_DIR)/gui/cli/include/*.h

cppcheck:
	cppcheck --enable=all --std=c11 --check-level=exhaustive --disable=information --suppress=missingIncludeSystem --suppress=missingInclude --suppress=checkersReport $(SRC_DIR)

mem: test
	valgrind --tool=memcheck --leak-check=yes $(BIN_DIR)/tetris_test

$(TARGET): $(TETRIS_OBJ) $(CLI_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TEST_TARGET): $(filter-out $(OBJ_DIR)/gui/cli/src/main.o,$(TETRIS_OBJ)) $(TEST_OBJ)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(TETRIS_INC) -I$(CLI_INC) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(TETRIS_INC) -c $< -o $@

$(BUILD_DIR)/doc/tetris.dvi: doc/tetris.tex
	@echo "Generating DVI documentation..."
	@mkdir -p $(BUILD_DIR)/doc
	@if command -v latex >/dev/null 2>&1; then \
		cp doc/tetris.tex $(BUILD_DIR)/doc/ && \
		(cd $(BUILD_DIR)/doc && latex tetris.tex > /dev/null 2>&1); \
		if [ -f "$(BUILD_DIR)/doc/tetris.dvi" ]; then \
			echo "DVI file generated: $(BUILD_DIR)/doc/tetris.dvi"; \
		else \
			echo "Error generating DVI file"; \
			exit 1; \
		fi \
	else \
		echo "LaTeX not found, cannot generate DVI."; \
		exit 1; \
	fi