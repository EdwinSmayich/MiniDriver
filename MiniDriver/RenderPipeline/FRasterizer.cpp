#include "FRasterizer.h"
#include <cmath>
#include "../Output/FFrameBuffer.h"
#include "../Resources/FScreenVertex.h"
#include "../Resources/FColor.h"


void FRasterizer::DrawTriangle(FFrameBuffer& InFB, const FScreenVertex& A, const FScreenVertex& B, const FScreenVertex& C) const
{
    float Area = EdgeFunction(A.Position, B.Position, C.Position);

    if (Area == 0.0f)
    {
        return;
    }

    for (size_t Y = 0; Y < InFB.GetHeight(); ++Y)
    {
        for (size_t X = 0; X < InFB.GetWidth(); ++X)
        {
            glm::vec2 PixelPos(static_cast<float>(X) + 0.5f,
                               static_cast<float>(Y) + 0.5f);

            float Edge0 = EdgeFunction(B.Position, C.Position, PixelPos);
            float Edge1 = EdgeFunction(C.Position, A.Position, PixelPos);
            float Edge2 = EdgeFunction(A.Position, B.Position, PixelPos);

            bool bIsInside =
                (Edge0 >= 0 && Edge1 >= 0 && Edge2 >= 0) ||
                (Edge0 <= 0 && Edge1 <= 0 && Edge2 <= 0);

            if (!bIsInside)
            {
                continue;
            }

            float WeightA = Edge0 / Area;
            float WeightB = Edge1 / Area;
            float WeightC = Edge2 / Area;

            float Depth = (WeightA * A.Depth) + (WeightB * B.Depth) + (WeightC * C.Depth);

            if (Depth < InFB.GetDepth(X, Y))
            {
                float InterpR =
                    WeightA * static_cast<float>(A.Color.R) +
                    WeightB * static_cast<float>(B.Color.R) +
                    WeightC * static_cast<float>(C.Color.R);

                float InterpG =
                    WeightA * static_cast<float>(A.Color.G) +
                    WeightB * static_cast<float>(B.Color.G) +
                    WeightC * static_cast<float>(C.Color.G);

                float InterpB =
                    WeightA * static_cast<float>(A.Color.B) +
                    WeightB * static_cast<float>(B.Color.B) +
                    WeightC * static_cast<float>(C.Color.B);

                FColor TotalColor(static_cast<unsigned char>(std::lround(InterpR)),
                                  static_cast<unsigned char>(std::lround(InterpG)),
                                  static_cast<unsigned char>(std::lround(InterpB)));

                InFB.SetPixel(X, Y, TotalColor);
                InFB.SetDepth(X, Y, Depth);
            }
        }
    }
}

float FRasterizer::EdgeFunction(const glm::vec2& A, const glm::vec2& B, const glm::vec2& P)
{
    return (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
}
