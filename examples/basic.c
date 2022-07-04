#include <stdio.h>
#include "../clargs.h"

int main (int argc, char **argv) {

  FLAG_PARSER parser;
  FLAG *name, *age, *is_single, *quote, *book, *citizenship;
  CLARGS other_args;
  int i;

  parser = clargs_parser_new(argc, argv);
  name = clargs_flag_new_str(&parser, "name", 'n', "Name of the person.", "Anonymous");
  age = clargs_flag_new_int(&parser, "age", 'a', "Age of the person.", -123);
  is_single = clargs_flag_new_bool(&parser, "single", 's', "Is the person single?", 0);

  other_args = clargs_parse_args(&parser);

  printf("name: %s\n", name->value._str);
  printf("age: %d\n", age->value._int);
  printf("is single: %s\n", is_single->value._bool ? "true" : "false");
  printf("other args count: %d\n", other_args.size);

  for (i = 0; i < other_args.size; i++) {
    printf("Other arg: %s\n", other_args.values[i]);
  }

  clargs_print_flag_usage(&parser, 0);

  return 0;
}