//
// Created by Guad on 06-Aug-17.
//

#pragma once

#define TO_BYTE(c4, c3, c2, c1, c0) (c4 << 4 | c3 << 3 | c2 << 2 | c1 << 1 | c0)

typedef unsigned char byte;

typedef union
{
    struct
    {
        byte reg       : 2;
        byte padding   : 1;
        byte inst_code : 5;
    } parts;
    byte data;
} instruction;

enum
{
    register_B = 0,
    register_C = 1,
    register_D = 2,
    register_E = 3
};