#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "queue.hpp"
int main() {
  con::queue<int> q;
  q.enqueue(4);
  std::copy(q.begin(), q.end(), std::ostream_iterator<int>(std::cout, " "));
}
