#ifndef CLARGS_H

  #define CLARGS_H

  #ifndef _INC_STDIO
    #include <stdio.h>
  #endif

  #ifndef _INC_STDLIB
    #include <stdlib.h>
  #endif

  #ifndef _INC_STRING
    #include <string.h>
  #endif

  #define CLARGS_LONG_NAME_MAX_LENGTH 20
  #define CLARGS_FLAG_DESCRIPTION_MAX_LENGTH 50

  typedef enum FLAG_TYPE {
    FLAG_TYPE_BOOL,
    FLAG_TYPE_INT,
    FLAG_TYPE_STR,
  } FLAG_TYPE;

  typedef union FLAG_VALUE {
    short _bool;
    int _int;
    char* _str;
  } FLAG_VALUE;

  typedef struct FLAG {
    char long_name[CLARGS_LONG_NAME_MAX_LENGTH], short_name;
    FLAG_TYPE type;
    FLAG_VALUE value;
    FLAG_VALUE default_value;
    char description[CLARGS_FLAG_DESCRIPTION_MAX_LENGTH];
    short is_set;
  } FLAG;

  typedef struct FLAG_PARSER {
    int argc;
    char **argv;
    size_t flag_count, flag_capacity;
    FLAG **flags;
  } FLAG_PARSER;

  typedef struct CLARGS {
    size_t capacity, size;
    char **values;
  } CLARGS;

  FLAG_PARSER clargs_parser_new(int argc, char **argv);

  FLAG* clargs_flag_new(FLAG_PARSER *parser, FLAG_TYPE type, const char *long_name, const char short_name, const char *description);
  FLAG* clargs_flag_new_bool(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, short default_value);
  FLAG* clargs_flag_new_int(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, int default_value);
  FLAG* clargs_flag_new_str(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, const char *default_value);

  CLARGS clargs_parse_args(FLAG_PARSER *parser);

  FLAG_VALUE clargs_flag_value_new_bool(short value);
  FLAG_VALUE clargs_flag_value_new_int(int value);
  FLAG_VALUE clargs_flag_value_new_str(const char *value);

  void clargs_print_flag_usage(FLAG_PARSER *parser, size_t indent);

  FLAG_PARSER clargs_parser_new(int argc, char **argv) {
    FLAG_PARSER parser;
    parser.argc = argc;
    parser.argv = argv;
    parser.flag_count = 0;
    parser.flag_capacity = 10;
    parser.flags = (FLAG**) malloc(sizeof(FLAG*) * parser.flag_capacity);
    return parser;
  }

  FLAG* clargs_flag_new(FLAG_PARSER *parser, FLAG_TYPE type, const char *long_name, const char short_name, const char *description) {

    FLAG *flag = (FLAG *) malloc(sizeof(FLAG));

    strcpy(flag->long_name, long_name);
    strcpy(flag->description, description);

    flag->type = type;
    flag->short_name = short_name;
    flag->is_set = 0;

    switch (flag->type) {
      case FLAG_TYPE_BOOL:
        flag->value = clargs_flag_value_new_bool(0);
        break;
      case FLAG_TYPE_INT:
        flag->value = clargs_flag_value_new_int(0);
        break;
      case FLAG_TYPE_STR:
        flag->value = clargs_flag_value_new_str(NULL);
        break;
      default:
        printf("Unknown flag type!\n");
        exit(1);
    }

    parser->flags[parser->flag_count++] = flag;
    if (parser->flag_count >= parser->flag_capacity) {
      parser->flag_capacity *= 2;
      parser->flags = (FLAG**) realloc(parser->flags, sizeof(FLAG*) * parser->flag_capacity);
    }

    return flag;
  }

  FLAG* clargs_flag_new_bool(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, short default_value) {
    FLAG *flag = clargs_flag_new(parser, FLAG_TYPE_BOOL, long_name, short_name, description);
    flag->default_value = clargs_flag_value_new_bool(default_value);
    return flag;
  }

  FLAG* clargs_flag_new_int(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, int default_value) {
    FLAG *flag = clargs_flag_new(parser, FLAG_TYPE_INT, long_name, short_name, description);
    flag->default_value = clargs_flag_value_new_int(default_value);
    return flag;
  }

  FLAG* clargs_flag_new_str(FLAG_PARSER *parser, const char *long_name, const char short_name, const char *description, const char *default_value) {
    FLAG *flag = clargs_flag_new(parser, FLAG_TYPE_STR, long_name, short_name, description);
    flag->default_value = clargs_flag_value_new_str(default_value);
    return flag;
  }

  FLAG_VALUE clargs_flag_value_new_bool(short value) {
    FLAG_VALUE flag_value;
    flag_value._bool = value;
    return flag_value;
  }

  FLAG_VALUE clargs_flag_value_new_int(int value) {
    FLAG_VALUE flag_value;
    flag_value._int = value;
    return flag_value;
  }

  FLAG_VALUE clargs_flag_value_new_str(const char *value) {
    FLAG_VALUE flag_value;
    if (value == NULL) {
      flag_value._str = (char*) malloc(sizeof(char));
      strcpy(flag_value._str, "");
    } else {
      flag_value._str = (char*) malloc(sizeof(char) * (strlen(value)));
      strcpy(flag_value._str, value);
    }
    return flag_value;
  }

  CLARGS clargs_parse_args(FLAG_PARSER *parser) {

    CLARGS result;
    char *arg, short_name, *long_name, expecting_value;
    size_t i, j, arg_length, flag_index;
    FLAG *flag;

    result.capacity = 5;
    result.size = 0;
    result.values = (char**) malloc(sizeof(char*) * result.capacity);

    expecting_value = 0;
    flag_index = -1;

    for (i = 1; i < parser->argc; i++) {

      arg = parser->argv[i];
      arg_length = strlen(arg);

      // FLAG VALUE
      if (expecting_value == 1 && flag_index != -1) {
        parser->flags[flag_index]->is_set = 1;
        switch (parser->flags[flag_index]->type) {
          case FLAG_TYPE_INT:
            parser->flags[flag_index]->value = clargs_flag_value_new_int(atoi(arg));
            break;
          case FLAG_TYPE_STR:
            parser->flags[flag_index]->value = clargs_flag_value_new_str(arg);
            break;
          default:
            printf("Invalid FLAG_TYPE\n");
            exit(1);
        }
        expecting_value = 0;
      }

      // FLAG NAME
      else if (arg[0] == '-') {

        long_name = NULL;
        short_name = '\0';

        if (arg_length == 2 && arg[1] != '-') {
          short_name = arg[1];
        }

        else if (arg_length >= 3 && arg[1] == '-') {
          long_name = (char*) malloc(sizeof(char) * (arg_length - 1));
          for (j = 2; j < arg_length; j++) {
            long_name[j - 2] = arg[j];
          }
          long_name[j - 2] = '\0';
        }

        for (j = 0; j < parser->flag_count; j++) {
          flag = parser->flags[j];
          if ((long_name != NULL && strcmp(flag->long_name, long_name) == 0) || (short_name != '\0' && flag->short_name == short_name) ) {
            if (flag->type != FLAG_TYPE_BOOL) {
              expecting_value = 1;
              flag_index = j;
            }
            else {
              expecting_value = 0;
              flag_index = -1;
              parser->flags[j]->value = clargs_flag_value_new_bool(1);
              parser->flags[j]->is_set = 1;
            }
            break;
          }
        }

      }

      // OTHER ARG
      else if (expecting_value == 0) {
        result.values[result.size++] = arg;
        if (result.size == result.capacity) {
          result.capacity *= 2;
          result.values = (char**) realloc(result.values, sizeof(char*) * result.capacity);
        }
      }

    }

    // Set default values if flag is absent
    for (i = 0; i < parser->flag_count; i++) {
      if (parser->flags[i]->is_set) continue;
      switch (parser->flags[i]->type) {
        case FLAG_TYPE_BOOL:
          parser->flags[i]->value = clargs_flag_value_new_bool(parser->flags[i]->default_value._bool);
          break;
        case FLAG_TYPE_INT:
          parser->flags[i]->value = clargs_flag_value_new_int(parser->flags[i]->default_value._int);
          break;
        case FLAG_TYPE_STR:
          parser->flags[i]->value = clargs_flag_value_new_str(parser->flags[i]->default_value._str);
          break;
      }
    }

    return result;
  }

  void clargs_print_flag_usage(FLAG_PARSER *parser, size_t indent) {

    size_t i, len_ln, len_d;
    size_t max_len_ln, max_len_d;
    char *hyphened_ln;
    size_t hyphened_len;
    FLAG *f;

    max_len_ln = 0;
    max_len_d = 0;

    for (i = 0; i < parser->flag_count; i++) {
      f = parser->flags[i];
      len_ln = strlen(f->long_name);
      len_d = strlen(f->description);
      if (len_ln > max_len_ln) max_len_ln = len_ln;
      if (len_d > max_len_d) max_len_d = len_d;
    }

    for (i = 0; i < parser->flag_count; i++) {
      f = parser->flags[i];

      hyphened_len = strlen(f->long_name) + 2;
      hyphened_ln = (char*) malloc(sizeof(char) * (hyphened_len));
      memset(hyphened_ln, 0, hyphened_len);
      hyphened_ln[0] = '-';
      hyphened_ln[1] = '-';
      strcat(hyphened_ln, f->long_name);

      if (indent > 0) {
        printf("%*c", indent, ' ');
      }

      printf("-%c, %-*s\t<%s>\t%-*s\n",
        f->short_name, max_len_ln + 2, hyphened_ln,
        f->type == FLAG_TYPE_BOOL ? "boolean" :
        f->type == FLAG_TYPE_INT ? "integer" :
        f->type == FLAG_TYPE_STR ? "string" : "",
        max_len_d, f->description);

    }

  }

#endif
