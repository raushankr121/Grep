[![progress-banner](https://backend.codecrafters.io/progress/grep/f60dc24b-b634-48e9-917a-f6fbdb023ef4)](https://app.codecrafters.io/users/codecrafters-bot?r=2qF)

This is a starting point for C++ solutions to the
["Build Your Own grep" Challenge](https://app.codecrafters.io/courses/grep/overview).

[Regular expressions](https://en.wikipedia.org/wiki/Regular_expression)
(Regexes, for short) are patterns used to match character combinations in
strings. [`grep`](https://en.wikipedia.org/wiki/Grep) is a CLI tool for
searching using Regexes.

In this challenge you'll build your own implementation of `grep`. Along the way
we'll learn about Regex syntax, how parsers/lexers work, and how regular
expressions are evaluated.

**Note**: If you're viewing this repo on GitHub, head over to
[codecrafters.io](https://codecrafters.io) to try the challenge.

# Passing the first stage

The entry point for your `grep` implementation is in `src/Server.cpp`. Study and
uncomment the relevant code, and push your changes to pass the first stage:

```sh
git commit -am "pass 1st stage" # any msg
git push origin master
```

Time to move on to the next stage!

# Running Locally

### Prerequisites

- **CMake** (v3.13+)
- **C++ Compiler** (TDM-GCC / MinGW or MSVC)

---

### Option 1: PowerShell / Command Prompt (Windows Native)

Use the helper script `your_program.bat` directly:

```powershell
# Pipe standard input
echo "apple" | .\your_program.bat -E "apple"

# Search in a file
.\your_program.bat -E "\d+" path\to\file.txt

# Recursive search in a directory
.\your_program.bat -r -E "needle" path\to\dir
```

---

### Option 2: Git Bash

Use the standard CodeCrafters script:

```bash
# Pipe standard input
echo "apple" | ./your_program.sh -E "apple"

# Test non-matching input (exits with code 1)
echo "banana" | ./your_program.sh -E "apple"
```

---

### Option 3: Manual CMake Build & Run

If you want to configure and build manually with MinGW:

```powershell
# 1. Configure the build directory
cmake -B build -S . -G "MinGW Makefiles"

# 2. Build the executable
cmake --build ./build

# 3. Run the binary
echo "hello world" | .\build\exe.exe -E "world"
```

---

## Testing Regex Features

Here are example commands to test supported patterns:

| Feature | Example Command | Expected Result |
| :--- | :--- | :--- |
| **Literal characters** | `echo "cat" \| .\your_program.bat -E "cat"` | Matches `cat` |
| **Digits (`\d`)** | `echo "item123" \| .\your_program.bat -E "\d+"` | Matches `item123` |
| **Alphanumeric (`\w`)** | `echo "alpha_1" \| .\your_program.bat -E "\w+"` | Matches `alpha_1` |
| **Positive Groups (`[...]`)** | `echo "cat" \| .\your_program.bat -E "c[aeiou]t"` | Matches `cat` |
| **Negative Groups (`[^...]`)** | `echo "cbt" \| .\your_program.bat -E "c[^aeiou]t"` | Matches `cbt` |
| **Zero or one (`?`)** | `echo "color" \| .\your_program.bat -E "colou?r"` | Matches `color` |
| **One or more (`+`)** | `echo "caaat" \| .\your_program.bat -E "ca+t"` | Matches `caaat` |
| **Alternation (`(a\|b)`)** | `echo "dog" \| .\your_program.bat -E "(cat\|dog)"` | Matches `dog` |
| **Backreferences (`\1`)** | `echo "cat and cat" \| .\your_program.bat -E "(\w+) and \1"` | Matches `cat and cat` |

---

## Submitting to CodeCrafters

Once your local tests pass, commit and push your solution:

```sh
git commit -am "Pass stage"
git push origin master
```
Test results will stream automatically to your terminal.
