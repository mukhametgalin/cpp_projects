#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

const double PI = acos(-1);

bool doubleEquality(double a, double b, double eps = 1e-4) {
  return fabs(a - b) < eps;
}

class Point;

class Vector;

class Line;

class Shape;

class Polygon;

class Ellipse;

class Circle;

class Rectangle;

class Square;

class Triangle;

class Point {
 public:
  double x;
  double y;
  Point();
  Point(double x, double y);
  explicit Point(Vector a);
  Point& operator=(Point a);
  Point& operator=(Vector a);
  bool operator==(Point a) const;

  bool operator!=(Point a) const;
  double squared_distance(Point a) const;

  double distance(Point a) const;
  void scale(Point center, double coefficient);

  void rotate(Point center, double angle);

  void reflex(Point center);
  void reflex(Line axis);
};

namespace point_namespace {

Point average_point(Point first, Point second);
}

class Vector {
 public:
  double x;
  double y;
  explicit Vector(Point a);
  Vector();
  Vector(double x, double y);
  Vector& operator=(Vector a);

  Vector& operator=(Point a);

  double squared_length() const;

  double length() const;
  bool operator==(Vector a) const;

  bool operator!=(Vector a) const;
  Vector operator-();
  Vector operator-=(Vector a);
  Vector& operator+=(Vector a);
  Vector operator+(Vector a);
  Vector& operator*=(double a);
  Vector operator-(Vector a);
  Vector rotate(double angle);
};

namespace vector_namespace {
double orientedArea(Vector first, Vector second);
double scalarProduct(Vector first, Vector second);
double getAngle(Vector first, Vector second);
}

class Line {
 public:
  double A;
  double B;
  double C;

  void normalize();

 public:
  Line(Point first, Point second);
  Line(double slope, double shift);
  Line(Vector direction, Point point);
  Line();
  Line(Point a, double slope);
  bool operator==(Line a) const;

  bool operator!=(Line a) const;

  double distancePoint(Point a) const;
  Vector getNormalVector() const;
};

namespace line_namespace {
Point intersection(Line first, Line second);
}

class Shape {
 public:
  virtual ~Shape() {}
  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual bool operator==(const Shape& another) const = 0;
  virtual bool operator!=(const Shape& another) const = 0;
  virtual bool isCongruentTo(const Shape& another) const = 0;
  virtual bool isSimilarTo(const Shape& another) const = 0;
  virtual bool containsPoint(Point point) const = 0;
  virtual void rotate(Point center, double angle) = 0;
  virtual void reflex(Point center) = 0;
  virtual void reflex(Line axis) = 0;
  virtual void scale(Point center, double coefficient) = 0;
};

namespace polygon_namespace {

bool compareInCongruentity(const Polygon&, const Polygon&);
bool compareInEquality(Polygon polygon, Polygon& another_polygon);
}

class Polygon : public Shape {
  friend bool polygon_namespace::compareInCongruentity(const Polygon&,
                                                       const Polygon&);

 protected:
  std::vector<Point> vertices;

 private:
  std::vector<Vector> getEdges() const;

 public:
  Polygon();
  Polygon(std::vector<Point> a);
  Polygon(std::initializer_list<Point> a);

  const std::vector<Point> getVertices();

  int verticesCount() const;

  double perimeter() const;
  double area() const;

  bool operator==(const Shape& another) const;
  bool operator!=(const Shape& another) const;
  void rotate(Point center, double angle);

  bool isCongruentTo(const Shape& another) const;
  bool isSimilarTo(const Shape& another) const;
  bool containsPoint(Point point) const;
  void reflex(Point center);
  void reflex(Line axis);
  void scale(Point center, double coefficient);
  Polygon reversed() const;
  Polygon reflected(Line l) const;

  bool isConvex() const;
};

class Ellipse : public Shape {
 protected:
  Point F1;
  Point F2;
  double D;

 public:
  double perimeter() const;
  double area() const;
  bool operator==(const Shape& another) const;
  bool operator!=(const Shape& another) const;
  bool isCongruentTo(const Shape& another) const;
  bool isSimilarTo(const Shape& another) const;
  bool containsPoint(Point point) const;
  void rotate(Point center, double angle);
  void reflex(Point center);
  void reflex(Line axis);
  void scale(Point center, double coefficient);
  double A() const;
  double B() const;
  double C() const;
  Point center() const;
  std::pair<Point, Point> focuses() const;
  std::pair<Line, Line> directrices() const;
  double eccentricity() const;

