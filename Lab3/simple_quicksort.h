#include "array.h"

#ifndef DEBUG
#define NDEBUG
#endif

#ifndef SIMPLE_SORT
#define SIMPLE_SORT

int simple_quicksort_ascending(struct array *);
int simple_quicksort_descending(struct array *);

#endif
