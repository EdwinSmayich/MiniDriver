#pragma once
#include <string>
#include <vector>

struct FColor;

class FFrameBuffer
{
public:
    FFrameBuffer(size_t InWidth, size_t InHeight);

    void SetPixel(size_t InX, size_t InY, const FColor& InColor);
    
    void SetDepth(size_t InX, size_t InY, float InDepth);

    void SavePPM(const std::string& InPPMPath) const;

    void Clear(const FColor& InColor);

    size_t GetWidth() const { return Width; }

    size_t GetHeight() const { return Height; }
    
    float GetDepth(size_t InX, size_t InY) const;

private:
    size_t Width;
    size_t Height;
    std::vector<FColor> FrameBuffer;
    std::vector<float> DepthBuffer;
};
