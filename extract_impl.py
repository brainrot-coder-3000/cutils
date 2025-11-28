#!/usr/bin/env python3
"""
C Implementation File Generator
Extracts only function implementations from C source files.
"""

import re
import sys

def extract_implementations(content):
    """Extract only function implementations from C source content."""
    implementations = []
    
    # Remove single-line comments
    content = re.sub(r'//.*?$', '', content, flags=re.MULTILINE)
    
    # Remove multi-line comments
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    # Extract function implementations (functions with bodies)
    # This regex captures the entire function including its body
    func_pattern = r'([a-zA-Z_][a-zA-Z0-9_*\s]+\s+[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{(?:[^{}]|\{(?:[^{}]|\{[^{}]*\})*\})*\})'
    
    functions = re.findall(func_pattern, content, re.DOTALL)
    for func in functions:
        # Clean up extra whitespace but preserve function body formatting
        func = func.strip()
        implementations.append(func)
    
    return implementations

def generate_implementation(source_file, output_file=None):
    """Generate implementation file from source file."""
    try:
        with open(source_file, 'r') as f:
            content = f.read()
    except FileNotFoundError:
        print(f"Error: File '{source_file}' not found.")
        return
    
    implementations = extract_implementations(content)
    
    # Determine header file name
    if source_file.endswith('.c'):
        header_file = source_file[:-2] + '.h'
    else:
        header_file = source_file + '.h'
    
    # Build implementation content
    impl = f'#include "{header_file}"\n\n'
    
    for func_impl in implementations:
        impl += func_impl + "\n\n"
    
    # Output to file or stdout
    if output_file:
        with open(output_file, 'w') as f:
            f.write(impl)
        print(f"Implementation file generated: {output_file}")
    else:
        print(impl)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python impl_gen.py <source_file> [output_file]")
        print("Example: python impl_gen.py mycode.c mycode_impl.c")
        sys.exit(1)
    
    source = sys.argv[1]
    output = sys.argv[2] if len(sys.argv) > 2 else None
    
    generate_implementation(source, output)