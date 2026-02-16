#include <vtask/scheduler.hpp>
#include <vtask/task_set.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

using MyClock = std::chrono::high_resolution_clock;

// ============================================================
// Anti-optimization compute workload (FIXED VERSION)
// seed ensures unique result per task
// ============================================================

inline uint64_t heavy_compute(uint32_t n, uint32_t seed)
{
    uint64_t x = seed + 0x123456789ABCDEFULL;

    for (uint32_t i = 1; i < n; ++i)
    {
        x ^= static_cast<uint64_t>(i) * 0x9E3779B97F4A7C15ULL;

        x = (x << 13) | (x >> 51);

        x *= 0xC2B2AE3D27D4EB4FULL;
    }

    return x;
}

// ============================================================
// Single thread benchmark
// ============================================================

uint64_t run_single(uint32_t count, uint32_t iter, uint64_t& checksum)
{
    auto start = MyClock::now();

    uint64_t sum = 0;

    for (uint32_t i = 0; i < count; ++i)
    {
        sum ^= heavy_compute(iter, i);
    }

    checksum = sum;

    auto end = MyClock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// ============================================================
// vtask benchmark
// ============================================================

uint64_t run_vtask(vtask::Scheduler& scheduler, uint32_t count, uint32_t iter, uint64_t& checksum)
{
    auto start = MyClock::now();

    std::vector<uint64_t> results(count);

    vtask::TaskSet task(count, 64, [&](vtask::Range r) {
        for (uint32_t i = r.begin; i < r.end; ++i)
        {
            results[i] = heavy_compute(iter, i);
        }
    });

    scheduler.run(task);

    scheduler.wait(task);

    uint64_t sum = 0;

    for (uint64_t v : results)
        sum ^= v;

    checksum = sum;

    auto end = MyClock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    const uint32_t count = 4000;

    const uint32_t iter = 300000;

    vtask::Scheduler scheduler;

    std::cout << "Threads: " << scheduler.concurrency() << "\n\n";

    // warmup

    uint64_t warm;

    run_single(100, 100000, warm);

    run_vtask(scheduler, 100, 100000, warm);

    // benchmark

    uint64_t checksum1;
    uint64_t checksum2;

    auto single = run_single(count, iter, checksum1);

    auto parallel = run_vtask(scheduler, count, iter, checksum2);

    if (checksum1 != checksum2)
    {
        std::cout << "ERROR: checksum mismatch\n";
    }

    double speedup = static_cast<double>(single) / static_cast<double>(parallel);

    std::cout << "Single: " << single << " ms\n";

    std::cout << "vtask: " << parallel << " ms\n";

    std::cout << "Speedup: " << speedup << "x\n";

    std::cout << "Checksum(single): 0x" << std::hex << checksum1 << "\n";

    std::cout << "Checksum(vtask):  0x" << std::hex << checksum2 << "\n";

    return checksum1 == checksum2 ? 0 : 1;
}
