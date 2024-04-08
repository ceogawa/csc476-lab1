#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

class BoundingBox {
public:
    vec3 min;
    vec3 max;

    // Constructor
    BoundingBox(const vec3& min, const vec3& max) : min(min), max(max) {}

    // Function to calculate the dimensions of the bounding box
    void calculateDimensions() const {
        vec3 dimensions = max - min;
    }
};