  Ellipse(Point F1, Point F2, double D);
  Ellipse();
};

class Circle : public Ellipse {
 public:
  Circle(Point center, double radius);
  Circle();
  double radius() const;
};

class Rectangle : public Polygon {
 public:
  Point center() const;
  std::pair<Line, Line> diagonals() const;
  Rectangle(Point first, Point second, double relation);
  Rectangle();
  Rectangle(std::initializer_list<Point> init);
};

class Square : public Rectangle {
 public:
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Square(Point first, Point second);
};

class Triangle : public Rectangle {
 public:
  Triangle(Point a, Point b, Point c);
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;
  Point circumscribedCircleCenter() const;
  Point inscribedCircleCenter() const;
};

Point::Point() : x(0), y(0) {}
Point::Point(double x, double y) : x(x), y(y) {}

Point::Point(Vector a) : x(a.x), y(a.y) {}
Point& Point::operator=(Point a) {
  x = a.x;
  y = a.y;
  return *this;
}
Point& Point::operator=(Vector a) {
  x = a.x;
  y = a.y;
  return *this;
}

bool Point::operator==(Point a) const {
  return doubleEquality(x, a.x) && doubleEquality(y, a.y);
}

bool Point::operator!=(Point a) const { return !(*this == a); }
double Point::squared_distance(Point a) const {
  return (x - a.x) * (x - a.x) + (y - a.y) * (y - a.y);
}

double Point::distance(Point a) const { return sqrt(squared_distance(a)); }
void Point::scale(Point center, double coefficient) {
  Vector vec = static_cast<Vector>(*this);
  vec -= static_cast<Vector>(center);
  vec *= coefficient;
  vec += static_cast<Vector>(center);
  *this = vec;
}

void Point::rotate(Point center, double angle) {
  Vector help_vec = static_cast<Vector>(*this);
  help_vec -= static_cast<Vector>(center);
  help_vec.rotate(angle);
  help_vec += static_cast<Vector>(center);
  *this = help_vec;
}

void Point::reflex(Point center) {
  Vector help_vec = static_cast<Vector>(center);
  help_vec -= static_cast<Vector>(*this);
  x = center.x + help_vec.x;
  y = center.y + help_vec.y;
}

void Point::reflex(Line axis) {
  Vector vec = axis.getNormalVector();

  if (axis.A * x + axis.B * y + axis.C > 0) vec = -vec;
  double r_sq = axis.distancePoint(*this) / vec.length();
  vec *= r_sq * 2;
  x += vec.x;
  y += vec.y;
}

namespace point_namespace {

Point average_point(Point first, Point second) {
  Point result;
  result.x = (first.x + second.x) / 2;
  result.y = (first.y + second.y) / 2;
  return result;
}
}

Vector::Vector(Point a) : x(a.x), y(a.y) {}
Vector::Vector() : x(0), y(0) {}

Vector::Vector(double x, double y) : x(x), y(y) {}
Vector& Vector::operator=(Vector a) {
  x = a.x;
  y = a.y;
  return *this;
}

Vector& Vector::operator=(Point a) {
  x = a.x;
  y = a.y;
  return *this;
}

double Vector::squared_length() const { return x * x + y * y; }

double Vector::length() const { return sqrt(x * x + y * y); }
bool Vector::operator==(Vector a) const {
  return doubleEquality(x, a.x) && doubleEquality(y, a.y);
}

bool Vector::operator!=(Vector a) const { return !(*this == a); }
Vector Vector::operator-() {
  Vector cpy = *this;
  cpy *= -1;
  return cpy;
}

Vector Vector::operator-=(Vector a) {
  x -= a.x;
  y -= a.y;
  return *this;
}

Vector& Vector::operator+=(Vector a) {
  x += a.x;
  y += a.y;
  return *this;
}
Vector Vector::operator+(Vector a) {
  Vector result = *this;
  result += a;
  return result;
}

Vector Vector::operator-(Vector a) {
  Vector result = *this;
  result -= a;
  return result;
}

