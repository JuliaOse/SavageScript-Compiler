# 🎭 SarcasmLang - The Most Insulting Programming Language

*"Finally, a programming language that matches your personality!"*

SavageScript is a programming language built with C++ and LLVM. It features a lexer, parser, and AST-based code generator that compiles sarcastically-written source code (like genius: show 42) into LLVM IR, then runs it via LLVM’s JIT. The project isn’t just about learning compiler design—it’s about being insulted by your own compiler while you do it.
## Building the Compiler

### Prerequisites

1. **Install LLVM Development Libraries:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install llvm-dev clang
   
   # macOS with Homebrew
   brew install llvm
   
   # CentOS/RHEL
   sudo yum install llvm-devel clang
   ```

2. **Install CMake:**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install cmake
   
   # macOS with Homebrew
   brew install cmake
   ```

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure the build
cmake ..

# Build the compiler
make

# Run the compiler
./sarcasmlang
```

## 🎭 SarcasmLang Language Reference

### Core Philosophy
SarcasmLang embraces the fact that programming is frustrating, so why not make the language reflect that? Every line must begin with an insult, because let's face it - you probably deserve it.

### Syntax Rules

#### 1.Every Line Must Start With An Insult
```
insult: statement
```

#### 2. Available Insults (50+ Creative Options)
**Classic Insults:**
- `idiot`, `moron`, `dummy`, `fool`, `doofus`, `numbskull`, `dimwit`, `nincompoop`
- `bonehead`, `knucklehead`, `airhead`, `birdbrain`, `blockhead`, `chucklehead`
- `fathead`, `meathead`, `pinhead`

**Sarcastic Compliments:**
- `genius`, `einstein`, `smartass`, `brainiac`, `hotshot`, `wiseguy`
- `smarty`, `clever_clogs`, `know_it_all`, `rocket_scientist`, `mastermind`
- `prodigy`, `savant`, `intellectual`, `scholar`, `philosopher`

**Brain-Related Insults:**
- `smooth_brain`, `pea_brain`, `walnut_brain`, `goldfish_brain`, `big_brain`

**Gaming/Internet Culture:**
- `noob`, `scrub`, `casual`, `try_hard`, `wannabe`, `peasant`, `pleb`

**Primitive References:**
- `caveman`, `neanderthal`, `primitive`

**Skill Level Insults:**
- `amateur`, `rookie`, `newbie`

#### 3. Variables and Assignment
```
genius: x = 42
smartass: result = x plus 10
```

#### 4. Arithmetic Operations
**Traditional Operators:**
```
idiot: sum = a + b
moron: diff = a - b  
dummy: product = a * b
fool: quotient = a / b
```

**Word-Based Operators (More Sarcastic!):**
```
brainiac: sum = a plus b
doofus: diff = a minus b
numbskull: product = a times b
dimwit: quotient = a divided_by b
```

#### 5. Output Statements
Choose your flavor of condescending output:
```
genius: show x           # "Fine, here's your precious number: 42.00"
smarty: display y        # "Displaying for the visually impaired: 5.00"
hotshot: reveal z        # "The shocking revelation is: 47.00"
wiseguy: output result   # "Output (because you demanded it): 100.00"
```

#### 6. Conditional Logic
Use `obviously` because the condition should be obvious:
```
rocket_scientist: obviously x > 10 then {
    genius: show x
    smartass: display 999
}
```

#### 7. Loops
Use `whatever` because you're too cool to care:
```
casual: counter = 1
try_hard: whatever counter < 5 do {
    noob: show counter
    scrub: counter = counter plus 1
}
```

#### 8. Comparison Operators
```
<   # less than
>   # greater than
```

### 🚀 Example Programs

#### Hello World (SarcasmLang Style)
```sarcasmlang
genius: show 42
```

#### Basic Arithmetic Mockery
```sarcasmlang
idiot: x = 10
moron: y = 5
dummy: result = x plus y times 2
genius: show result
smartass: final = result divided_by 3
brainiac: display final
```

#### Loop with Maximum Sarcasm
```sarcasmlang
amateur: i = 1
noob: total = 0
casual: whatever i < 6 do {
    scrub: total = total plus i
    try_hard: show i
    wannabe: i = i plus 1
}
peasant: display total
```

#### Conditional Insult Fest
```sarcasmlang
rocket_scientist: score = 85
mastermind: obviously score > 80 then {
    genius: show 1
    einstein: display score
    brainiac: reveal 999
}
```

#### Factorial Calculator (With Attitude)
```sarcasmlang
numbskull: n = 5
bonehead: factorial = 1
meathead: i = 1
dimwit: whatever i < n plus 1 do {
    birdbrain: factorial = factorial times i
    airhead: i = i plus 1
}
smooth_brain: show factorial
```

#### Complex Expression Example
```sarcasmlang
pea_brain: a = 3
walnut_brain: b = 4
goldfish_brain: c = 5
big_brain: hypotenuse = (a times a plus b times b)
intellectual: obviously hypotenuse > c times c then {
    philosopher: show hypotenuse
    scholar: display c
}
```

## 🎪 Special Features

### 1. Sarcastic Compilation Comments
During compilation, SarcasmLang adds sarcastic comments about each line:
```
; Line 1: idiot says something ridiculous
; Line 2: genius says something ridiculous  
; Line 3: smartass says something ridiculous
```

### 2. Condescending Output Messages
Each output command has its own sarcastic message:
- `show`: "Fine, here's your precious number: X"
- `display`: "Displaying for the visually impaired: X"  
- `reveal`: "The shocking revelation is: X"
- `output`: "Output (because you demanded it): X"

### 3. Random Insult Generator
The compiler ends execution with a random insult, just to make sure you know where you stand.

### 4. LLVM Block Names
Even the generated LLVM IR gets sarcastic names:
- `obviously_then` (for if-then blocks)
- `obviously_cont` (for if continuation)
- `whatever_loop` (for while loop headers)
- `whatever_body` (for while loop bodies)

## 🎨 Language Design Philosophy

### Why SarcasmLang?
1. **Honesty**: Finally, a programming language that's honest about how it feels about your code
2. **Motivation**: Nothing motivates better than a good insult
3. **Entertainment**: Makes debugging fun when the compiler insults you too
4. **Reality**: Reflects the true nature of the programmer-computer relationship

### Educational Value
- Learn compiler construction while having fun
- Understand lexical analysis with creative token types
- Practice parsing with unconventional grammar rules
- Experience LLVM IR generation with personality

## 🔧 Architecture Details

### Enhanced Lexer Features
- **Insult Recognition**: Identifies 50+ insults as special tokens
- **Word-Based Operators**: Supports both symbolic (`+`) and word-based (`plus`) operators
- **Flexible Keywords**: `show`, `display`, `reveal`, `output` all work for printing

### Parser Enhancements
- **Mandatory Insult Parsing**: Every line must start with an insult
- **Flexible Operator Parsing**: Handles both traditional and word-based operators
- **Sarcastic Error Messages**: Even parse errors are insulting

### Code Generation Features
- **Dynamic Format Strings**: Different output messages based on print command
- **Sarcastic Comments**: Adds personality to generated LLVM IR
- **Named Basic Blocks**: Uses sarcastic names for control flow blocks

## 🚀 Extending SarcasmLang

### Adding New Insults
Simply add to the `insults` set:
```cpp
insults.insert("your_new_insult");
```

### Adding New Output Styles  
Extend the `PrintAST::codegen()` method:
```cpp
else if (printWord == "announce") format = "Ladies and gentlemen: %.2f\n";
```

### New Control Flow Constructs
Consider adding:
- `obviously_not` (else clause)
- `for_crying_out_loud` (for loop)
- `seriously` (assert statement)
- `whatever_nevermind` (break statement)

### Possible Future Features
- **String Insults**: Support for custom string-based insults
- **Insult Variables**: Variables that change their insult over time
- **Conditional Insults**: Different insults based on runtime conditions
- **Insult Functions**: Functions that return different insults
- **Multi-language Insults**: Support for insults in different languages
- **Insult Intensity Levels**: Mild, medium, and brutal insult modes

## 🎭 Fun Facts

- The compiler itself becomes increasingly sarcastic as compilation progresses
- LLVM basic blocks are named with attitude
- Error messages are more entertaining than helpful
- The random insult generator ensures no two program runs end the same way
- Even successful compilation ends with an insult

## 💡 Usage Tips

1. **Embrace the Insults**: Don't take them personally (the computer started it)
2. **Mix and Match**: Use different insults to keep things interesting
3. **Word Operators**: Try `plus` instead of `+` for extra verbosity
4. **Output Variety**: Experiment with `show`, `display`, `reveal`, and `output`
5. **Creative Insults**: The more creative your insults, the more fun the code
6. **Debugging**: When your code fails, at least you'll be entertained

---

*Remember: In SarcasmLang, every bug is just another opportunity for the language to mock you. Enjoy! 🎭*
