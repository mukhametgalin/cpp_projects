#include <iostream>
#include "Octagon.h"


int main() {
  Octagon first = {{1, 1}, {0, 0}, {0, 1}, {1, 0}, {0, 2}, {2, 1},
                   {0, -1}, {-1, 0}};

  for (int i = 0; i < Octagon::size; ++i)
  {
    Point a = first.vertex(i);
    std::cout << a.x << ' ' << a.y << std::endl;
  }
  return 0;
}
