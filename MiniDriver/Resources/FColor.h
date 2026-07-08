#pragma once

struct FColor
{
    FColor() = default;

    FColor(unsigned char InR, unsigned char InG, unsigned char InB)
        : R(InR), G(InG), B(InB)
    {
    }

    unsigned char R = 0;
    unsigned char G = 0;
    unsigned char B = 0;
};