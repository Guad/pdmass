//
// Created by Guad on 06-Aug-17.
// A vector of bytes.
//
#pragma once
#include "instruction.h"
#define INCREASE_STEP 64

typedef struct
{
    int length;
    int capacity;
    byte *data;
} vector;

vector* create_vector();
void destroy_vector(vector *vector);
void push_item(vector *vector, byte data);