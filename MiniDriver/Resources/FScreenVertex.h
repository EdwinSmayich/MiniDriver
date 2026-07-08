#pragma once
#include <glm/vec2.hpp>
#include "FColor.h"

struct FScreenVertex
{
    FScreenVertex() = default;

    FScreenVertex(float InX, float InY, const FColor& InColor)
        : Position(InX, InY),
          Color(InColor)
    {
    }

    glm::vec2 Position;
    FColor Color;
};
