#include <vtask/scheduler.hpp>
#include <vtask/task_set.hpp>

#include <iostream>
#include <vector>

int main()
{
    // create scheduler
    vtask::Scheduler scheduler;

    // data
    std::vector<int> data(100);

    // create task
    vtask::TaskSet task(data.size(),
                        16, // grain size
                        [&](vtask::Range r) {
                            for (uint32_t i = r.begin; i < r.end; ++i)
                            {
                                data[i] = i * 2;
                            }
                        });

    // run task
    scheduler.run(task);

    // wait
    scheduler.wait(task);

    // verify
    std::cout << data[10] << std::endl; // 20

    return data[10] == 20 ? 0 : 1;
}
