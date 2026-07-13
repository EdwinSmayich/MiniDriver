#include <array>
#include <filesystem>
#include <string>
#include <format>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Output/FFrameBuffer.h"
#include "RenderPipeline/FRasterizer.h"
#include "Resources/FColor.h"
#include "Resources/FScreenVertex.h"
#include "Resources/FVertex.h"

constexpr size_t Width = 256;
constexpr size_t Height = 256;
constexpr glm::vec3 CAMERA_POSITION(0.0f, 0.0f, 3.0f);;

struct FFace
{
    int A, B, C, D;
    FColor Color;
};

FScreenVertex TransformVertex(const FVertex& InVertex, const glm::mat4& InMVP)
{
    glm::vec4 Clip = InMVP * glm::vec4(InVertex.Position, 1.0f);

    float ProjX = Clip.x / Clip.w;
    float ProjY = Clip.y / Clip.w;
    float Depth = Clip.z / Clip.w;

    float ScreenX = (ProjX * 0.5f + 0.5f) * Width;
    float ScreenY = (1.0f - (ProjY * 0.5f + 0.5f)) * Height;

    return FScreenVertex(ScreenX, ScreenY, Depth, InVertex.Color);
}

int main()
{
    FColor BlackColor(0, 0, 0);
    FColor WhiteColor(255, 255, 255);
    FColor RedColor(255, 0, 0);
    FColor GreenColor(0, 255, 0);
    FColor BlueColor(0, 0, 255);
    FColor YellowColor(255, 255, 0);
    FColor CyanColor(0, 255, 255);
    FColor MagentaColor(255, 0, 255);

    glm::vec3 Corners[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, // 0 1 2 3
        {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}, // 4 5 6 7
    };

    FFace Faces[6] = {
        {0, 1, 2, 3, RedColor}, {4, 5, 6, 7, GreenColor},
        {0, 3, 7, 4, BlueColor}, {1, 5, 6, 2, YellowColor},
        {0, 4, 5, 1, CyanColor}, {3, 2, 6, 7, MagentaColor},
    };

    std::array Vertices =
    {
        FVertex(Corners[0], RedColor),
        FVertex(Corners[1], GreenColor),
        FVertex(Corners[2], BlueColor),
        FVertex(Corners[3], YellowColor),

        FVertex(Corners[4], CyanColor),
        FVertex(Corners[5], MagentaColor),
        FVertex(Corners[6], WhiteColor),
        FVertex(Corners[7], BlackColor)
    };

    FFrameBuffer FrameBuffer(Width, Height);
    FRasterizer Rasterizer;

    float Aspect = static_cast<float>(Width) / static_cast<float>(Height);

    glm::mat4 View = glm::translate(glm::mat4(1.0f), -CAMERA_POSITION);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 100.0f);

    std::filesystem::remove_all("Output/Frames");
    std::filesystem::create_directories("Output/Frames");

    constexpr int FrameCount = 36;
    for (int Frame = 0; Frame < FrameCount; ++Frame)
    {
        FrameBuffer.Clear(WhiteColor);

        float Angle = static_cast<float>(Frame) * (360.0f / FrameCount);

        glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 MVP = Projection * View * Model;

        for (const FFace& Face : Faces)
        {
            FVertex V0 = Vertices[Face.A];
            FVertex V1 = Vertices[Face.B];
            FVertex V2 = Vertices[Face.C];
            FVertex V3 = Vertices[Face.D];

            FScreenVertex ScreenV0 = TransformVertex(V0, MVP);
            FScreenVertex ScreenV1 = TransformVertex(V1, MVP);
            FScreenVertex ScreenV2 = TransformVertex(V2, MVP);
            FScreenVertex ScreenV3 = TransformVertex(V3, MVP);

            Rasterizer.DrawTriangle(FrameBuffer, ScreenV0, ScreenV1, ScreenV2);
            Rasterizer.DrawTriangle(FrameBuffer, ScreenV0, ScreenV2, ScreenV3);
        }

        std::string FramesPath = std::format("Output/Frames/frame_{:03}.ppm", Frame);
        FrameBuffer.SavePPM(FramesPath);
    }

    std::system("py make_gif.py");

    return 0;
}

