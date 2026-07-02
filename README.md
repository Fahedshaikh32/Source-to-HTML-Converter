<div align="center">

# S2Html — Source to HTML Converter

**A lightweight C utility that converts C source files into syntax-highlighted HTML documents.**

Built with a hand-written Finite State Machine lexer and an event-driven parsing architecture — no external dependencies.

[![Language](https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Build](https://img.shields.io/badge/build-gcc-blue)](https://gcc.gnu.org/)
[![License](https://img.shields.io/badge/license-MIT-green)](#license)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20WSL-lightgrey)](#)

[Overview](#overview) •
[Demo](#demo) •
[Installation](#installation) •
[Usage](#usage) •
[Architecture](#architecture) •
[Project Structure](#project-structure) •
[Roadmap](#roadmap)

</div>

---

## Overview

`s2html` reads a `.c` source file character by character, classifies each token using a **Finite State Machine (FSM)**, and emits a matching HTML file where every token is wrapped in a styled `<span>`. The result is a readable, syntax-highlighted rendering of your source code that opens directly in any browser.

It currently recognizes:

| Token type | Example | CSS class |
|---|---|---|
| Reserved keywords | `int`, `return`, `if`, `while` | `reserved_key1` |
| String literals | `"Hello World"` | `string` |
| Character literals | `'a'`, `'\n'` | `ascii_char` |
| Numeric constants | `123` | `numeric_constant` |
| Single-line comments | `// comment` | `comment` |
| Multi-line comments | `/* comment */` | `comment` |
| Preprocessor directives | `#include`, `#define` | `preprocess_dir` |
| Standard headers | `<stdio.h>` | `header_file` |
| User headers | `"myheader.h"` | `header_file` |

All other characters (identifiers, operators, whitespace) pass through as regular text so the original formatting is preserved via `<pre>`.

## Demo

**Input** — `test.c`
```c
#include <stdio.h>

int main()
{
    printf("Hello World");
    return 0;
}
```

**Output** — `test.c.html` (rendered)
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

Open the generated `.html` file in a browser (with `styles.css` alongside it) to see the highlighted result.

## Installation

### Prerequisites

- A C compiler (`gcc` or `clang`)
- POSIX-compatible shell (Linux, macOS, or WSL on Windows)

### Build from source

```bash
git clone https://github.com/Fahedshaikh32/Source-to-HTML-Converter.git
cd Source-to-HTML-Converter
gcc s2html_main.c s2html_event.c s2html_conv.c -o s2html
```

This produces an `s2html` executable in the current directory.

## Usage

```bash
./s2html <input_file.c> [output_name]
```

| Command | Result |
|---|---|
| `./s2html test.c` | Generates `test.c.html` |
| `./s2html test.c output` | Generates `output.html` |

> **Note:** Make sure `styles.css` is in the same directory as the generated HTML file so the highlighting renders correctly in the browser.

### Example

```bash
./s2html test.c
# Output file test.c.html generated
```

## Architecture

The parser processes input one character at a time, transitioning between states and emitting **parser events** that the HTML converter turns into styled markup.

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
        Syntax-Highlighted HTML File
```

**Parsing flow:**
1. `get_parser_event()` reads the source file character-by-character and drives the FSM (`s2html_event.c`).
2. Each state handler (idle, word, string, comment, preprocessor, etc.) accumulates characters and, on a state transition, returns a fully-formed `pevent_t` token.
3. `source_to_html()` (`s2html_conv.c`) maps each token type to a CSS class and writes the corresponding `<span>` element to the output file.
4. `html_begin()` / `html_end()` wrap the output in a complete, valid HTML document that links `styles.css`.

## Project Structure

```
.
├── s2html_main.c    # Entry point: file I/O and the main parse-convert loop
├── s2html_event.c    # FSM lexer: tokenizes source into parser events
├── s2html_event.h    # Event/token type definitions
├── s2html_conv.c    # Converts parser events into styled HTML output
├── s2html_conv.h    # HTML conversion function prototypes
├── styles.css       # Syntax highlighting theme
├── test.c           # Sample input file for testing
└── README.md
```

## Technologies Used

- **C** — core implementation
- **Finite State Machine (FSM)** — lexical analysis
- **File I/O** — stream-based source reading and HTML writing
- **HTML / CSS** — output rendering and styling

## Roadmap

- [ ] Floating-point number parsing
- [ ] Hexadecimal and binary literal support
- [ ] Multi-language syntax highlighting
- [ ] Command-line flags (theme, output directory, etc.)
- [ ] Configurable color themes
- [ ] Line numbering
- [ ] Improved error reporting

## Contributing

Contributions are welcome. If you'd like to add a feature or fix a bug:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes
4. Open a pull request

## License

This project is available under the MIT License. See the `LICENSE` file for details.

## Author

**Fahed Shaikh**
BE, Electronics & Telecommunication Engineering
Embedded Systems · C Programming · Linux · Compiler Fundamentals

<div align="center">

If you find this project useful, consider giving it a ⭐

</div>