Vector& Vector::operator*=(double a) {
  x *= a;
  y *= a;
  return *this;
}
Vector Vector::rotate(double angle) {
  double x_help = x;
  double y_help = y;
  angle = angle / 180 * PI;
  x = x_help * cos(angle) - y_help * sin(angle);
  y = x_help * sin(angle) + y_help * cos(angle);
  return *this;
}

namespace vector_namespace {

double orientedArea(Vector first, Vector second) {
  return first.x * second.y - first.y * second.x;
}

double scalarProduct(Vector first, Vector second) {
  return first.x * second.x + first.y * second.y;
}

double getAngle(Vector first, Vector second) {
  return atan2(orientedArea(first, second), scalarProduct(first, second)) *
         180 / PI;
}
}

void Line::normalize() {
  if (doubleEquality(A, 0)) {
    A /= B;
    C /= B;
    B /= B;
  } else {
    B /= A;
    C /= A;
    A /= A;
  }
}
Line::Line(Point first, Point second) {
  A = first.y - second.y;
  B = second.x - first.x;
  C = (second.y - first.y) * second.x + (first.x - second.x) * second.y;
  normalize();
}
Line::Line(double slope, double shift) {
  A = -slope;
  B = 1;
  C = -shift;
  normalize();
}

Line::Line(Vector direction, Point point) {
  A = -direction.y;
  B = direction.x;
  C = -A * point.x - B * point.y;
  normalize();
}

Line::Line() : A(1), B(0), C(0) {}

Line::Line(Point a, double slope) {
  A = -slope;
  B = 1;
  double shift = a.y - slope * a.x;
  C = -shift;
  normalize();
}

bool Line::operator==(Line a) const {
  return doubleEquality(A, a.A) && doubleEquality(B, a.B) &&
         doubleEquality(C, a.C);
}

bool Line::operator!=(Line a) const { return !(*this == a); }

double Line::distancePoint(Point a) const {
  return fabs(A * a.x + B * a.y + C) / sqrt(A * A + B * B);
}
Vector Line::getNormalVector() const {
  Vector n(A, B);
  return n;
}
namespace line_namespace {

Point intersection(Line first, Line second) {
  Point result;
  result.x = -(first.C * second.B - second.C * first.B) /
             (first.A * second.B - second.A * first.B);
  result.y = -(first.A * second.C - second.A * first.C) /
             (first.A * second.B - second.A * first.B);
  return result;
}
}

Polygon::Polygon() : vertices(0) {}
Polygon::Polygon(std::vector<Point> a) : vertices(a) {}
Polygon::Polygon(std::initializer_list<Point> a) : vertices(a) {}

const std::vector<Point> Polygon::getVertices() { return vertices; }

int Polygon::verticesCount() const { return static_cast<int>(vertices.size()); }
bool Polygon::isConvex() const {
  Vector first = static_cast<Vector>(vertices[0]);
  Vector second = static_cast<Vector>(vertices[1]);
  second -= static_cast<Vector>(vertices[0]);
  first -= static_cast<Vector>(vertices.back());
  bool fl = 0;
  if (vector_namespace::orientedArea(first, second) < 0) fl = 1;
  for (size_t i = 1; i < vertices.size(); ++i) {
    int j = (i + 1) % vertices.size();
    first = static_cast<Vector>(vertices[i]);
    second = static_cast<Vector>(vertices[j]);
    second -= static_cast<Vector>(vertices[i]);
    first -= static_cast<Vector>(vertices[i - 1]);
    if ((!fl && vector_namespace::orientedArea(first, second) < 0) ||
        (fl && vector_namespace::orientedArea(first, second) > 0))
      return false;
  }
  return true;
}

double Polygon::perimeter() const {
  double sum = 0;
  for (size_t i = 1; i < vertices.size(); ++i) {
    sum += vertices[i].distance(vertices[i - 1]);
  }
  sum += vertices.back().distance(vertices.front());

  return sum;
}
double Polygon::area() const {
  double sum = 0;
  for (size_t i = 1; i < vertices.size(); ++i) {
    sum += vector_namespace::orientedArea(static_cast<Vector>(vertices[i]),
                                          static_cast<Vector>(vertices[i - 1]));
  }
  sum += vector_namespace::orientedArea(static_cast<Vector>(vertices[0]),
                                        static_cast<Vector>(vertices.back()));
  if (sum < 0) sum = -sum;
  return sum / 2;
}

