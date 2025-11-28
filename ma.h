#ifndef MA_H
#define MA_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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
#define slice(T)                                                               \
  struct {                                                                     \
    T *p;                                                                      \
    size_t len;                                                                \
  }
#define slice_from_arr(s, ptr, from, to)                                       \
  do {                                                                         \
    (s).p = (ptr) + (from);                                                    \
    (s).len = (to) - (from);                                                   \
  } while (0)
#define slice_from_vector(s, vector, from, to)                                 \
  slice_from_arr((s), (vector).p, (from), (to))
#define slice_init(s, pointer, length)                                         \
  do {                                                                         \
    (s).p = pointer;                                                           \
    (s).len = length;                                                          \
  } while (0)
#define slice_foreach(i, s)                                                    \
  for (typeof(*(s)->p) *i = (s)->p; i < (s)->p + (s)->len; ++i)
#define read_whole_file_sv(sv, filename)                                       \
  do {                                                                         \
    (sv).p = read_whole_file(filename, &(sv).len);                             \
  } while (0)
#define read_whole_file_sb(sb, filename)                                       \
  do {                                                                         \
    (sb).p = read_whole_file(filename, &(sb).len);                             \
    (sb).cap = (sb).len;                                                       \
  } while (0)
#define sv_print(sv) fwrite(sv.p, sizeof(char), sv.len, stdout)
#define sb_append(sb, cstr)                                                    \
  do {                                                                         \
    char *s = (cstr);                                                          \
    size_t n = strlen(s);                                                      \
    vector_push_many(sb, s, n);                                                \
  } while (0)
#define sv_from_sb(sb) sv_from_parts((sb).p, (sb).len)
typedef vector(char) stringb;
typedef slice(char) stringv;
stringv sv_chop_by_delim(stringv *sv, char delim);
stringv sv_chop_left(stringv *sv, size_t n);
stringv sv_from_parts(char *p, size_t len);
stringv sv_trim_left(stringv sv);
stringv sv_trim_right(stringv sv);
stringv sv_trim(stringv sv);
stringv sv_from_cstr(char *cstr);
stringb sb_from_cstr(char *cstr);
bool sv_eq(stringv a, stringv b);
bool sv_end_with(stringv sv, char *cstr);
bool sv_starts_with(stringv sv, stringv expected_prefix);

#ifndef SIZEOF
#define SIZEOF(n) sizeof(n)/sizeof(n[0])
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef SWAP
#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T t = a;                                                                   \
    a = b;                                                                     \
    b = t;                                                                     \
  } while (0)
#endif
#ifndef CLAMP
#define CLAMP(val, low, high) (MAX((low), MIN((val), (high))))
#endif

#endif /* MA_H */

