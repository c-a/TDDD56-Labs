#include <stdbool.h>

static inline void
insertion_sort(int* a, int first, int last) {
  int i;

  for (i = first + 1; i <= last; ++i) {
    int v, j;

    v = a[i];
    j = i;
    for (j = i; j > first && a[j-1] > v; --j) {
      a[j] = a[j-1];
    }
    a[j] = v;
  }
}

static inline void
swap(int* a, int* b) {
  int c;

  c = *a;
  *a = *b;
  *b = c;
}

static inline int
median_of_three(int* a, int first, int last) {

  int mid = first + (last - first) / 2;

  /* Puts smallest vallue first, biggest value in the middle and the
   middle value last */
  if (a[last] < a[first])
    swap(&a[last], &a[first]);
  if (a[mid] < a[first])
    swap(&a[mid], &a[first]);
  if (a[last] < a[mid])
    swap(&a[last], &a[mid]);

  return mid;
}

/* 
 * Assumes that the pivot element is found in a[first+1]
 * and that a[first] is less than or equal to the pivot and
 * that a[last] is greater and equal to the pivot.
 */
static inline int
two_way_partition(int *a, int first, int last) {
  int l, r, pivot;

  l = first+1; r = last;
  pivot = a[first+1];
  while (true) {
    while (a[++l] < pivot);
    while (a[--r] > pivot);

    if (l >= r)
      break;

    swap(&a[l], &a[r]);
  }

  /* Put pivot into place */
  swap(&a[first+1], &a[r]);

  return r;
}

void
quicksort(int* a, int first, int last) {
  int m, p;

  /* Cutoff to insertion sort */
  if (last - first <= 7) {
    insertion_sort(a, first, last);
    return;
  }

  /* Take the median of three as the pivot element */
  m = median_of_three(a, first, last);
  /* Put median next first */
  swap(&a[m], &a[first+1]);

  /* Partition, returns position of pivot */
  p = two_way_partition(a, first, last);

  /* Sort a[first..p-1] and a[p+1..last] */
  quicksort(a, first, p-1);
  quicksort(a, p+1, last);
}
