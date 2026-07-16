#pragma once
#include <glm/vec3.hpp>

#include "FColor.h"

struct FVertex
{
    FVertex(const glm::vec3& InPos, const FColor& InColor, glm::vec2 InUV)
        : Position(InPos),
          Color(InColor),
          UV(InUV)
    {
    }

    glm::vec3 Position;
    FColor Color;
    glm::vec2 UV;
};
