#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "queue.hpp"
int main() {
    con::queue<int> qu{6, 8, 7, 9};
    qu.enqueue(5);
    qu.enqueue(5);
    qu.resize(10);
    con::queue<int> q(qu);
    std::copy(q.begin(), q.end(), std::ostream_iterator<int>(std::cout, " "));
}
