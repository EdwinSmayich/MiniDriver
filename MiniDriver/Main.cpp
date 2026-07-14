#include <array>
#include <filesystem>
#include <string>
#include <format>
#include <iostream>

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
    glm::vec3 Normal;
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
    FColor GreyColor(80, 80, 80);
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
        {0, 1, 2, 3, RedColor, glm::vec3(0.0f, 0.0f, -1.0f)},
        {4, 5, 6, 7, GreenColor, glm::vec3(0.0f, 0.0f, 1.0f)},
        {0, 3, 7, 4, BlueColor, glm::vec3(-1.0f, 0.0f, 0.0f)},
        {1, 5, 6, 2, YellowColor, glm::vec3(1.0f, 0.0f, 0.0f)},
        {0, 4, 5, 1, CyanColor, glm::vec3(0.0f, -1.0f, 0.0f)},
        {3, 2, 6, 7, MagentaColor, glm::vec3(0.0f, 1.0f, 0.0f)},
    };

    // std::array Vertices =
    // {
    //     FVertex(Corners[0], RedColor),
    //     FVertex(Corners[1], GreenColor),
    //     FVertex(Corners[2], BlueColor),
    //     FVertex(Corners[3], YellowColor),
    //
    //     FVertex(Corners[4], CyanColor),
    //     FVertex(Corners[5], MagentaColor),
    //     FVertex(Corners[6], WhiteColor),
    //     FVertex(Corners[7], BlackColor)
    // };

    FFrameBuffer FrameBuffer(Width, Height);
    FRasterizer Rasterizer;

    float Aspect = static_cast<float>(Width) / static_cast<float>(Height);

    glm::mat4 View = glm::translate(glm::mat4(1.0f), -CAMERA_POSITION);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 100.0f);

    std::filesystem::remove_all("Output/Frames");
    std::filesystem::create_directories("Output/Frames");

    glm::vec3 LightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 3.0f));

    constexpr int FrameCount = 36;
    for (int Frame = 0; Frame < FrameCount; ++Frame)
    {
        FrameBuffer.Clear(GreyColor);

        float Angle = static_cast<float>(Frame) * (360.0f / FrameCount);

        glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(40.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 MVP = Projection * View * Model;

        for (const FFace& Face : Faces)
        {
            glm::vec3 WorldNormal = glm::normalize(glm::mat3(Model) * Face.Normal);
            float Brightness = std::max(0.0f, glm::dot(WorldNormal, LightDir));

            if (Frame == 0)
            {
                std::cout << "Edge brightness: " << Brightness << "\n";
            }

            FColor Lit(
                static_cast<unsigned char>(Face.Color.R * Brightness),
                static_cast<unsigned char>(Face.Color.G * Brightness),
                static_cast<unsigned char>(Face.Color.B * Brightness));

            FVertex V0(Corners[Face.A], Lit);
            FVertex V1(Corners[Face.B], Lit);
            FVertex V2(Corners[Face.C], Lit);
            FVertex V3(Corners[Face.D], Lit);

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
