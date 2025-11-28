#include "ma.h"
#include <stdio.h>

int main() {
  vector(int) v = {0};

  for (int i = 0; i < 10; i++)
    vector_push(&v, i * 10);

  int more[] = {100, 200, 300};
  vector_push_many(v, more, sizeof(more));

  slice(int) sl;
  slice_from_vector(sl, v, v.len - 5, v.len);

  stringv sv = sv_from_cstr("bazinga");
  stringv sv1 = sv_from_cstr("baz");

  slice_foreach(it, &v) { printf("%d\n", *it); }

  stringb sb = sb_from_cstr("   bazinga");

  sb_append(sb, " another string  \n");

	stringv sv2 = sv_from_sb(sb);
	sv2 = sv_trim(sv2);

	// sb.len--;

  // sv_chop_left(&v, 1);

  sv_print(sv2);

}
