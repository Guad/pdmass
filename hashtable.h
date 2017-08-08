//
// Created by Guad on 08-Aug-17.
//

#pragma once

#define MAX_CELLS 512

typedef struct table_chain_t
{
    unsigned long int hash;
    int value;
    struct table_chain_t *next;
} table_chain;

typedef struct
{
    table_chain *chain[MAX_CELLS];
} hashtable;

hashtable* create_table();
void table_set(hashtable *table, char *key, int value);
int table_get(hashtable *table, char *key);
int table_exists(hashtable *table, char *key);
void destroy_table(hashtable *table);