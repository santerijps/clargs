#ifndef CLARGS2_H

#define CLARGS2_h

#ifndef _INC_STDLIB
  #include <stdlib.h>
#endif

#ifndef _INC_STRING
  #include <string.h>
#endif

enum FlagType
{
  FLAG_BOOL,
  FLAG_INT,
  FLAG_STR,
};

union FlagValue
{
  int _bool;
  int _int;
  char *_str;
};

struct Flag
{
  int is_multi, is_set;
  const char *description, *name_long;
  char name_short;
  enum FlagType type;
  int value_capacity, value_count;
  union FlagValue *values;
};

struct OtherArgs
{
  int count, capacity;
  char **values;
};

struct FlagParser
{
  int flag_capacity, flag_count;
  struct Flag **flags;
};

/**
 * Initializes a new flag parser.
 * This should be done first.
*/
struct FlagParser
InitFlagParser()
{
  struct FlagParser fp;
  fp.flag_capacity = 5;
  fp.flag_count = 0;
  fp.flags = NULL;
  return fp;
}

struct Flag *
CreateFlag
(
  enum FlagType type,
  const char *name_long,
  const char name_short,
  const char *description,
  int is_multi
)
{
  struct Flag *f = malloc(sizeof(struct Flag));
  f->is_multi = is_multi;
  f->is_set = 0;
  f->description = description;
  f->name_long = name_long;
  f->name_short = name_short;
  f->type = type;
  f->value_capacity = 5;
  f->value_count = 0;
  f->values = NULL;
  return f;
}

/**
 * Initializes a new boolean flag and adds a reference of it
 * to the flag parser. Sets is_multi to 0;
 * @return A new struct Flag instance
*/
struct Flag *
NewBoolFlag
(
  struct FlagParser *fp,
  const char *name_long,
  const char name_short,
  const char *description
)
{
  if (fp->flag_count == 0 && fp->flags == NULL)
  {
    fp->flags = malloc(sizeof(struct Flag *) * fp->flag_capacity);
  }
  fp->flags[fp->flag_count++] = CreateFlag(FLAG_BOOL, name_long, name_short, description, 0);
  if (fp->flag_count == fp->flag_capacity)
  {
    fp->flag_capacity *= 2;
    fp->flags = realloc(fp->flags, sizeof(struct Flag *) * fp->flag_capacity);
  }
  return fp->flags[fp->flag_count - 1];
}

/**
 * Initializes a new integer flag and adds a reference of it
 * to the flag parser.
 * @return A new struct Flag instance
*/
struct Flag *
NewIntFlag
(
  struct FlagParser *fp,
  const char *name_long,
  const char name_short,
  const char *description,
  int is_multi
)
{
  if (fp->flag_count == 0 && fp->flags == NULL)
  {
    fp->flags = malloc(sizeof(struct Flag *) * fp->flag_capacity);
  }
  fp->flags[fp->flag_count++] = CreateFlag(FLAG_INT, name_long, name_short, description, is_multi);
  if (fp->flag_count == fp->flag_capacity)
  {
    fp->flag_capacity *= 2;
    fp->flags = realloc(fp->flags, sizeof(struct Flag *) * fp->flag_capacity);
  }
  return fp->flags[fp->flag_count - 1];
}

/**
 * Initializes a new string flag and adds a reference of it
 * to the flag parser.
 * @return A new struct Flag instance
*/
struct Flag *
NewStrFlag
(
  struct FlagParser *fp,
  const char *name_long,
  const char name_short,
  const char *description,
  int is_multi
)
{
  if (fp->flag_count == 0 && fp->flags == NULL)
  {
    fp->flags = malloc(sizeof(struct Flag *) * fp->flag_capacity);
  }
  fp->flags[fp->flag_count++] = CreateFlag(FLAG_STR, name_long, name_short, description, is_multi);
  if (fp->flag_count == fp->flag_capacity)
  {
    fp->flag_capacity *= 2;
    fp->flags = realloc(fp->flags, sizeof(struct Flag *) * fp->flag_capacity);
  }
  return fp->flags[fp->flag_count - 1];
}

struct OtherArgs
InitOtherArgs()
{
  struct OtherArgs oa;
  oa.capacity = 5;
  oa.count = 0;
  oa.values = NULL;
  return oa;
}

