#include <vtask/pinned_task.hpp>
#include <vtask/scheduler.hpp>
#include <vtask/task_set.hpp>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

using MyClock = std::chrono::high_resolution_clock;

// ============================================================
// fake heavy compute (integer version, deterministic)
// ============================================================

inline uint64_t fake_work(uint32_t n)
{
    uint64_t x = 0x123456789ABCDEFULL;

    for (uint32_t i = 1; i < n; ++i)
    {
        x ^= static_cast<uint64_t>(i) * 0x9E3779B97F4A7C15ULL;

        x = (x << 13) | (x >> 51);

        x *= 0xC2B2AE3D27D4EB4FULL;
    }

    return x;
}

// ============================================================
// Fake Engine Data
// ============================================================

struct Object
{
    double position;
    double velocity;
    double transform;
    double visible;
};

// ============================================================
// SINGLE THREAD ENGINE
// ============================================================

uint64_t run_single(std::vector<Object>& objects, uint64_t& checksumOut)
{
    auto start = MyClock::now();

    uint64_t checksum = 0;

    checksum ^= fake_work(5'000'000);

    for (auto& o : objects)
    {
        checksum ^= fake_work(200'000);

        o.position += o.velocity;
    }

    for (auto& o : objects)
    {
        checksum ^= fake_work(200'000);

        o.transform = o.position * 2.0;
    }

    for (auto& o : objects)
    {
        checksum ^= fake_work(100'000);

        o.visible = (o.transform > 0.0);
    }

    for (auto& o : objects)
    {
        checksum ^= fake_work(100'000);

        o.transform += 0.000001;
    }

    checksum ^= fake_work(10'000'000);

    checksumOut = checksum;

    auto end = MyClock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// ============================================================
// VTASK FAKE ENGINE
// ============================================================

uint64_t run_vtask(vtask::Scheduler& scheduler, std::vector<Object>& objects, uint64_t& checksumOut)
{
    auto start = MyClock::now();

    std::atomic<uint64_t> checksum {0};

    checksum.fetch_xor(fake_work(5'000'000), std::memory_order_relaxed);

    vtask::TaskSet physics(objects.size(), 64, [&](vtask::Range r) {
        uint64_t local = 0;

        for (uint32_t i = r.begin; i < r.end; ++i)
        {
            local ^= fake_work(200'000);

            objects[i].position += objects[i].velocity;
        }

        checksum.fetch_xor(local, std::memory_order_relaxed);
    });

    vtask::TaskSet animation(objects.size(), 64, [&](vtask::Range r) {
        uint64_t local = 0;

        for (uint32_t i = r.begin; i < r.end; ++i)
        {
            local ^= fake_work(200'000);

            objects[i].transform = objects[i].position * 2.0;
        }

        checksum.fetch_xor(local, std::memory_order_relaxed);
    });

    vtask::TaskSet culling(objects.size(), 64, [&](vtask::Range r) {
        uint64_t local = 0;

        for (uint32_t i = r.begin; i < r.end; ++i)
        {
            local ^= fake_work(100'000);

            objects[i].visible = (objects[i].transform > 0.0);
        }

        checksum.fetch_xor(local, std::memory_order_relaxed);
    });

    vtask::TaskSet renderPrep(objects.size(), 64, [&](vtask::Range r) {
        uint64_t local = 0;

        for (uint32_t i = r.begin; i < r.end; ++i)
        {
            local ^= fake_work(100'000);

            objects[i].transform += 0.000001;
        }

        checksum.fetch_xor(local, std::memory_order_relaxed);
    });

    vtask::PinnedTask submit([&] { checksum.fetch_xor(fake_work(10'000'000), std::memory_order_relaxed); });

    scheduler.run(physics);

    scheduler.run(animation);

    scheduler.run(culling);

    scheduler.run(renderPrep);

    scheduler.run(submit);

    scheduler.waitAll();

    checksumOut = checksum.load(std::memory_order_relaxed);

    auto end = MyClock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// ============================================================
// MAIN
// ============================================================

int main()
{
    const uint32_t objectCount = 2000;

    std::vector<Object> objects(objectCount);

    for (auto& o : objects)
    {
        o.position  = 1.0;
        o.velocity  = 1.0;
        o.transform = 0.0;
        o.visible   = 0.0;
    }

    vtask::Scheduler scheduler;

    std::cout << "Threads: " << scheduler.concurrency() << "\n\n";

    // warmup

    {
        uint64_t cs1, cs2;

        run_single(objects, cs1);

        run_vtask(scheduler, objects, cs2);
    }

    uint64_t csSingle;
    uint64_t csParallel;

    auto single = run_single(objects, csSingle);

    auto parallel = run_vtask(scheduler, objects, csParallel);

    double speedup = static_cast<double>(single) / static_cast<double>(parallel);

    std::cout << "Single thread: " << single << " ms\n";

    std::cout << "vtask: " << parallel << " ms\n";

    std::cout << "Speedup: " << speedup << "x\n";

    std::cout << "Checksum(single): 0x" << std::hex << csSingle << "\n";

    std::cout << "Checksum(vtask):  0x" << std::hex << csParallel << "\n";

    return csSingle == csParallel ? 0 : 1;
}
