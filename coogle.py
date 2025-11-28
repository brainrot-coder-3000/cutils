#!/usr/bin/env python3
"""
Search C/C++ header files for functions by return type and argument types
Usage: python c_parser.py <file.h> ["search query"]
       python c_parser.py mycode.h "void*"
       python c_parser.py mycode.h              (interactive mode)
"""

import clang.cindex
from pathlib import Path
import sys
from collections import Counter

# ANSI color codes
COLOR_TYPE = '\033[36m'      # Cyan for types
COLOR_NAME = '\033[33m'      # Yellow for function names
COLOR_RESET = '\033[0m'      # Reset to default
UNDERLINE = '\033[4m'        # Underline
UNDERLINE_OFF = '\033[24m'   # Underline off

def normalize_type(type_str):
    """Normalize type string for comparison"""
    # Remove const
    type_str = type_str.replace('const', '').strip()
    # Normalize whitespace around asterisks for pointer types
    type_str = type_str.replace('*', ' * ')
    # Normalize whitespace
    type_str = ' '.join(type_str.split())
    # Case insensitive
    type_str = type_str.lower()
    return type_str

def parse_functions(header_path):
    """Parse C/C++ header and extract functions"""
    
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
                return_type = normalize_type(node.result_type.spelling)
                func_name = node.spelling
                
                # Get parameters
                param_types = []
                params = []
                for arg in node.get_arguments():
                    param_type = normalize_type(arg.type.spelling)
                    param_types.append(param_type)
                    param_name = arg.spelling or ""
                    params.append(f"{arg.type.spelling} {param_name}".strip())
                
                param_str = ", ".join(params) if params else "void"
                
                functions.append({
                    'name': func_name,
                    'return_type': return_type,
                    'param_types': param_types,
                    'signature': f"{node.result_type.spelling} {func_name}({param_str})",
                    'line': node.location.line
                })
        
        for child in node.get_children():
            visit_node(child)
    
    visit_node(translation_unit.cursor)
    return functions

def format_signature(func, search_terms=None):
    """Format function signature with colors and highlight search terms"""
    return_type = func['signature'].split(func['name'])[0].strip()
    
    # Extract arguments (everything after function name)
    rest = func['signature'].split(func['name'], 1)[1]
    
    # Highlight search terms if provided
    if search_terms:
        for term in search_terms:
            if not term:
                continue
            # Case insensitive replacement with underline
            import re
            pattern = re.compile(re.escape(term), re.IGNORECASE)
            return_type = pattern.sub(lambda m: f"{UNDERLINE}{m.group()}{UNDERLINE_OFF}", return_type)
            rest = pattern.sub(lambda m: f"{UNDERLINE}{m.group()}{UNDERLINE_OFF}", rest)
    
    return f"{COLOR_TYPE}{return_type}{COLOR_RESET} {COLOR_NAME}{func['name']}{COLOR_RESET}{rest}"

def search_functions(functions, query):
    """
    Search functions by return type and argument types
    Rules:
    - "int," -> functions returning int (any args)
    - ",int,float" -> functions with those args (any return type)
    - "int" (no comma) -> functions with int in return type OR arguments
    """
    if not query.strip():
        return functions
    
    has_comma = ',' in query
    
    # No comma: search in both return type and arguments
    if not has_comma:
        search_term = normalize_type(query.strip())
        matches = []
        for func in functions:
            # Check return type
            if search_term in func['return_type']:
                matches.append(func)
                continue
            # Check any argument
            if any(search_term in ptype for ptype in func['param_types']):
                matches.append(func)
        return matches
    
    # Has comma: parse parts
    parts = [normalize_type(p.strip()) for p in query.split(',')]
    
    # Check if starts with comma (any return type)
    if query.strip().startswith(','):
        search_return_type = None
        search_arg_types = Counter(parts[1:])  # Skip first empty element
    else:
        search_return_type = parts[0] if parts[0] else None
        search_arg_types = Counter(parts[1:]) if len(parts) > 1 else Counter()
    
    matches = []
    
    for func in functions:
        # Check return type (skip if None means any return type)
        if search_return_type is not None:
            if not search_return_type:
                # Empty but not None means no specific requirement beyond args
                pass
            elif search_return_type not in func['return_type']:
                continue
        
        # Check argument types (order doesn't matter)
        # All searched arg types must be present in function
        match = True
        for arg_type, count in search_arg_types.items():
            if not arg_type:  # Skip empty strings
                continue
            # Check if the arg_type is a substring of any function param type
            found_count = sum(1 for ftype in func['param_types'] if arg_type in ftype)
            if found_count < count:
                match = False
                break
        
        if match:
            matches.append(func)
    
    return matches

def main():
    # Require at least one argument (the header file)
    if len(sys.argv) < 2:
        print("Usage: python c_parser.py <file.h> [search query]")
        print("       python c_parser.py mycode.h \"void*\"")
        print("       python c_parser.py mycode.h              (interactive mode)")
        sys.exit(1)
    
    # First argument is always the header file
    header_path = Path(sys.argv[1])
    search_query = ' '.join(sys.argv[2:]) if len(sys.argv) > 2 else None
    
    if not header_path.exists():
        print(f"Error: {header_path} not found!")
        sys.exit(1)
    
    print(f"Parsing {header_path}...\n")
    functions = parse_functions(header_path)
    print(f"Loaded {len(functions)} functions\n")
    
    # Interactive search loop
    print("Search rules:")
    print("  'int,'           -> functions returning int (any args)")
    print("  ',int,float'     -> functions with those args (any return type)")
    print("  'int'            -> functions with int in return OR args")
    print("  'int,char*,bool' -> functions returning int with those args")
    print("Type 'quit' to exit\n")
    
    while True:
        try:
            if search_query:
                # Single search from command line
                query = search_query
                print(f"Search: {query}")
            else:
                # Interactive mode
                query = input("Search: ").strip()
            
            if query.lower() in ('quit', 'exit', 'q'):
                break
            
            matches = search_functions(functions, query)
            
            # Extract search terms for highlighting
            search_terms = []
            if ',' in query:
                parts = [p.strip() for p in query.split(',')]
                search_terms = [p for p in parts if p]
            else:
                search_terms = [query.strip()]
            
            print(f"\n{'=' * 70}")
            print(f"MATCHES: {len(matches)}")
            print(f"{'=' * 70}\n")
            
            for func in sorted(matches, key=lambda x: x['line']):
                print(f"{format_signature(func, search_terms)};")
            
            print()
            
            if search_query:
                # Exit after single search
                break
                
        except EOFError:
            break
        except KeyboardInterrupt:
            print("\n")
            break

if __name__ == "__main__":
    main()