# 🎨 Source to HTML Converter (s2html)

A C-based tool that converts C source code into an HTML file with syntax highlighting. It parses the input file using a state-machine-based lexer and generates structured HTML output styled with CSS.

---

## 🚀 Features

* 🎯 Syntax highlighting for:

  * Keywords (`int`, `return`, `if`, etc.)
  * Identifiers (variables & functions)
  * Numeric constants (`123`, `45.6`)
  * String literals (`"Hello"`)
  * Character literals (`'a'`, `'\n'`)
  * Comments (`//`, `/* */`)
  * Preprocessor directives (`#include`, `#define`)
  * Header files:

    * Standard (`<stdio.h>`)
    * User-defined (`"file.h"`)
* 🔁 State-machine-based parsing (lexer)
* 🧩 Event-driven design (parser → converter)
* 🌐 HTML-safe output (`<` → `&lt;`, `>` → `&gt;`)
* 🎨 External CSS styling (`styles.css`)
* 📄 Preserves formatting using `<pre>`

---

## ⚙️ How to Compile & Run

### 🔹 Compile

```bash
gcc s2html_main.c s2html_event.c s2html_conv.c -o s2html
```

### 🔹 Run

```bash
./s2html input.c
```

### 🔹 Optional (custom output name)

```bash
./s2html input.c output_name
```

👉 This will generate:

```
input.c.html   OR   output_name.html
```

---

## 📁 File Structure

| File Name      | Description                               |
| -------------- | ----------------------------------------- |
| s2html_main.c  | Entry point, handles file I/O and flow    |
| s2html_event.c | Parser (state machine, token generation)  |
| s2html_event.h | Event types, structures, parser interface |
| s2html_conv.c  | Converts tokens to HTML with styling      |
| s2html_conv.h  | Function declarations for HTML conversion |
| styles.css     | CSS styles for syntax highlighting        |
| test.c         | Sample test file (covers all token types) |

---

## 🧠 Key Concepts Used

* Compiler Design (Lexical Analysis)
* Finite State Machine (FSM)
* Tokenization
* File Handling in C (`fgetc`, `fseek`)
* String Handling (`strcmp`, `strcpy`, `strlen`)
* Modular Programming
* HTML & CSS Integration

---

## 🔄 How It Works

1. The program reads the input file character by character.
2. The parser (state machine) identifies tokens based on context:

   * word, number, string, comment, preprocessor, etc.
3. Each token is stored as an event (`pevent_t`).
4. The converter maps events to HTML `<span>` tags with CSS classes.
5. Output is written inside a `<pre>` block to preserve formatting.

---

## 📌 Sample Output

```html
<span class="reserved_key1">int</span> main()
{
    <span class="reserved_key1">return</span> <span class="numeric_constant">0</span>;
}
```

---

## 🎨 Sample Highlighting

| Token Type        | Example      | Style Class         |
| ----------------- | ------------ | ------------------- |
| Keyword           | `int`        | `.reserved_key1`    |
| String            | `"Hello"`    | `.string`           |
| Number            | `123`        | `.numeric_constant` |
| Comment           | `// comment` | `.comment`          |
| Preprocessor      | `#include`   | `.preprocess_dir`   |
| Header File       | `<stdio.h>`  | `.header_file`      |
| Character Literal | `'a'`        | `.ascii_char`       |

---

## 🧑‍💻 Developer

**Fahed Shaikh**

🎓 BE in Electronics & Telecommunication Engineering

🛠️ Skills: C Programming, Embedded Systems, Parsing, File Handling

---

## 📌 About

This project simulates syntax highlighting similar to code editors by converting raw C source code into a styled HTML document. It demonstrates how lexical analysis and state-based parsing can be used to interpret and transform programming languages into structured output.
