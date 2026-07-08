#pragma once
#include <string>
#include <vector>

struct FColor;

class FFrameBuffer
{
public:
    FFrameBuffer(size_t InWidth, size_t InHeight);

    void SetPixel(size_t InX, size_t InY, const FColor& InColor);

    void SavePPM(const std::string& InPPMPath) const;

    void Clear(const FColor& InColor);

    size_t GetWidth() const { return Width; }

    size_t GetHeight() const { return Height; }

private:
    size_t Width;
    size_t Height;
    std::vector<FColor> FrameBuffer;
};