/**
 * Parse arguments and map them to the correct flags.
*/
struct OtherArgs
ParseArgs(struct FlagParser *fp, int argc, char **argv)
{
  char *arg, *long_name, short_name;
  int flag_index = -1, flag_was_found;
  size_t len;
  struct OtherArgs other_args = InitOtherArgs();
  for (int i = 1; i < argc; i++)
  {
    arg = argv[i];
    long_name = NULL;
    short_name = '\0';
    /////
    // Flag index has been set.
    // => Expect to read a flag value.
    if (flag_index != -1)
    {
      struct Flag *f = fp->flags[flag_index];
      union FlagValue *fv;
      if (f->values == NULL)
      {
        if (!f->is_multi)
        {
          f->values = malloc(sizeof(union FlagValue));
          f->value_count = 1;
        }
        else
        {
          f->values = malloc(sizeof(union FlagValue) * f->value_capacity);
          f->value_count++;
        }
        fv = f->values;
      }
      else
      {
        if (!f->is_multi)
        {
          fv = f->values;
          f->value_count = 1;
        }
        else
        {
          fv = &(f->values[f->value_count++]);
        }
      }
      switch (f->type)
      {
        case FLAG_INT:
          fv->_int = atoi(arg);
          break;
        case FLAG_STR:
          fv->_str = arg;
          break;
        default:
          // Uncreachable code
          break;
      }
      /////
      // Increase the value capacity on demand.
      if (f->is_multi && f->value_count == f->value_capacity)
      {
        f->value_capacity *= 2;
        f->values = realloc(f->values, f->value_capacity);
      }
      f->is_set = 1;
      flag_index = -1;
      /////
      // No need to check if long name or short name is not empty.
      // Since we just set the flag value.
      // => Continue to the next arg in argv.
      continue;
    }
    /////
    // Arg starts with a hyphen.
    // => It might be a flag
    else if (arg[0] == '-')
    {
      len = strlen(arg);
      /////
      // Arg length is 2 and the second character is not a hyphen.
      // => Short name (e.g. -v)
      if (len == 2 && arg[1] != '-')
      {
        short_name = arg[1];
      }
      /////
      // Arg length is greater than 2, the second character is a hyphen,
      // and the third character is not a hyphen.
      // => Long name (e.g. --version)
      else if (len > 2 && arg[1] == '-' && arg[2] != '-')
      {
        long_name = (arg + 2);
      }
    }
    /////
    // Long name or short name has been set.
    // => We should proceed to read the flag value
    if (long_name != NULL  || short_name != '\0')
    {
      flag_was_found = 0;
      /////
      // Find the flag in the flag parser.
      for (int j = 0; j < fp->flag_count; j++)
      {
        /////
        // Short name or long name matches a flag
        if ((fp->flags[j]->name_short == short_name && short_name != '\0') || (long_name != NULL && strcmp(fp->flags[j]->name_long, long_name) == 0))
        {
          /////
          // In the case of boolean flags, no value is read.
          // If the flag exists, the is_set field can be used.
          if (fp->flags[j]->type == FLAG_BOOL)
          {
            fp->flags[j]->is_set = 1;
            fp->flags[j]->value_count++;
          }
          /////
          // Otherwise, set the flag_index to the found flag index.
          // The next arg to be read will be inserted in this flag.
          else
          {
            flag_index = j;
          }
          flag_was_found = 1;
          break;
        }
      }
      /////
      // Flag was found
      // => Continue with the loop
      if (flag_was_found)
      {
        continue;
      }
    }
    /////
    // Add value to other args.
    if (other_args.values == NULL && other_args.count == 0)
    {
      other_args.values = malloc(sizeof(char*) * other_args.capacity);
    }
    other_args.values[other_args.count++] = arg;
    if (other_args.count == other_args.capacity)
    {
      other_args.capacity *= 2;
      other_args.values = realloc(other_args.values, sizeof(char*) * other_args.capacity);
    }
  }
  return other_args;
}

/**
 * @param f struct Flag pointer
 * @param index The index of the flag value
 * @return The integer value of a flag at the specified index.
*/
int
FlagIntVal(struct Flag *f, int index)
{
  return f->values[index]._int;
}

/**
 * @param f struct Flag pointer
 * @param index The index of the flag value
 * @return The string value of a flag at the specified index.
*/
char *
FlagStrVal(struct Flag *f, int index)
{
  return f->values[index]._str;
}

/**
 * A prettier way of saying f->is_set.
*/
int
FlagIsSet(struct Flag *f)
{
  return f->is_set;
}

char *
GetFlagTypeAsStr(enum FlagType t)
{
  switch (t)
  {
    case FLAG_BOOL:
      return "boolean";
    case FLAG_INT:
      return "integer";
    case FLAG_STR:
      return "string";
    default:
      return "undefined";
  }
}

static int
GetMaxFlagLength(struct FlagParser *fp)
{
  int max = 0, current, i;
  for (i = 0; i < fp->flag_count; i++)
  {
    current = strlen(fp->flags[i]->name_long);
    if (current > max)
    {
      max = current;
    }
  }
  return max;
}

void
PrintFlagUsage(struct FlagParser *fp)
{
  int long_max = GetMaxFlagLength(fp) + 2;
  printf("Usage:\n");
  for (int i = 0; i < fp->flag_count; i++)
  {
    struct Flag *f = fp->flags[i];
    char *ln = malloc(strlen(f->name_long) + 2);
    memset(ln, 0, strlen(f->name_long) + 2);
    ln[0] = ln[1] = '-';
    strcat(ln, f->name_long);
    printf(
      "  %-*s  -%c    <%s>\t%s\n",
      long_max, ln, f->name_short,
      GetFlagTypeAsStr(f->type), f->description
    );
  }
}

#endif