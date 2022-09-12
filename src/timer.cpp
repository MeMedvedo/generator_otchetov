#include "timer.h"

Timer::Timer() { start = std::chrono::_V2::high_resolution_clock::now(); }
Timer::~Timer() {
  end = std::chrono::_V2::high_resolution_clock::now();
  std::chrono::duration<float> Duration{end - start};
  cout << "&&&TIMER: Duration = " << Duration.count() << " seconds" << endl;
}
