#ifndef ENV_H
#define ENV_H

#include "uthash.h"

struct Hash_Node {
  char *name;
  int val;
  UT_hash_handle hh;
};

extern struct Hash_Node *g_env;

void addIdentifier(struct Hash_Node *identifier);

struct Hash_Node *findIdentifier(char* name);

void deleteIdentifier(char* name);

void freeEnv();

#endif
