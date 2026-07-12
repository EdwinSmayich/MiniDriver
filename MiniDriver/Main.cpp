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
constexpr glm::vec3 CAMERA_POSITION(0.0f, 0.0f, 2.0f);


FScreenVertex TransformVertex(const FVertex& InVertex)
{
    float RelativeX = InVertex.Position.x - CAMERA_POSITION.x;
    float RelativeY = InVertex.Position.y - CAMERA_POSITION.y;
    float RelativeZ = InVertex.Position.z - CAMERA_POSITION.z;

    float Distance = -RelativeZ;
    float ProjX = RelativeX / Distance;
    float ProjY = RelativeY / Distance;

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
    FrameBuffer.Clear(WhiteColor);

    FVertex VertexA(glm::vec3(0.0f, 1.0f, 0.0f), RedColor);
    FVertex VertexB(glm::vec3(1.0f, -1.0f, 0.0f), GreenColor);
    FVertex VertexC(glm::vec3(-1.0f, -1.0f, 0.0f), BlueColor);

    FScreenVertex ScreenVertexA = TransformVertex(VertexA);
    FScreenVertex ScreenVertexB = TransformVertex(VertexB);
    FScreenVertex ScreenVertexC = TransformVertex(VertexC);

    FRasterizer Rasterizer;
    Rasterizer.DrawTriangle(FrameBuffer, ScreenVertexA, ScreenVertexB, ScreenVertexC);

    FrameBuffer.SavePPM("Output/FrameBuffer.ppm");

    return 0;
}

// FScreenVertex TransformVertex(const FVertex& InVertex, const glm::mat4& InMVP)
// {
//     glm::vec4 VertexLocal(InVertex.Position, 1.0f);
//     glm::vec4 Clip = InMVP * VertexLocal;
//     glm::vec3 NDC = glm::vec3(Clip) / Clip.w;
//
//     float ScreenX = (NDC.x * 0.5f + 0.5f) * Width;
//     float ScreenY = (1.0f - (NDC.y * 0.5f + 0.5f)) * Height;
//
//     std::cout << "NDC=( " << NDC.x << ", " << NDC.y << ")" << "\n"
//         << "Screen=( " << ScreenX << ", " << ScreenY << ")" << "\n\n";
//
//     return FScreenVertex(ScreenX, ScreenY, InVertex.Color);
// }