#ifndef THREADPOOLTESTS_H
#define THREADPOOLTESTS_H

#include "threading/IThreadPool.h"
#include "threading/IRunnable.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <memory>

// Simple IRunnable implementation for testing
class PrintRunnable : public IRunnable {
    std::string msg;
public:
    explicit PrintRunnable(const std::string& m) : msg(m) {}
    void Run() override {
        printf("[Runnable] %s running on core %d\n", msg.c_str(), xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(100)); // simulate work
        printf("[Runnable] %s finished\n", msg.c_str());
    }
};

void RunThreadPoolTests() {
    printf("=== ThreadPool Tests Start ===\n");

    IThreadPoolPtr pool = Implementation<IThreadPool>::type::GetInstance();

    // Submit lambdas
    for (int i = 0; i < 5; ++i) {
        bool ok = pool->Submit([i]() {
            printf("[Lambda] Task %d running on core %d\n", i, xPortGetCoreID());
            vTaskDelay(pdMS_TO_TICKS(50 + (rand() % 100)));
            printf("[Lambda] Task %d done\n", i);
        });
        printf(ok ? "[PASS] Submitted lambda %d\n" : "[FAIL] Submit lambda %d\n", i);
    }

    // Execute IRunnable objects
    for (int i = 0; i < 3; ++i) {
        auto runnable = std::make_shared<PrintRunnable>("Runnable " + std::to_string(i));
        bool ok = pool->Execute(runnable,
                                ThreadPoolCore::Application,
                                ThreadPoolStackSize::KB_8);
        printf(ok ? "[PASS] Executed runnable %d\n" : "[FAIL] Execute runnable %d\n", i);
    }

    // Wait for all tasks to finish
    bool allDone = pool->WaitForCompletion(5000); // 5s timeout
    printf(allDone ? "[PASS] All tasks completed\n" : "[FAIL] Timeout waiting for tasks\n");

    // Test shutdown
    pool->Shutdown();
    printf(pool->IsShutdown() ? "[PASS] Pool shutdown\n" : "[FAIL] Pool not shutdown\n");

    // Try submitting after shutdown (should fail)
    bool ok = pool->Submit([]() { printf("This should not run\n"); });
    printf(!ok ? "[PASS] Submit rejected after shutdown\n" : "[FAIL] Submit accepted after shutdown\n");

    printf("=== ThreadPool Tests End ===\n");
}

#endif // THREADPOOLTESTS_H