#pragma once
#include <glm/vec2.hpp>


class FFrameBuffer;
struct FColor;
struct FScreenVertex;

class FRasterizer
{
public:
    void DrawTriangle(FFrameBuffer& InFB, const FScreenVertex& A, const FScreenVertex& B, const FScreenVertex& C) const;

private:
    static float EdgeFunction(const glm::vec2& A, const glm::vec2& B, const glm::vec2& P);
    static FColor SampleTexture(float InU, float InV);
};