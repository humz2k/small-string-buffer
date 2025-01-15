#include <chrono>
#include <iostream>
#include <smallstring/smallstring.hpp>

double __attribute__((noinline)) smallstring_benchmark() {
    smallstring::Buffer buff{2048};
    std::chrono::high_resolution_clock::time_point start, end;
    {
        start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 100; j++) {
            buff.clear();
            for (int i = 0; i < 100; i++) {
                buff.push("hello");
            }
        }
        end = std::chrono::high_resolution_clock::now();
    }
    auto time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    // hopefully this stops the compiler from optimizing out the loop lol
    static volatile int tmp = buff.view().size();
    static volatile std::string_view tmp1 = buff.view();
    return time;
}

double __attribute__((noinline)) stdstring_benchmark() {
    std::string str;
    str.reserve(2048);
    std::chrono::high_resolution_clock::time_point start, end;
    {
        start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 100; j++) {
            str.clear();
            for (int i = 0; i < 100; i++) {
                str.append("hello");
            }
        }
        end = std::chrono::high_resolution_clock::now();
    }
    auto time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count();
    static volatile int tmp = str.size();
    static volatile std::string_view tmp1 = str;
    return time;
}

int main() {
    int runs = 10000;
    double smallstr_average_time = 0;
    double stdstr_average_time = 0;
    for (int i = 0; i < runs; i++) {
        smallstr_average_time += smallstring_benchmark() / ((double)runs);
        stdstr_average_time += stdstring_benchmark() / ((double)runs);
    }
    std::cout << "Results:\n" << "   - smallstr::Buffer = " << smallstr_average_time << "ns\n"
              << "   - std::string      = " << stdstr_average_time << "ns" << std::endl;
    return 0;
}