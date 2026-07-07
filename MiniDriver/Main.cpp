#include <cstddef>
#include <vector>
#include <fstream>
#include <iostream>

constexpr size_t Width = 256;
constexpr size_t Height = 256;


struct FColor
{
    FColor() = default;

    FColor(unsigned char InR, unsigned char InG, unsigned char InB)
        : R(InR), G(InG), B(InB)
    {
    }

    unsigned char R = 0;
    unsigned char G = 0;
    unsigned char B = 0;
};

struct FVector2
{
    FVector2() = default;

    FVector2(float InX, float InY)
        : X(InX),
          Y(InY)
    {
    }

    float X = 0.0f;
    float Y = 0.0f;
};

struct FVertex
{
    FVertex() = default;

    FVertex(float InX, float InY, const FColor& InColor)
        : Position(InX, InY),
          Color(InColor)
    {
    }

    FVector2 Position;
    FColor Color;
};

class FFrameBuffer
{
public:
    FFrameBuffer(size_t InWidth, size_t InHeight)
        : Width(InWidth),
          Height(InHeight),
          FrameBuffer(Width * Height)
    {
    }

    void SetPixel(size_t InX, size_t InY, const FColor& InColor)
    {
        if (InX < Width && InY < Height)
        {
            FrameBuffer[InY * Width + InX] = InColor;
        }
    }

    void SavePPM(const std::string& InPPMPath) const
    {
        std::ofstream Out(InPPMPath);

        if (!Out.is_open())
        {
            std::cerr << "Failed to open file: " << InPPMPath << '\n';
            return;
        }
        
        Out << "P3\n";
        Out << Width << " " << Height << "\n";
        Out << "255\n";

        for (const auto& Color : FrameBuffer)
        {
            Out << static_cast<unsigned int>(Color.R) << " "
                << static_cast<unsigned int>(Color.G) << " "
                << static_cast<unsigned int>(Color.B) << "\n";
        }

        Out.close();
    }

    void Clear(const FColor& InColor)
    {
        for (auto& Color : FrameBuffer)
        {
            Color = InColor;
        }
    }

    size_t GetWidth() const
    {
        return Width;
    }

    size_t GetHeight() const
    {
        return Height;
    }

private:
    size_t Width;
    size_t Height;
    std::vector<FColor> FrameBuffer;
};

class FRasterizer
{
public:
    void DrawTriangle(FFrameBuffer& InFB, const FVertex& A, const FVertex& B, const FVertex& C) const
    {
        float Area = EdgeFunction(A.Position, B.Position, C.Position);

        if (Area == 0.0f)
        {
            return;
        }

        for (size_t y = 0; y < InFB.GetHeight(); ++y)
        {
            for (size_t x = 0; x < InFB.GetWidth(); ++x)
            {
                FVector2 PixelPos(static_cast<float>(x) + 0.5f,
                                  static_cast<float>(y) + 0.5f);

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

                float InterpR = WeightA * A.Color.R + WeightB * B.Color.R + WeightC * C.Color.R;
                float InterpG = WeightA * A.Color.G + WeightB * B.Color.G + WeightC * C.Color.G;
                float InterpB = WeightA * A.Color.B + WeightB * B.Color.B + WeightC * C.Color.B;

                FColor TotalColor(static_cast<unsigned char>(InterpR + 0.5f),
                                  static_cast<unsigned char>(InterpG + 0.5f),
                                  static_cast<unsigned char>(InterpB + 0.5f));

                InFB.SetPixel(x, y, TotalColor);
            }
        }
    }

private:
    float EdgeFunction(const FVector2& A, const FVector2& B, const FVector2& P) const
    {
        return (B.X - A.X) * (P.Y - A.Y) - (B.Y - A.Y) * (P.X - A.X);
    }
};

int main()
{
    FColor WhiteColor(255, 255, 255);
    FColor RedColor(255, 0, 0);
    FColor GreenColor(0, 255, 0);
    FColor BlueColor(0, 0, 255);

    FVertex A(122, 30, RedColor);
    FVertex B(205, 220, GreenColor);
    FVertex C(50, 220, BlueColor);

    FFrameBuffer FrameBuffer(Width, Height);
    FrameBuffer.Clear(WhiteColor);

    FRasterizer Rasterizer;
    Rasterizer.DrawTriangle(FrameBuffer, A, B, C);

    FrameBuffer.SavePPM("Output/FrameBuffer.ppm");

    return 0;
}
