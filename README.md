# clargs

`clargs` is a command line argument parsing library for the C programming language. This library was inspired by the [Go flag](https://pkg.go.dev/flag) package. The flags currently support:

- Booleans (`bool`)
- Integers (`int`)
- Strings (`char*`)

Find usage information in the `examples` directory. The most important functions are shown below.

```c
// Initialize the parser
FLAG_PARSER clargs_parser_new(int argc, char **argv);

// Create flags
FLAG* clargs_flag_new(FLAG_PARSER *parser, FLAG_TYPE type, const char *long_name, const char short_name, const char *description);
FLAG* clargs_flag_new_bool(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, bool default_value);
FLAG* clargs_flag_new_int(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, int default_value);
FLAG* clargs_flag_new_str(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, const char *default_value);

// Parse the argv, map to flags
CLARGS clargs_parse_args(FLAG_PARSER *parser);

// Print the usage of the flags
void clargs_print_flag_usage(FLAG_PARSER *parser, size_t indent);
```
