#include "ma.h"
#include <ctype.h>
#include <stdio.h>

stringv sv_chop_by_delim(stringv *sv, char delim) {
  size_t i = 0;
  while (i < sv->len && sv->p[i] != delim) {
    i += 1;
  }

  stringv result = sv_from_parts(sv->p, i);

  if (i < sv->len) {
    sv->len -= i + 1;
    sv->p += i + 1;
  } else {
    sv->len -= i;
    sv->p += i;
  }

  return result;
}

stringv sv_chop_left(stringv *sv, size_t n) {
  if (n > sv->len) {
    n = sv->len;
  }

  stringv result = sv_from_parts(sv->p, n);

  sv->p += n;
  sv->len -= n;

  return result;
}

stringv sv_from_parts(char *p, size_t len) {
  stringv sv;
  sv.len = len;
  sv.p = p;
  return sv;
}

stringv sv_trim_left(stringv sv) {
  size_t i = 0;
  while (i < sv.len && isspace(sv.p[i])) {
    i += 1;
  }

  return sv_from_parts(sv.p + i, sv.len - i);
}

stringv sv_trim_right(stringv sv) {
  size_t i = 0;
  while (i < sv.len && isspace(sv.p[sv.len - 1 - i])) {
    i += 1;
  }

  return sv_from_parts(sv.p, sv.len - i);
}

stringv sv_trim(stringv sv) { return sv_trim_right(sv_trim_left(sv)); }

stringv sv_from_cstr(char *cstr) { return sv_from_parts(cstr, strlen(cstr)); }

stringb sb_from_cstr(char *cstr) {
    stringb sb;
    sb.len = strlen(cstr);
    sb.cap = sb.len * 2 + 1;
    sb.p = malloc(sb.cap);
    memcpy(sb.p, cstr, sb.len);
    sb.p[sb.len] = '\0';
    return sb;
}

bool sv_eq(stringv a, stringv b) {
  if (a.len != b.len) {
    return false;
  } else {
    return memcmp(a.p, b.p, a.len) == 0;
  }
}

bool sv_end_with(stringv sv, char *cstr) {
  size_t cstr_len = strlen(cstr);
  if (sv.len >= cstr_len) {
    size_t ending_start = sv.len - cstr_len;
    stringv sv_ending = sv_from_parts(sv.p + ending_start, cstr_len);
    return sv_eq(sv_ending, sv_from_cstr(cstr));
  }
  return false;
}

bool sv_starts_with(stringv sv, stringv expected_prefix) {
  if (expected_prefix.len <= sv.len) {
    stringv actual_prefix = sv_from_parts(sv.p, expected_prefix.len);
    return sv_eq(expected_prefix, actual_prefix);
  }

  return false;
}
