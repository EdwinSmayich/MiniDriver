#pragma once
#include <glm/vec2.hpp>
#include "FColor.h"

struct FScreenVertex
{
    FScreenVertex() = default;

    FScreenVertex(float InX, float InY, float InDepth, const FColor& InColor)
        : Position(InX, InY),
          Depth(InDepth),
          Color(InColor)
    {
    }

    glm::vec2 Position;
    float Depth = 0.0f;
    FColor Color;
};
