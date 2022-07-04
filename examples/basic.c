#include <stdio.h>
#include "../clargs.h"

int main (int argc, char **argv) {

  FLAG_PARSER parser;
  FLAG *name, *age, *is_single;
  CLARGS other_args;
  int i;

  parser = clargs_parser_new(argc, argv);
  name = clargs_flag_new_str(&parser, "name", 'n', "Name of the person.", "Anonymous");
  age = clargs_flag_new_int(&parser, "age", 'a', "Age of the person.", -123);
  is_single = clargs_flag_new_bool(&parser, "single", 's', "Is the person single?", 0);

  other_args = clargs_parse_args(&parser);

  printf("\nname: %s\n", name->value._str);
  printf("age: %d\n", age->value._int);
  printf("is single: %s\n\n", is_single->value._bool ? "true" : "false");

  printf("Other (non-flag) args:\n");
  for (i = 0; i < other_args.size; i++) {
    printf("  %d. %s\n", i + 1, other_args.values[i]);
  }

  // Print usage of all configured flags
  printf("\nPrint all flags at once:\n");
  clargs_print_flag_usage_all(&parser, 2);

  // Print single flag usage
  printf("\nPrint flags one at a time:\n");
  clargs_print_flag_usage(&parser, name, 2);
  clargs_print_flag_usage(&parser, age, 2);
  clargs_print_flag_usage(&parser, is_single, 2);
  printf("\n");

  return 0;
}