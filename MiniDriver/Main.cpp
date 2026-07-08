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

FScreenVertex TransformVertex(const FVertex& InVertex, const glm::mat4& InModel)
{
    glm::vec4 VertexLocal(InVertex.Position, 1.0f);
    glm::vec4 Transformed = InModel * VertexLocal;

    return FScreenVertex(Transformed.x, Transformed.y, InVertex.Color);
}

int main()
{
    FColor WhiteColor(255, 255, 255);
    FColor RedColor(255, 0, 0);
    FColor GreenColor(0, 255, 0);
    FColor BlueColor(0, 0, 255);

    FVertex A(glm::vec3(122.0f, 30.0f, 0.0f), RedColor);
    FVertex B(glm::vec3(205.0f, 220.0f, 0.0f), GreenColor);
    FVertex C(glm::vec3(50.0f, 220.0f, 0.0f), BlueColor);

    FFrameBuffer FrameBuffer(Width, Height);
    FrameBuffer.Clear(WhiteColor);

    glm::mat4 Model = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, -20.f, 0.0f));
    
    FScreenVertex ScreenVertexA = TransformVertex(A, Model);
    FScreenVertex ScreenVertexB = TransformVertex(B, Model);
    FScreenVertex ScreenVertexC = TransformVertex(C, Model);
    
    FRasterizer Rasterizer;
    Rasterizer.DrawTriangle(FrameBuffer, ScreenVertexA, ScreenVertexB, ScreenVertexC);

    FrameBuffer.SavePPM("Output/FrameBuffer.ppm");

    return 0;
}
