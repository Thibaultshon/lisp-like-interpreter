#ifndef UTIL_H
#define UTIL_H

#define CAR(node) ((node)->cons.car)
#define CDR(node) ((node)->cons.cdr)

void error(char* message);

#endif
