//
// Created by artur on 07.05.2021.
//

#ifndef OCTAGON__TEST_OCTAGON_H_
#define OCTAGON__TEST_OCTAGON_H_

#include "Octagon.h"
#include <cassert>
#include <chrono>
#include <thread>
#include <random>

void constructorsTest() {
  std::cout << "Constructor tests: \n";

  Octagon first = {{1, 1}, {1, 2}, {99, 10}, {-30, -300}};
  Octagon second = first;
  assert(first == second);

  std::cout << "Ok! Copy constructor works\n";

  std::mt19937 rnd(time(0));

  Octagon third;
  try {
    third.vertex(3);
  } catch (EmptyOctagonError) {
    std::cout << "Ok, exception safety works\n";
  }

  std::cout << "Now I will check octagon, that contains only 1 point\n";

  for (int j = 0; j < 10000; ++j) {
    third.clear();
    Point p = Point((rnd() % 2 ? 1 : -1) * rnd() % int(1e5), (rnd() % 2 ? 1 : -1) * rnd() % int(1e5));
    third.coverPoint(p);

    for (size_t i = 0; i < Octagon::c.size(); ++i) {
      Point q = p;
      q.x += Octagon::c[i].first;
      q.y += Octagon::c[i].second;

      assert(third.isPointInside(q) == -1);
    }
  }

  std::cout << "Ok, one-point-octagon works\n";

  assert(first.isPointInside({1, 1}) >= 0
             && second.isPointInside({1, 1}) >= 0
             && first.isPointInside({1, 2}) >= 0
             && second.isPointInside({1, 2}) >= 0
             && first.isPointInside({99, 10}) >= 0
             && second.isPointInside({99, 10}) >= 0
             && first.isPointInside({-30, -300}) >= 0
             && second.isPointInside({-30, -300}) >= 0);

  std::vector<Point> points;
  for (int i = 0; i < 10000; ++i) {
    points.emplace_back((rnd() % 2 ? 1 : -1) * rnd() % int(1e5),
                        (rnd() % 2 ? 1 : -1) * rnd() % int(1e5));
  }

  Octagon fourth = points;

  for (int i = 0; i < 10000; ++i) {
    assert(fourth.isPointInside(points[i]) >= 0);
  }


  std::cout << "Ok! Points constructor works\n";


  std::cout << "Ok! Limits constructor works\n";

}

void testingFunction() {
  constructorsTest();
}

#endif //OCTAGON__TEST_OCTAGON_H_
