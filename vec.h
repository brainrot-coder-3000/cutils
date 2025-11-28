#include <stdlib.h>

#define vector(T)                                                              \
  struct {                                                                     \
    T *p;                                                                      \
    size_t len;                                                                \
    size_t cap;                                                                \
  }
#define vector_reserve(v, n)                                                   \
  do {                                                                         \
    if ((v)->cap < (n)) {                                                      \
      (v)->cap = (n);                                                          \
      (v)->p = realloc((v)->p, sizeof(*(v)->p) * (v)->cap);                    \
    }                                                                          \
  } while (0)
#define vector_push(v, x)                                                      \
  do {                                                                         \
    if ((v)->len == (v)->cap) {                                                \
      (v)->cap = (v)->cap ? (v)->cap * 2 : 4;                                  \
      (v)->p = realloc((v)->p, sizeof(*(v)->p) * (v)->cap);                    \
    }                                                                          \
    (v)->p[(v)->len++] = (x);                                                  \
  } while (0)
#define vector_push_many(v, data, size)                                        \
  do {                                                                         \
    size_t elem_size = sizeof(*(v).p);                                         \
    size_t new_len = (v).len + size / elem_size;                               \
                                                                               \
    if (new_len > (v).cap) {                                                   \
      (v).cap = new_len * 2;                                                   \
      (v).p = realloc((v).p, elem_size * (v).cap);                             \
    }                                                                          \
                                                                               \
    memcpy((v).p + (v).len, data, size);                                       \
    (v).len = new_len;                                                         \
  } while (0)