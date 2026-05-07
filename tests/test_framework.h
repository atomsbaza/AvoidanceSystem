#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>

extern int testsRun;
extern int testsFailed;

#define REQUIRE(cond) do { \
  testsRun++; \
  if (!(cond)) { \
    testsFailed++; \
    std::cerr << "  FAIL " << __FILE__ << ":" << __LINE__ << "  " << #cond << "\n"; \
  } \
} while (0)

#endif
