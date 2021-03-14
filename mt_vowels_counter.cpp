#include <algorithm>
#include <atomic>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
class Mt_Counter {
    std::atomic<std::size_t> count;
    std::size_t max_threads;
    std::string input;
    std::vector<std::thread> threads;
    bool is_vowel(char c) {
        switch (std::tolower(static_cast<unsigned char>(c))) {
        default:
            return false;
        case 'a':
        case 'e':
        case 'u':
        case 'i':
        case 'o':
            return true;
        }
    }
    void run_threads() {
        std::for_each(threads.begin(), threads.end(),
                      [](auto &th) { th.join(); });
    }

  public:
    explicit Mt_Counter(const std::string &i, std::size_t th = 3) noexcept
        : count(0), input(i) {
        if (th > std::thread::hardware_concurrency()) {
            max_threads = std::thread::hardware_concurrency();
        }
        max_threads = th;
        for (std::size_t u = 0; u <= max_threads; ++u) {
            threads.emplace_back([this, u] {
                auto s_begin = input.begin() + u * (input.size() / max_threads);
                auto s_end =
                    input.begin() + (u + 1) * (input.size() / max_threads);
                std::string substr(s_begin, s_end);
                count.fetch_add(
                    std::count_if(substr.begin(), substr.end(),
                                  [this](char c) { return is_vowel(c); }));
            });
        }
    }
    std::size_t run() {
        run_threads();
        return count;
    }
};
