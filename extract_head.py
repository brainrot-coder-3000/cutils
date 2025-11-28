#!/usr/bin/env python3

import re
import sys

def extract_declarations(content):
    """Extract all declarations from C source content."""
    declarations = []
    
    # Remove single-line comments
    content = re.sub(r'//.*?$', '', content, flags=re.MULTILINE)
    
    # Remove multi-line comments
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    # Extract preprocessor directives (include, define, etc.)
    preprocessor = re.findall(r'^#.*?(?:\\\n.*?)*$', content, re.MULTILINE)
    for directive in preprocessor:
        declarations.append(directive)

    # Extract structs (including typedef struct)
    struct_pattern = r'(?:typedef\s+)?struct\s+\w*\s*\{[^}]*\}(?:\s*\w+)?;'
    structs = re.findall(struct_pattern, content, re.DOTALL)
    declarations.extend(structs)
    
    # Extract enums (including typedef enum)
    enum_pattern = r'(?:typedef\s+)?enum\s+\w*\s*\{[^}]*\}(?:\s*\w+)?;'
    enums = re.findall(enum_pattern, content, re.DOTALL)
    declarations.extend(enums)
    
    # Extract unions (including typedef union)
    union_pattern = r'(?:typedef\s+)?union\s+\w*\s*\{[^}]*\}(?:\s*\w+)?;'
    unions = re.findall(union_pattern, content, re.DOTALL)
    declarations.extend(unions)
    
    # Extract simple typedefs
    typedef_pattern = r'typedef\s+(?!struct|enum|union)[^;]+;'
    typedefs = re.findall(typedef_pattern, content)
    declarations.extend(typedefs)
    
    # Extract function implementations and convert to declarations
    # Match function definitions with bodies
    func_pattern = r'([a-zA-Z_][a-zA-Z0-9_*\s]+\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\))\s*\{[^}]*\}'
    
    functions = re.findall(func_pattern, content, re.DOTALL)
    for func in functions:
        # Clean up the function signature
        func_decl = func.strip()
        # Remove extra whitespace
        func_decl = re.sub(r'\s+', ' ', func_decl)
        # Skip main function
        if not re.match(r'int\s+main\s*\(', func_decl):
            declarations.append(func_decl + ';')
    
    return declarations

def generate_header(source_file, output_file=None):
    """Generate header file from source file."""
    try:
        with open(source_file, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File '{source_file}' not found.")
        return
    
    declarations = extract_declarations(content)
    
    # Generate header guard name
    # Remove .c extension if present, otherwise use full filename
    if source_file.endswith('.c'):
        base_name = source_file[:-2]
    else:
        base_name = source_file
    
    guard_name = base_name.upper().replace('.', '_').replace('/', '_') + '_H'
    
    # Build header content
    header = f"#ifndef {guard_name}\n"
    header += f"#define {guard_name}\n\n"
    
    for decl in declarations:
        header += decl + "\n"
    
    header += f"#endif /* {guard_name} */\n"

    
    # Output to file or stdout
    if output_file:
        with open(output_file, 'w') as f:
            f.write(header)
        print(f"Header file generated: {output_file}")
    else:
        print(header)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python header_gen.py <source_file> [output_file]")
        print("Example: python header_gen.py mycode.c mycode.h")
        sys.exit(1)
    
    source = sys.argv[1]
    output = sys.argv[2] if len(sys.argv) > 2 else None
    
    generate_header(source, output)