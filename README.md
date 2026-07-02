# Source to HTML Converter (s2html)

A lightweight **Source-to-HTML Converter** written in **C** that transforms C source code into a syntax-highlighted HTML document.

The project uses a **Finite State Machine (FSM)** based lexical parser to identify different C language tokens such as keywords, strings, comments, numeric constants, character literals, preprocessor directives, and header files. Each parsed token is converted into styled HTML using CSS, producing readable source code similar to modern code editors.

---

## Features

- Syntax highlighting for C source files
- State-machine based lexical parser
- Event-driven parsing architecture
- Supports:
  - Reserved keywords
  - Identifiers
  - Numeric constants
  - String literals
  - Character literals
  - Single-line comments
  - Multi-line comments
  - Preprocessor directives
  - Standard header files (`<stdio.h>`)
  - User-defined header files (`"file.h"`)
- HTML-safe character escaping
- Preserves original source formatting using `<pre>`
- Modular project structure

---

## Project Architecture

```
                Source File (.c)
                        │
                        ▼
              Character-by-Character Parser
                        │
                        ▼
               Finite State Machine (FSM)
                        │
                        ▼
                 Parser Events (Tokens)
                        │
                        ▼
               HTML Conversion Module
                        │
                        ▼
            Syntax Highlighted HTML File
```

---

## Project Structure

```
.
├── s2html_main.c
├── s2html_event.c
├── s2html_event.h
├── s2html_conv.c
├── s2html_conv.h
├── styles.css
├── test.c
└── README.md
```

---

## Build

```bash
gcc s2html_main.c s2html_event.c s2html_conv.c -o s2html
```

---

## Usage

Generate HTML using the default output name:

```bash
./s2html input.c
```

Generate HTML using a custom output name:

```bash
./s2html input.c output
```

Output:

```
input.c.html
```

or

```
output.html
```

---

## Parser Workflow

The parser reads the source code **one character at a time**.

Depending on the current parser state, it generates parser events for different token types.

Each generated event is passed to the HTML converter, which wraps the token inside the appropriate HTML `<span>` element with CSS styling.

---

## Technologies Used

- C Programming
- Finite State Machine (FSM)
- Lexical Analysis
- File Handling
- HTML
- CSS
- Modular Programming

---

## Example

Input

```c
#include <stdio.h>

int main()
{
    printf("Hello World");
    return 0;
}
```

Output

```html
<span class="preprocess_dir">#include</span>
<span class="header_file">&lt;stdio.h&gt;</span>

<span class="reserved_key1">int</span> main()
{
    printf(<span class="string">"Hello World"</span>);
    <span class="reserved_key1">return</span>
    <span class="numeric_constant">0</span>;
}
```

---

## Future Improvements

- Floating-point number parsing
- Hexadecimal and binary literals
- Multi-language syntax highlighting
- Command-line options
- Theme support
- Line numbering
- Error reporting

---

## Author

**Fahed Shaikh**

BE Electronics & Telecommunication Engineering

Embedded Systems | C Programming | Linux | Compiler Fundamentals

---
