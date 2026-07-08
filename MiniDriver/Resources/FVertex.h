#pragma once
#include <glm/vec3.hpp>

#include "FColor.h"

struct FVertex
{
    FVertex(const glm::vec3& InPos, const FColor& InColor)
        : Position(InPos),
          Color(InColor)
    {
    }

    glm::vec3 Position;
    FColor Color;
};
