// BoundingBox.h

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <glm/glm.hpp>
#include <iostream>

class BoundingBox {
public:
    // Public members, including min and max
    glm::vec3 min;
    glm::vec3 max;

    // Constructors
    BoundingBox calculateBoundingBox() const;

    BoundingBox();

    // Function to calculate and print the dimensions of the bounding box
    void calculateDimensions() const;

};

#endif // BOUNDING_BOX_H
