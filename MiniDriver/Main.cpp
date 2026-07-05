#include <cstdint>
#include <vector>
#include <fstream>

constexpr uint32_t Width = 256;
constexpr uint32_t Height = 256;


struct FColor
{
    FColor() = default;

    FColor(unsigned char InR, unsigned char InG, unsigned char InB)
    {
        R = InR;
        G = InG;
        B = InB;
    }

    unsigned char R;
    unsigned char G;
    unsigned char B;
};

struct FVector2
{
    FVector2(float InX, float InY)
    {
        X = InX;
        Y = InY;
    }

    float X;
    float Y;
};

float EdgeFunction(FVector2 A, FVector2 B, FVector2 P)
{
    return (B.X - A.X) * (P.Y - A.Y) - (B.Y - A.Y) * (P.X - A.X);
}

int main()
{
    std::vector<FColor> FrameBuffer;
    FrameBuffer.resize(Width * Height);
    std::ofstream Out("Output/FrameBuffer.ppm");

    FVector2 A(122, 30);
    FVector2 B(205, 220);
    FVector2 C(50, 220);

    for (int y = 0; y < Height; ++y)
    {
        for (int x = 0; x < Width; ++x)
        {
            const FVector2 PixelPos(static_cast<float>(x),
                                    static_cast<float>(y));

            float EdgeBC = EdgeFunction(B, C, PixelPos);
            float EdgeCA = EdgeFunction(C, A, PixelPos);
            float EdgeAB = EdgeFunction(A, B, PixelPos);

            bool bInInside = (EdgeBC >= 0 && EdgeCA >= 0 && EdgeAB >= 0) ||
                (EdgeBC <= 0 && EdgeCA <= 0 && EdgeAB <= 0);

            if (!bInInside)
            {
                continue;
            }

            float TotalArea = EdgeBC + EdgeCA + EdgeAB;
            float WeightA = EdgeBC / TotalArea;
            float WeightB = EdgeCA / TotalArea;
            float WeightC = EdgeAB / TotalArea;

            FColor ColorA(255, 0, 0);
            FColor ColorB(0, 255, 0);
            FColor ColorC(0, 0, 255);

            float InterpR = WeightA * ColorA.R + WeightB * ColorB.R + WeightC * ColorC.R;
            float InterpG = WeightA * ColorA.G + WeightB * ColorB.G + WeightC * ColorC.G;
            float InterpB = WeightA * ColorA.B + WeightB * ColorB.B + WeightC * ColorC.B;

            FColor FinalColor(static_cast<unsigned char>(InterpR),
                              static_cast<unsigned char>(InterpG),
                              static_cast<unsigned char>(InterpB));

            FrameBuffer[y * Width + x] = FinalColor;
        }
    }

    if (Out.is_open())
    {
        Out << "P3\n";
        Out << Width << " " << Height << "\n";
        Out << "255\n";

        for (const auto& Color : FrameBuffer)
        {
            Out << static_cast<unsigned int>(Color.R) << " "
                << static_cast<unsigned int>(Color.G) << " "
                << static_cast<unsigned int>(Color.B) << "\n";
        }
    }

    Out.close();

    return 0;
}
