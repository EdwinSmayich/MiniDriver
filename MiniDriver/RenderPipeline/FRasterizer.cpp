#include "FRasterizer.h"
#include "../Output/FFrameBuffer.h"
#include "../Resources/FScreenVertex.h"
#include "../Resources/FColor.h"


void FRasterizer::DrawTriangle(FFrameBuffer& InFB,
                               const FScreenVertex& A, const FScreenVertex& B, const FScreenVertex& C,
                               const std::function<FColor(glm::vec2, FColor)>& InFragShader) const
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
                float InvWInterp = (WeightA * A.InterpW) + (WeightB * B.InterpW) + (WeightC * C.InterpW);

                float U =
                (WeightA * A.UV.x * A.InterpW +
                    WeightB * B.UV.x * B.InterpW +
                    WeightC * C.UV.x * C.InterpW) / InvWInterp;

                float V =
                (WeightA * A.UV.y * A.InterpW +
                    WeightB * B.UV.y * B.InterpW +
                    WeightC * C.UV.y * C.InterpW) / InvWInterp;

                FColor InterpColor(
                    static_cast<unsigned char>(WeightA * A.Color.R + WeightB * B.Color.R + WeightC * C.Color.R),
                    static_cast<unsigned char>(WeightA * A.Color.G + WeightB * B.Color.G + WeightC * C.Color.G),
                    static_cast<unsigned char>(WeightA * A.Color.B + WeightB * B.Color.B + WeightC * C.Color.B));
                
                FColor FinalColor = InFragShader(glm::vec2(U, V), InterpColor);

                InFB.SetPixel(X, Y, FinalColor);
                InFB.SetDepth(X, Y, Depth);
            }
        }
    }
}

float FRasterizer::EdgeFunction(const glm::vec2& A, const glm::vec2& B, const glm::vec2& P)
{
    return (B.x - A.x) * (P.y - A.y) - (B.y - A.y) * (P.x - A.x);
}
