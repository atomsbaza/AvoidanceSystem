CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wno-unused-parameter \
            -I. -Istubs -I../avoidance_system

SRCS := \
  ../avoidance_system/avoidance_logic.cpp \
  ../avoidance_system/movement.cpp \
  ../avoidance_system/sensors.cpp \
  test_main.cpp \
  test_sensors.cpp \
  test_movement.cpp \
  test_avoidance_logic.cpp

run_tests: $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o run_tests

test: run_tests
	./run_tests

clean:
	rm -f run_tests

.PHONY: test clean
