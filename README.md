# SquirrelShell

SquirrelShell is a simple Unix shell written in C designed specifically for teaching systems programming concepts, including parsing, process control, and POSIX calls.
It is structured to be easy to read, modular, and assignment-friendly, making it suitable for lab use.

The project is actively being adapted into coursework, including a planned fork where students implement portions of the execution engine to practice POSIX system calls.


## Educational Goals:  
SquirrelShell is designed to help students learn:
- Lexical analysis
- Top-down parsing
- Abstract Syntax Tree construction
- Process creation
- Basic interprocess communication
- File descriptor redirection


## Features:
- Tokenizer with quotation handling
- Recursive descent parser producing a left-associative Abstract Syntax Tree
- Execution engine based on AST traversal
- Pipelines (|)
- Input and output redirection (<, >, >>)
- Command sequences (;)
- Subshell grouping (( commands ))
- Logical operators (&&, ||)
- Built-in commands:
    - cd
    - exit
    - printast (prints parsed AST to better visualize the data structure)


## Assignments:
Planned instructional adaptations include:
- Providing students with a complete tokenizer and parser
- Replacing executor logic with instructional comments or light pseudocode
- Assignments will focus on:
    - Fork and exec usage
    - Pipe construction
    - Redirection using dup2
    - Process synchronization

This allows students to focus on POSIX process control without needing full shell design knowledge.


## AST Visualization:
To support learning and debugging, the shell includes:
- printast `<command>`

This prints the AST representation of a command, allowing students to observe how shell syntax is parsed.


## Future Features:
- Background execution
- Additional assignment templates
- Automated tests for student assignments


## Build Instructions 
### Requirements:
- POSIX-compliant operating system
- POSIX terminal and process model
- GCC
- Make

### Build:
make

### Run:
./squirrelshell
