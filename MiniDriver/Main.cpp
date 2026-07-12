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
constexpr glm::vec3 CAMERA_POSITION(0.0f, 0.0f, 2.0f);


FScreenVertex TransformVertex(const FVertex& InVertex, const glm::mat4& InMVP)
{
    glm::vec4 Clip = InMVP * glm::vec4(InVertex.Position, 1.0f);
    
    float ProjX = Clip.x / Clip.w;
    float ProjY = Clip.y / Clip.w;

    float ScreenX = (ProjX * 0.5f + 0.5f) * Width;
    float ScreenY = (1.0f - (ProjY * 0.5f + 0.5f)) * Height;
    
    return FScreenVertex(ScreenX, ScreenY, InVertex.Color);
}

int main()
{
    FColor WhiteColor(255, 255, 255);
    FColor RedColor(255, 0, 0);
    FColor GreenColor(0, 255, 0);
    FColor BlueColor(0, 0, 255);

    FFrameBuffer FrameBuffer(Width, Height);
    FRasterizer Rasterizer;

    FVertex VertexA(glm::vec3(0.0f, 1.0f, -1.0f), RedColor);
    FVertex VertexB(glm::vec3(1.0f, -1.0f, -1.0f), GreenColor);
    FVertex VertexC(glm::vec3(-1.0f, -1.0f, -1.0f), BlueColor);
    
    float Aspect = static_cast<float>(Width) / static_cast<float>(Height);
    
    glm::mat4 View = glm::translate(glm::mat4(1.0f), -CAMERA_POSITION);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 100.0f);
    
    std::filesystem::create_directories("Output/Frames");
    
    constexpr int FrameCount = 36;
    for (int Frame = 0; Frame < FrameCount; ++Frame)
    {
        float Angle = static_cast<float>(Frame) * (360.0f / FrameCount);
        
        glm::mat4 Model = glm::rotate(glm::mat4(1.0f), glm::radians(Angle), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 MVP = Projection * View * Model;

        FrameBuffer.Clear(WhiteColor);
        
        FScreenVertex ScreenVertexA = TransformVertex(VertexA, MVP);
        FScreenVertex ScreenVertexB = TransformVertex(VertexB, MVP);
        FScreenVertex ScreenVertexC = TransformVertex(VertexC, MVP);
        
        Rasterizer.DrawTriangle(FrameBuffer, ScreenVertexA, ScreenVertexB, ScreenVertexC);

        std::string FramesPath = std::format("Output/Frames/frame_{:03}.ppm", Frame);
        FrameBuffer.SavePPM(FramesPath);
    }

    return 0;
}