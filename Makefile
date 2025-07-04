# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Output executable name
TARGET   := test-bipartite

# Source files
SRCS     := main.cpp bipartite.cpp helpers.cpp congestion_balancing.cpp

# Header files
HDRS     := bipartite.h helpers.h congestion_balancing.h

# Object files
OBJS     := $(SRCS:.cpp=.o)

# Default epsilon (can be overridden on the make command line)
EPS      ?= 0.1

.PHONY: all clean run_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

# run_test: build, then run the binary on each graph/removal pair
# Usage: make run_test           # uses EPS=0.1
#        make run_test EPS=0.2   # uses EPS=0.2
run_test: all
	@echo "=== Running all tests with ε=$(EPS) ==="
	@for g in rand_n*_p*.txt; do \
	  case "$$g" in \
	    *_removals.txt) continue ;; \
	  esac; \
	  r="$${g%.txt}_removals.txt"; \
	  if [ -f "$$g" ] && [ -f "$$r" ]; then \
	    echo "--- $$g + $$r (ε=$(EPS)) ---"; \
	    ./$(TARGET) $$g $(EPS) $$r; \
	  else \
	    echo "!! Missing pair for $$g (need $$r), skipping"; \
	  fi \
	done