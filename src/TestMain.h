#define JDJEJED
#ifdef JDJEJED

#include "HttpClientTester.h"

void RunAllTests() {
    HttpClientTester tester;
    tester.RunAllTests();
}


extern "C" void app_main(void) {
    RunAllTests();
}

#endif