void Polygon::rotate(Point center, double angle) {
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].rotate(center, angle);
  }
}

std::vector<Vector> Polygon::getEdges() const {
  std::vector<Vector> result;
  for (size_t i = 1; i < vertices.size(); ++i)
    result.push_back(static_cast<Vector>(vertices[i]) -
                     static_cast<Vector>(vertices[i - 1]));
  result.push_back(static_cast<Vector>(vertices[0]) -
                   static_cast<Vector>(vertices.back()));
  return result;
}
bool Polygon::operator==(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
  Polygon another_polygon = dynamic_cast<const Polygon&>(another);
  if (vertices.size() != another_polygon.vertices.size()) return false;
  if (polygon_namespace::compareInEquality(*this, another_polygon)) return true;
  std::reverse(another_polygon.vertices.begin(),
               another_polygon.vertices.end());
  if (polygon_namespace::compareInEquality(*this, another_polygon)) return true;
  return false;
}
bool Polygon::operator!=(const Shape& another) const {
  return !(*this == another);
}

namespace polygon_namespace {

bool compareInEquality(Polygon polygon, Polygon& another_polygon) {
  std::vector<Point> first = polygon.getVertices();
  std::vector<Point> second = another_polygon.getVertices();

  for (size_t i = 0; i < first.size(); ++i) {
    bool fl = 0;
    for (size_t j = 0; j < second.size(); ++j) {
      if (first[(i + j) % first.size()] != second[j]) {
        fl = 1;
        break;
      }
    }
    if (!fl) return true;
  }
  return false;
}

bool compareInCongruentity(const Polygon& first_polygon,
                           const Polygon& second_polygon) {
  std::vector<Vector> first = first_polygon.getEdges();
  std::vector<Vector> second = second_polygon.getEdges();
  for (size_t i = 0; i < second.size(); ++i) {
    double angle = vector_namespace::getAngle(second[i], first[0]);
    for (size_t j = 0; j < second.size(); ++j) {
      second[j].rotate(angle);
    }
    bool fl = 0;
    for (size_t j = 0; j < second.size(); ++j) {
      if (first[j] != second[(j + i) % second.size()]) {
        fl = 1;
        break;
      }
    }
    if (!fl) return true;
  }
  return false;
}
}

Polygon Polygon::reversed() const {
  Polygon result = *this;
  std::reverse(result.vertices.begin(), result.vertices.end());
  return result;
}

Polygon Polygon::reflected(Line l) const {
  Polygon result = *this;
  result.reflex(l);
  return result;
}
bool Polygon::isCongruentTo(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
  Polygon another_p = dynamic_cast<const Polygon&>(another);
  if (verticesCount() != another_p.verticesCount()) return false;
  Line l;
  return polygon_namespace::compareInCongruentity(*this, another_p) ||
         polygon_namespace::compareInCongruentity(*this,
                                                  another_p.reversed()) ||
         polygon_namespace::compareInCongruentity(*this,
                                                  another_p.reflected(l)) ||
         polygon_namespace::compareInCongruentity(
             *this, another_p.reflected(l).reversed());
}
bool Polygon::isSimilarTo(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) == nullptr) return false;
  Polygon another_p = dynamic_cast<const Polygon&>(another);
  double another_p_perimeter = another_p.perimeter();
  double this_perimeter = perimeter();
  Point center;
  another_p.scale(center, this_perimeter / another_p_perimeter);
  return another_p.isCongruentTo(*this);
}

bool Polygon::containsPoint(Point point) const {
  double sum_angle = 0;
  for (size_t i = 1; i < vertices.size(); ++i) {
    Vector first = static_cast<Vector>(vertices[i - 1]);
    first -= static_cast<Vector>(point);
    Vector second = static_cast<Vector>(vertices[i]);
    second -= static_cast<Vector>(point);
    sum_angle += vector_namespace::getAngle(first, second);
  }
  Vector first = static_cast<Vector>(vertices.back());
  first -= static_cast<Vector>(point);
  Vector second = static_cast<Vector>(vertices.front());
  second -= static_cast<Vector>(point);
  sum_angle += vector_namespace::getAngle(first, second);

  return !doubleEquality(sum_angle, static_cast<double>(0));
}
void Polygon::reflex(Point center) {
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].reflex(center);
  }
}