// // Red facet
// FVertex VForward0(glm::vec3(-1.0f, -1.0f, 1.0f), RedColor);
// FVertex VForward1(glm::vec3(1.0f, -1.0f, 1.0f), RedColor);
// FVertex VForward2(glm::vec3(1.0f, 1.0f, 1.0f), RedColor);
// FVertex VForward3(glm::vec3(-1.0f, 1.0f, 1.0f), RedColor);
//
// FScreenVertex ScreenVForward0 = TransformVertex(VForward0, MVP);
// FScreenVertex ScreenVForward1 = TransformVertex(VForward1, MVP);
// FScreenVertex ScreenVForward2 = TransformVertex(VForward2, MVP);
// FScreenVertex ScreenVForward3 = TransformVertex(VForward3, MVP);
//
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVForward0, ScreenVForward1, ScreenVForward2);
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVForward0, ScreenVForward2, ScreenVForward3);
//
// // Green facet
// FVertex VRight0(glm::vec3(-1.0f, -1.0f, -1.0f), GreenColor);
// FVertex VRight1(glm::vec3(1.0f, -1.0f, -1.0f), GreenColor);
// FVertex VRight2(glm::vec3(1.0f, 1.0f, -1.0f), GreenColor);
// FVertex VRight3(glm::vec3(-1.0f, 1.0f, -1.0f), GreenColor);
//
// FScreenVertex ScreenVRight0 = TransformVertex(VRight0, MVP);
// FScreenVertex ScreenVRight1 = TransformVertex(VRight1, MVP);
// FScreenVertex ScreenVRight2 = TransformVertex(VRight2, MVP);
// FScreenVertex ScreenVRight3 = TransformVertex(VRight3, MVP);
//
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVRight0, ScreenVRight1, ScreenVRight2);
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVRight0, ScreenVRight2, ScreenVRight3);
//
// // Blue facet
// FVertex VUp0(glm::vec3(-1.0f, 1.0f, 1.0f), BlueColor);
// FVertex VUp1(glm::vec3(1.0f, 1.0f, 1.0f), BlueColor);
// FVertex VUp2(glm::vec3(1.0f, 1.0f, -1.0f), BlueColor);
// FVertex VUp3(glm::vec3(-1.0f, 1.0f, -1.0f), BlueColor);
//
// FScreenVertex ScreenVUp0 = TransformVertex(VUp0, MVP);
// FScreenVertex ScreenVUp1 = TransformVertex(VUp1, MVP);
// FScreenVertex ScreenVUp2 = TransformVertex(VUp2, MVP);
// FScreenVertex ScreenVUp3 = TransformVertex(VUp3, MVP);
//
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVUp0, ScreenVUp1, ScreenVUp2);
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVUp0, ScreenVUp2, ScreenVUp3);
//
// // Magenta facet
// FVertex VMagenta0(glm::vec3(-1.0f, -1.0f, 1.0f), MagentaColor);
// FVertex VMagenta1(glm::vec3(1.0f, -1.0f, 1.0f), MagentaColor);
// FVertex VMagenta2(glm::vec3(1.0f, -1.0f, -1.0f), MagentaColor);
// FVertex VMagenta3(glm::vec3(-1.0f, -1.0f, -1.0f), MagentaColor);
//
// FScreenVertex ScreenVMagenta0 = TransformVertex(VMagenta0, MVP);
// FScreenVertex ScreenVMagenta1 = TransformVertex(VMagenta1, MVP);
// FScreenVertex ScreenVMagenta2 = TransformVertex(VMagenta2, MVP);
// FScreenVertex ScreenVMagenta3 = TransformVertex(VMagenta3, MVP);
//
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVMagenta0, ScreenVMagenta1, ScreenVMagenta2);
// Rasterizer.DrawTriangle(FrameBuffer, ScreenVMagenta0, ScreenVMagenta2, ScreenVMagenta3);
// std::string Path = std::format("Frames/Frame_{:03}.ppm", Frame);
