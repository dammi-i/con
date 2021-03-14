#include "queue.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
int main() {
  con::queue<int> qu{1, 4, 3, 9, 5};
  std::copy(qu.begin(), qu.end(), std::ostream_iterator<int>(std::cout, ","));
}
