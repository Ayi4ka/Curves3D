#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <random>

// ---------- Helper Struct for 3D points / vectors ----------
struct Vec3 {
    double x, y, z;
};

std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// ---------- Abstract Base Class ----------
class Curve {
public:
    virtual ~Curve() = default;
    virtual Vec3 getPoint(double t) const = 0;
    virtual Vec3 getDerivative(double t) const = 0;
};

// ---------- Circle ----------
class Circle : public Curve {
    double r;
public:
    explicit Circle(double radius) : r(radius > 0 ? radius : 1.0) {}
    double getRadius() const { return r; }

    Vec3 getPoint(double t) const override {
        return { r * cos(t), r * sin(t), 0.0 };
    }

    Vec3 getDerivative(double t) const override {
        return { -r * sin(t), r * cos(t), 0.0 };
    }
};

// ---------- Ellipse ----------
class Ellipse : public Curve {
    double rx, ry;
public:
    Ellipse(double rx_, double ry_)
        : rx(rx_ > 0 ? rx_ : 1.0), ry(ry_ > 0 ? ry_ : 1.0) {
    }

    Vec3 getPoint(double t) const override {
        return { rx * cos(t), ry * sin(t), 0.0 };
    }

    Vec3 getDerivative(double t) const override {
        return { -rx * sin(t), ry * cos(t), 0.0 };
    }
};

// ---------- Helix ----------
class Helix : public Curve {
    double r, step;
public:
    Helix(double radius, double step_)
        : r(radius > 0 ? radius : 1.0), step(step_) {
    }

    Vec3 getPoint(double t) const override {
        return { r * cos(t), r * sin(t), step * t / (2 * M_PI) };
    }

    Vec3 getDerivative(double t) const override {
        return { -r * sin(t), r * cos(t), step / (2 * M_PI) };
    }
};

// ---------- Main ----------
int main() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(1.0, 10.0);
    std::uniform_int_distribution<> typeDist(0, 2);

    std::vector<std::shared_ptr<Curve>> curves;

    // (2) Populate with random curves
    for (int i = 0; i < 10; ++i) {
        int type = typeDist(gen);
        if (type == 0) {
            curves.push_back(std::make_shared<Circle>(dist(gen)));
        }
        else if (type == 1) {
            curves.push_back(std::make_shared<Ellipse>(dist(gen), dist(gen)));
        }
        else {
            curves.push_back(std::make_shared<Helix>(dist(gen), dist(gen)));
        }
    }

    // (3) Print points and derivatives at t=PI/4
    double t = M_PI / 4.0;
    std::cout << "Curves at t = PI/4:\n";
    for (const auto& c : curves) {
        std::cout << "Point: " << c->getPoint(t)
            << " | Derivative: " << c->getDerivative(t) << "\n";
    }

    // (4) Extract only circles into second container (shared_ptrs!)
    std::vector<std::shared_ptr<Circle>> circles;
    for (const auto& c : curves) {
        if (auto circle = std::dynamic_pointer_cast<Circle>(c)) {
            circles.push_back(circle);
        }
    }

    // (5) Sort by radius
    std::sort(circles.begin(), circles.end(),
        [](const std::shared_ptr<Circle>& a, const std::shared_ptr<Circle>& b) {
            return a->getRadius() < b->getRadius();
        });

    std::cout << "\nSorted circles by radius:\n";
    for (const auto& c : circles) {
        std::cout << "Radius: " << c->getRadius() << "\n";
    }

    // (6) Sum of radii
    double sum = 0.0;
    for (const auto& c : circles) sum += c->getRadius();

    std::cout << "\nTotal sum of radii: " << sum << "\n";

    return 0;
}
