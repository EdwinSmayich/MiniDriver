#include <filesystem>
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
constexpr glm::vec3 CAMERA_POSITION(0.0f, 0.0f, 2.0f);;

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
    float InterpW = 1.0f / Clip.w;

    float ScreenX = (ProjX * 0.5f + 0.5f) * Width;
    float ScreenY = (1.0f - (ProjY * 0.5f + 0.5f)) * Height;

    return FScreenVertex(ScreenX, ScreenY, Depth, InVertex.Color, InVertex.UV, InterpW);
}

FColor SampleTexture(const glm::vec2& InUV)
{
    int Cell = (static_cast<int>(InUV.x * 8.0f) + static_cast<int>(InUV.y * 8.0f)) % 2;

    if (Cell == 0)
    {
        return FColor(230, 230, 230);
    }

    return FColor(30, 30, 30);
}

auto FragmentShader = [](const glm::vec2& InUV, const FColor& InLight) -> FColor
{
    FColor Texture = SampleTexture(InUV);

    return FColor((Texture.R * InLight.R) / 255,
                  (Texture.G * InLight.G) / 255,
                  (Texture.B * InLight.B) / 255);
};

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

    FFrameBuffer FrameBuffer(Width, Height);
    FRasterizer Rasterizer;

    float Aspect = static_cast<float>(Width) / static_cast<float>(Height);

    glm::mat4 View = glm::translate(glm::mat4(1.0f), -CAMERA_POSITION);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 100.0f);

    FrameBuffer.Clear(GreyColor);

    std::filesystem::remove_all("Output/Frames");
    std::filesystem::create_directories("Output/Frames");

    glm::vec3 LightDir = glm::normalize(glm::vec3(0.0f, 1.0f, 3.0f));

    constexpr int FrameCount = 36;
    for (int Frame = 0; Frame < FrameCount; ++Frame)
    {
        FrameBuffer.Clear(GreyColor);

        float Angle = static_cast<float>(Frame) * (360.0f / FrameCount);

        glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(0.0f, 1.0f, 0.0f));
        Model = glm::rotate(Model, glm::radians(Angle), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 MVP = Projection * View * Model;

        for (const FFace& Face : Faces)
        {
            glm::vec3 WorldNormal = glm::normalize(glm::mat3(Model) * Face.Normal);
            float Brightness = std::max(0.0f, glm::dot(WorldNormal, LightDir));

            FColor Light(
                static_cast<unsigned char>(255 * Brightness),
                static_cast<unsigned char>(255 * Brightness),
                static_cast<unsigned char>(255 * Brightness));

            FVertex V0(Corners[Face.A], Light, glm::vec2(0.0f, 0.0f));
            FVertex V1(Corners[Face.B], Light, glm::vec2(1.0f, 0.0f));
            FVertex V2(Corners[Face.C], Light, glm::vec2(1.0f, 1.0f));
            FVertex V3(Corners[Face.D], Light, glm::vec2(0.0f, 1.0f));

            FScreenVertex ScreenV0 = TransformVertex(V0, MVP);
            FScreenVertex ScreenV1 = TransformVertex(V1, MVP);
            FScreenVertex ScreenV2 = TransformVertex(V2, MVP);
            FScreenVertex ScreenV3 = TransformVertex(V3, MVP);

            Rasterizer.DrawTriangle(FrameBuffer, ScreenV0, ScreenV1, ScreenV2, FragmentShader);
            Rasterizer.DrawTriangle(FrameBuffer, ScreenV0, ScreenV2, ScreenV3, FragmentShader);
        }

        std::string FramesPath = std::format("Output/Frames/frame_{:03}.ppm", Frame);
        FrameBuffer.SavePPM(FramesPath);
    }

    std::system("py make_gif.py");

    return 0;
}
