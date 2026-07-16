#pragma once
#include <glm/vec2.hpp>
#include "FColor.h"

struct FScreenVertex
{
    FScreenVertex() = default;

    FScreenVertex(float InX, float InY, float InDepth, const FColor& InColor, const glm::vec2& InUV, float InInterpW)
        : Position(InX, InY),
          Depth(InDepth),
          Color(InColor),
          UV(InUV),
          InterpW(InInterpW)
    {
    }

    glm::vec2 Position;
    float Depth = 0.0f;
    FColor Color;
    glm::vec2 UV;
    float InterpW = 1.0f;
};
