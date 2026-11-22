# mypreprocessor

A simple C preprocessor written in C. It reads a C source file and
creates an expanded output file with the `.i` extension.

## Features

- **File inclusion:** replaces `#include "file"` with the file contents.
  Files not found in the current folder (like `<stdio.h>`) are left as they are.
- **Comment removal:** removes `//` and `/* ... */` comments, and leaves
  comment-like text inside strings unchanged.
- **Macro substitution:** supports `#define` for simple macros
  (`#define PI 3.14`) and macros with arguments (`#define SQUARE(x) ((x)*(x))`).

## Project structure

| File | Purpose |
|---|---|
| `main.c` | Reads the file name and runs the three steps |
| `file_inclusion.c/.h` | Handles `#include` |
| `comment_removal.c/.h` | Removes comments |
| `macro_handler.c/.h` | Handles `#define` and macro replacement |
| `Makefile` | Builds the executable |
| `test.c` | Sample input file |

## How it works

```
input.c -> file inclusion -> comment removal -> macro substitution -> input.i
```

Temporary files (`temp1.tmp`, `temp2.tmp`) are created during processing
and deleted at the end.

## Build and run

```
make
./mypreprocessor test.c
cat test.i
```

## Known limitations

- Macros inside strings are also replaced.
- A macro used inside another macro's result is not expanded again.
- Macro arguments with commas inside brackets, like `MAX(f(1,2), 3)`, are split wrongly.
- At most 4 macro arguments.
- Nested `#include` inside included files is not expanded.
- `#undef`, `#if`, `#ifdef` and multi-line macros are not supported.

## Hardware

No hardware is required. This is a software-only project.