void Polygon::reflex(Line axis) {
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].reflex(axis);
  }
}
void Polygon::scale(Point center, double coefficient) {
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].scale(center, coefficient);
  }
}

Ellipse::Ellipse(Point F1, Point F2, double D) : F1(F1), F2(F2), D(D) {}
Ellipse::Ellipse() : F1(), F2(), D(0) {}
bool Ellipse::operator==(const Shape& another) const {
  if (dynamic_cast<const Ellipse*>(&another) == nullptr) return false;
  Ellipse another_ellipse = dynamic_cast<const Ellipse&>(another);
  return ((F1 == another_ellipse.F1 && F2 == another_ellipse.F2) ||
          (F1 == another_ellipse.F2 && F2 == another_ellipse.F1)) &&
         D == another_ellipse.D;
}
bool Ellipse::operator!=(const Shape& another) const {
  return (*this == another);
}

bool Ellipse::containsPoint(Point point) const {
  return D > point.distance(F1) + point.distance(F2);
}

void Ellipse::rotate(Point center, double angle) {
  F1.rotate(center, angle);
  F2.rotate(center, angle);
}

void Ellipse::reflex(Point center) {
  F1.reflex(center);
  F2.reflex(center);
}

void Ellipse::reflex(Line axis) {
  F1.reflex(axis);
  F2.reflex(axis);
}

void Ellipse::scale(Point center, double coefficient) {
  D *= coefficient;
  F1.scale(center, coefficient);
  F2.scale(center, coefficient);
}

std::pair<Point, Point> Ellipse::focuses() const {
  return std::make_pair(F1, F2);
}

double Ellipse::C() const { return F1.distance(F2) / 2; }
double Ellipse::A() const { return D / 2; }

double Ellipse::B() const { return sqrt(A() * A() - C() * C()); }

double Ellipse::eccentricity() const { return C() / A(); }

Point Ellipse::center() const {
  Point center;
  center.x = (F1.x + F2.x) / 2;
  center.y = (F2.y + F1.y) / 2;
  return center;
}

double Ellipse::area() const { return PI * A() * B(); }

double Ellipse::perimeter() const {
  double A1 = A();
  double B1 = B();
  return PI * (3 * A1 + 3 * B1 - sqrt((3 * A1 + B1) * (A1 + 3 * B1)));
}

bool Ellipse::isCongruentTo(const Shape& another) const {
  if (dynamic_cast<const Ellipse*>(&another) == nullptr) return false;
  Ellipse another_ellipse = dynamic_cast<const Ellipse&>(another);
  return eccentricity() == another_ellipse.eccentricity() &&
         D == another_ellipse.D;
}

bool Ellipse::isSimilarTo(const Shape& another) const {
  if (dynamic_cast<const Ellipse*>(&another) == nullptr) return false;
  Ellipse another_ellipse = dynamic_cast<const Ellipse&>(another);
  return eccentricity() == another_ellipse.eccentricity();
}

std::pair<Line, Line> Ellipse::directrices() const {
  Vector vect = static_cast<Vector>(F2) - static_cast<Vector>(center());
  vect *= A() / eccentricity() / vect.length();
  Point D2 = static_cast<Point>(static_cast<Vector>(center()) + vect);
  vect = vect;
  Point D1 = static_cast<Point>(static_cast<Vector>(center()) - vect);
  Line second(Line(F1, F2).getNormalVector(), D2);
  Line first(Line(F1, F2).getNormalVector(), D1);
  return std::make_pair(first, second);
}

Circle::Circle(Point center, double radius)
    : Ellipse(center, center, radius * 2) {}
Circle::Circle() : Ellipse() {}
double Circle::radius() const { return D / 2; }

Rectangle::Rectangle() : Polygon() {}
Rectangle::Rectangle(std::initializer_list<Point> init) : Polygon(init) {}
Rectangle::Rectangle(Point first, Point second, double relation) {
  if (relation < 1) relation = 1 / relation;
  Vector vec = static_cast<Vector>(second) - static_cast<Vector>(first);
  vec.rotate(atan(relation) * 180 / PI);
  vec *= 1 / sqrt(1 + relation * relation);
  vertices.resize(4);
  vertices[0] = first;
  vertices[1] = static_cast<Point>(static_cast<Vector>(first) + vec);
  vertices[2] = second;
  vec = -vec;
  vertices[3] = static_cast<Point>(static_cast<Vector>(second) + vec);
}

