#!/usr/bin/env python3
"""
List all functions and function-like macros from C/C++ header files
Usage: python list_functions.py [file.h]
       python list_functions.py            (uses ./code.h)
"""

import clang.cindex
from pathlib import Path
import sys
import re

# ANSI color codes
COLOR_FUNCTION = '\033[33m'  # Yellow for functions
COLOR_MACRO = '\033[35m'     # Magenta for macros
COLOR_RESET = '\033[0m'

def parse_functions(header_path):
    """Parse C/C++ header and extract functions using clang"""
    
    index = clang.cindex.Index.create()
    translation_unit = index.parse(
        header_path,
        args=['-x', 'c++']
    )
    
    functions = []
    
    def visit_node(node):
        """Recursively visit AST nodes"""
        
        if node.location.file and str(node.location.file) == str(header_path):
            
            if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
                return_type = node.result_type.spelling
                func_name = node.spelling
                
                # Get parameters
                params = []
                for arg in node.get_arguments():
                    param_type = arg.type.spelling
                    param_name = arg.spelling or ""
                    params.append(f"{param_type} {param_name}".strip())
                
                param_str = ", ".join(params) if params else "void"
                
                functions.append({
                    'name': func_name,
                    'type': 'function',
                    'signature': f"{return_type} {func_name}({param_str})",
                    'line': node.location.line
                })
        
        for child in node.get_children():
            visit_node(child)
    
    visit_node(translation_unit.cursor)
    return functions

def parse_macros(header_path):
    """Parse function-like macros from header file"""
    
    macros = []
    
    try:
        with open(header_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Regex to match function-like macros: #define NAME(args) ...
        # Matches: #define MACRO_NAME(param1, param2) body
        pattern = r'#define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(([^)]*)\)'
        
        for line_num, line in enumerate(content.split('\n'), 1):
            match = re.search(pattern, line)
            if match:
                macro_name = match.group(1)
                params = match.group(2).strip()
                
                # Get the full line (handle multi-line macros with backslashes)
                full_line = line.strip()
                
                macros.append({
                    'name': macro_name,
                    'type': 'macro',
                    'signature': f"#define {macro_name}({params})",
                    'line': line_num
                })
    
    except Exception as e:
        print(f"Warning: Could not parse macros: {e}")
    
    return macros

def main():
    # Get header file path from command line or default
    if len(sys.argv) > 1:
        header_path = Path(sys.argv[1])
    else:
        print("give c file path as arg")
        sys.exit(1)
    
    # print(f"Parsing {header_path}...\n")
    
    # Parse functions with clang
    functions = parse_functions(header_path)
    
    # Parse macros with regex
    macros = parse_macros(header_path)
    
    # Combine and sort by line number
    all_items = functions + macros
    all_items.sort(key=lambda x: x['line'])
    
    # Print results
    # print(f"{'=' * 70}")
    # print(f"FUNCTIONS: {len(functions)} | MACROS: {len(macros)} | TOTAL: {len(all_items)}")
    # print(f"{'=' * 70}\n")
    
    for item in all_items:
        if item['type'] == 'function':
            print(f"{COLOR_FUNCTION}[FUNC] {COLOR_RESET} [Line {item['line']:4d}] {item['signature']};")
        else:
            print(f"{COLOR_MACRO}[MACRO]{COLOR_RESET} [Line {item['line']:4d}] {item['signature']}")

if __name__ == "__main__":
    main()