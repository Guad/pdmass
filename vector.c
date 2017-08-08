//
// Created by Guad on 06-Aug-17.
//

#include <afxres.h>
#include "vector.h"

vector* create_vector()
{
    vector *result = malloc(sizeof(vector));
    result->capacity = 0;
    result->length = 0;
    result->data = NULL;

    return result;
}

void destroy_vector(vector *vector)
{
    free(vector->data);
    free(vector);
}

void push_item(vector *vect, byte data)
{
    if (vect->length >= vect->capacity)
    {
        vect->capacity += INCREASE_STEP;
        vect->data = realloc(vect->data, vect->capacity);
    }

    vect->data[vect->length] = data;
    vect->length++;
}