std::pair<Line, Line> Rectangle::diagonals() const {
  return std::make_pair(Line(vertices[0], vertices[2]),
                        Line(vertices[1], vertices[3]));
}

Point Rectangle::center() const {
  Point result = vertices[0];
  result.x = (result.x + vertices[2].x) / 2;
  result.y = (result.y + vertices[2].y) / 2;
  return result;
}

Circle Square::circumscribedCircle() const {
  return Circle(center(), vertices[0].distance(vertices[2]) / 2);
}
Circle Square::inscribedCircle() const {
  return Circle(center(), vertices[0].distance(vertices[1]) / 2);
}
Square::Square(Point first, Point second) {
  Vector center = (static_cast<Vector>(second) - static_cast<Vector>(first));
  Vector help_vec = center;
  help_vec *= 0.5;
  help_vec.rotate(90);
  center *= 0.5;
  center += static_cast<Vector>(first);
  vertices.resize(4);
  center += help_vec;
  vertices[1] = center;
  help_vec *= -2;
  center += help_vec;
  vertices[3] = center;
  vertices[0] = first;
  vertices[2] = second;
}

Triangle::Triangle(Point a, Point b, Point c) : Rectangle({a, b, c}) {}
Point Triangle::centroid() const {
  Point result;
  result.x = (vertices[0].x + vertices[1].x + vertices[2].x) / 3;
  result.y = (vertices[0].y + vertices[1].y + vertices[2].y) / 3;
  return result;
}
Point Triangle::orthocenter() const {
  Line zero = Line(vertices[1], vertices[2]);
  Line first = Line(vertices[0], vertices[2]);
  Line perp_first(first.getNormalVector(), vertices[1]);
  Line perp_zero(zero.getNormalVector(), vertices[0]);
  return line_namespace::intersection(perp_first, perp_zero);
}

Line Triangle::EulerLine() const {
  return Line(orthocenter(), circumscribedCircleCenter());
}

Circle Triangle::ninePointsCircle() const {
  Point first = point_namespace::average_point(vertices[0], vertices[1]);
  Point second = point_namespace::average_point(vertices[0], vertices[2]);
  Point third = point_namespace::average_point(vertices[1], vertices[2]);
  Triangle tr(first, second, third);
  return tr.circumscribedCircle();
}

Circle Triangle::circumscribedCircle() const {
  Point center = circumscribedCircleCenter();
  double radius = center.distance(vertices[0]);
  return Circle(center, radius);
}

Circle Triangle::inscribedCircle() const {
  Point center = inscribedCircleCenter();
  double radius = Line(vertices[0], vertices[1]).distancePoint(center);
  return Circle(center, radius);
}

Point Triangle::circumscribedCircleCenter() const {
  Line zero = Line(vertices[1], vertices[2]);
  Line second = Line(vertices[0], vertices[1]);
  Point zero_center = point_namespace::average_point(vertices[1], vertices[2]);
  Point second_center =
      point_namespace::average_point(vertices[0], vertices[1]);
  Line perp_zero(zero.getNormalVector(), zero_center);
  Line perp_second(second.getNormalVector(), second_center);
  return line_namespace::intersection(perp_second, perp_zero);
}

Vector getBisector(Vector first, Vector second) {
  double k = 1;
  if (vector_namespace::scalarProduct(first, second) < 0) second = -second;
  first *= k / first.length();
  second *= k / second.length();
  return first + second;
}

Point Triangle::inscribedCircleCenter() const {
  Vector first =
      static_cast<Vector>(vertices[1]) - static_cast<Vector>(vertices[0]);
  Vector second =
      static_cast<Vector>(vertices[2]) - static_cast<Vector>(vertices[0]);
  Line zero_bisector(getBisector(first, second), vertices[0]);
  Vector third =
      static_cast<Vector>(vertices[2]) - static_cast<Vector>(vertices[1]);
  first = -first;
  Line first_bisector(getBisector(first, third), vertices[1]);
  return line_namespace::intersection(zero_bisector, first_bisector);
}
