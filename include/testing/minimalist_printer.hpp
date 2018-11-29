#pragma once

// This class provides an alternative output mode which produces minimal amount of information about
// tests. Only failed assertions are disclosed.

#include "gtest/gtest.h"

namespace testing {
// testing::

class MinimalistPrinter : public ::testing::EmptyTestEventListener {
private:
  // Called after a failed assertion or a SUCCEED() invocation.
  virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
    if (test_part_result.failed()) {
      fprintf(stdout, "%s in %s:%d\n%s\n", "*** Failure", test_part_result.file_name(),
              test_part_result.line_number(), test_part_result.summary());
      fflush(stdout);
    }
  }
};

}  // testing
