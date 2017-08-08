//
// Created by Guad on 06-Aug-17.
//

#pragma once

#include "instruction.h"
#include "vector.h"

void init_tables();
void destroy_tables();
int parse_line(char *line, vector *program);