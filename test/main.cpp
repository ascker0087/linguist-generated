//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2020
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "td/utils/tests.h"

#include "td/utils/common.h"
#include "td/utils/crypto.h"
#include "td/utils/logging.h"
#include "td/utils/OptionParser.h"
#include "td/utils/Slice.h"
#include "td/utils/Status.h"

#include <cstring>

#if TD_EMSCRIPTEN
#include <emscripten.h>
#endif

int main(int argc, char **argv) {
  td::init_openssl_threads();

  td::TestsRunner &runner = td::TestsRunner::get_default();
  SET_VERBOSITY_LEVEL(VERBOSITY_NAME(ERROR));

  td::OptionParser options;
  options.add_option('\0', "filter", "Run only specified tests", [&](td::Slice filter) {
    runner.add_substr_filter(filter.str());
    return td::Status::OK();
  });
  options.add_option('\0', "stress", "Run tests infinitely", [&] {
    runner.set_stress_flag(true);
    return td::Status::OK();
  });
  auto result = options.run(argc, argv);
  if (result.is_error() || !result.ok().empty()) {
    LOG(PLAIN) << options;
    return 1;
  }

#if TD_EMSCRIPTEN
  emscripten_set_main_loop(
      [] {
        td::TestsRunner &default_runner = td::TestsRunner::get_default();
        if (!default_runner.run_all_step()) {
          emscripten_cancel_main_loop();
        }
      },
      10, 0);
#else
  runner.run_all();
#endif
  return 0;
}
