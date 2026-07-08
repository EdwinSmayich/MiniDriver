#include "FFrameBuffer.h"

#include <fstream>
#include <iostream>
#include "../Resources/FColor.h"


FFrameBuffer::FFrameBuffer(size_t InWidth, size_t InHeight)
    : Width(InWidth),
      Height(InHeight),
      FrameBuffer(Width * Height)
{
}

void FFrameBuffer::SetPixel(size_t InX, size_t InY, const FColor& InColor)
{
    if (InX < Width && InY < Height)
    {
        FrameBuffer[InY * Width + InX] = InColor;
    }
}

void FFrameBuffer::SavePPM(const std::string& InPPMPath) const
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

void FFrameBuffer::Clear(const FColor& InColor)
{
    for (auto& Color : FrameBuffer)
    {
        Color = InColor;
    }
}
