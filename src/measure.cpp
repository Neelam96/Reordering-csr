#include <cpuid.h>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "measure.h"
#include "ittnotify.h"

std::clock_t c_start, c_end;

// example coming from Wikipedia:CPUID
static void call_cpuid(void) {
  unsigned int a, b, c, d;
  int code = 0; // vendor string
  if (!__get_cpuid(code, &a, &b, &c, &d)) {
    std::cerr << std::setbase(16) << std::showbase;
    std::cerr << "Warning: CPUID requests " << code << " unsupported!\n";
  }
#ifdef SP2018_CS377P_DEBUG
  std::cout << std::setbase(16) << std::showbase;
  std::cout << "EAX: " << a << "\nEBX: " << b << "\nECX: " << c << "\nEDX: " << d << "\n";
#endif
}

void start_measurement() {
  // flush data cache
  double* buffer = new double[2000];
  for (int i = 0; i < 2000; i++) {
    buffer[i] = 0.0;
  }
  delete[] buffer;

  c_start = std::clock();
  __itt_resume();
  call_cpuid();
}

void stop_measurement() {
  call_cpuid();
  __itt_pause();
  c_end = std::clock();

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "CPU time used: " << 1000.0 * (c_end -c_start) / CLOCKS_PER_SEC << " ms\n";
}
