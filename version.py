#!/usr/bin/env python3
"""Generate version header from VERSION file and git info.

Usage:
    python version.py                      # Generate C header (default)
    python version.py patch                # Bump version (patch/minor/major)
    python version.py --output-dir <dir>   # Custom output directory
    python version.py --language python    # Generate Python module
    python version.py --language c,python  # Generate both
"""

import subprocess
import os
import sys
import re
from datetime import datetime

# Configuration
VERSION_FORMAT = 'full'        # Which format to use for VERSION_STR (full, compact, semantic, minimal, commit, build)
PROJECT_PREFIX = "AMULIB"      # Prefix for macros/variables (e.g., AMULIB_VERSION_FULL)
OUTPUT_DIR = 'src/amulibc'     # Where to write generated files

def run_git_command(command):
    result = subprocess.run(command, shell=True, capture_output=True, text=True, cwd=os.path.dirname(__file__))
    return result.stdout.strip() if result.returncode == 0 else None

def read_version_file():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    version_file = os.path.join(script_dir, 'VERSION')
    try:
        with open(version_file, 'r') as f:
            return f.read().strip()
    except:
        return "0.0.0"

def write_version_file(major, minor, patch):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    version_file = os.path.join(script_dir, 'VERSION')
    new_version = f"{major}.{minor}.{patch}"
    with open(version_file, 'w') as f:
        f.write(new_version + '\n')
    return new_version

def bump_version(bump_type):
    version = read_version_file()
    major, minor, patch = map(int, version.split('.'))
    if bump_type == 'major':
        major, minor, patch = major + 1, 0, 0
    elif bump_type == 'minor':
        minor, patch = minor + 1, 0
    elif bump_type == 'patch':
        patch += 1
    else:
        major, minor, patch = map(int, bump_type.split('.'))
    return write_version_file(major, minor, patch)

# ============================================================================
# Version Formats - Add new formats here
# ============================================================================
# To add a format: def format_yourname(info): return "format string"
# It will automatically appear as VERSION_YOURNAME in C and __version_yourname__ in Python

def format_full(info):
    parts = [f"v{info['semantic_version']}"]
    if info['commit'] != 'unknown':
        parts.append(f"+git.{info['commit']}")
    if info['dirty']:
        parts.append("-dirty")
    return ''.join(parts)

def format_compact(info):
    parts = [f"v{info['semantic_version']}"]
    if info['commit'] != 'unknown':
        parts.append(f"-{info['commit']}")
        if info['dirty']:
            parts.append("*")
    return ''.join(parts)

def format_semantic(info):
    return f"v{info['semantic_version']}"

def format_minimal(info):
    return info['semantic_version']

def format_commit(info):
    if info['commit'] != 'unknown':
        return info['commit'] + ('*' if info['dirty'] else '')
    return 'unknown'

def format_build(info):
    return f"{info['semantic_version']}.{info['commit_count']}"

def write_if_changed(output_file, content, timestamp_markers):
    """Write file only if content changed (ignoring timestamp lines)"""
    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    if os.path.exists(output_file):
        with open(output_file, 'r') as f:
            old_content = f.read()
        old_lines = [l for l in old_content.split('\n') if not any(m in l for m in timestamp_markers)]
        new_lines = [l for l in content.split('\n') if not any(m in l for m in timestamp_markers)]
        if old_lines == new_lines:
            print(f"Unchanged: {output_file}")
            return True

    with open(output_file, 'w') as f:
        f.write(content)
    print(f"Generated: {output_file}")
    return True

def get_version_info():
    semantic_version = read_version_file()
    version_info = {
        'semantic_version': semantic_version,
        'commit': run_git_command('git rev-parse --short HEAD') or 'unknown',
        'branch': run_git_command('git rev-parse --abbrev-ref HEAD') or 'unknown',
        'dirty': bool(run_git_command('git status --porcelain')),
        'commit_count': run_git_command('git rev-list --count HEAD') or '0',
        'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    }

    # Build all formats using registry
    format_funcs = {name[7:]: func for name, func in globals().items()
                    if name.startswith('format_') and callable(func)}
    formats = {name: func(version_info) for name, func in format_funcs.items()}

    version_str = formats.get(VERSION_FORMAT)
    if not version_str:
        print(f"Warning: Unknown VERSION_FORMAT '{VERSION_FORMAT}', using 'compact'")
        version_str = formats['compact']

    version_info['formats'] = formats
    version_info['version_str'] = version_str
    version_info['buffer_size'] = max(len(v) for v in formats.values()) + 1
    return version_info

def generate_c_header(version_info, output_file, prefix):
    major, minor, patch = version_info['semantic_version'].split('.')
    pfx = f"{prefix}_" if prefix else ""

    format_macros = '\n'.join(f'#define {pfx}VERSION_{fmt.upper()} "{ver}"'
                               for fmt, ver in sorted(version_info['formats'].items()))

    header_content = f'''#ifndef {pfx}VERSION_H
#define {pfx}VERSION_H

#include <stdio.h>

#define {pfx}VERSION_MAJOR {major}
#define {pfx}VERSION_MINOR {minor}
#define {pfx}VERSION_PATCH {patch}
#define {pfx}VERSION_SEMANTIC "{version_info['semantic_version']}"

#define {pfx}VERSION_COMMIT "{version_info['commit']}"
#define {pfx}VERSION_BRANCH "{version_info['branch']}"
#define {pfx}VERSION_TIMESTAMP "{version_info['timestamp']}"

#define {pfx}VERSION_DIRTY {1 if version_info['dirty'] else 0}
#define {pfx}VERSION_COMMIT_COUNT {version_info['commit_count']}

{format_macros}

#define {pfx}VERSION_STR {pfx}VERSION_{VERSION_FORMAT.upper()}
#define {pfx}VERSION_STR_LEN {version_info['buffer_size']}

#endif // {pfx}VERSION_H
'''
    return write_if_changed(output_file, header_content, ['VERSION_TIMESTAMP'])

def generate_python_module(version_info, output_file, prefix):
    major, minor, patch = version_info['semantic_version'].split('.')
    prefix_upper = prefix.upper() if prefix else ""
    prefix_with_sep = f"{prefix_upper}_" if prefix_upper else ""
    formats = version_info['formats']

    format_vars = '\n'.join(f'__version_{fmt}__ = "{ver}"'
                             for fmt, ver in sorted(formats.items()))

    content = f'''"""Auto-generated version info"""

__version__ = "{version_info['semantic_version']}"

{format_vars}

{prefix_with_sep}VERSION_MAJOR = {major}
{prefix_with_sep}VERSION_MINOR = {minor}
{prefix_with_sep}VERSION_PATCH = {patch}

{prefix_with_sep}GIT_COMMIT = "{version_info['commit']}"
{prefix_with_sep}GIT_BRANCH = "{version_info['branch']}"
{prefix_with_sep}GIT_DIRTY = {version_info['dirty']}
{prefix_with_sep}GIT_COMMIT_COUNT = {version_info['commit_count']}
{prefix_with_sep}BUILD_TIMESTAMP = "{version_info['timestamp']}"
'''
    return write_if_changed(output_file, content, ['BUILD_TIMESTAMP', 'timestamp'])

# ============================================================================
# Language Generators - Add new languages here
# ============================================================================
# To add a language: (filename_lambda, generator_function)
# Example: 'rust': (lambda p: f"{p.lower()}_version.rs", generate_rust_file)

GENERATORS = {
    'c': (lambda p: f"{p.lower()}_version.h" if p else "version.h", generate_c_header),
    'python': (lambda p: f"{p.lower()}_version.py" if p else "_version.py", generate_python_module),
}

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = OUTPUT_DIR
    languages = ['c']
    args = sys.argv[1:]

    if '--output-dir' in args:
        idx = args.index('--output-dir')
        if idx + 1 < len(args):
            output_dir = args[idx + 1]
            args = args[:idx] + args[idx + 2:]

    if '--language' in args or '--lang' in args:
        key = '--language' if '--language' in args else '--lang'
        idx = args.index(key)
        if idx + 1 < len(args):
            languages = args[idx + 1].split(',')
            args = args[:idx] + args[idx + 2:]

    if args and args[0] in ['patch', 'minor', 'major'] or (args and re.match(r'^\d+\.\d+\.\d+$', args[0])):
        bump_version(args[0])

    version_info = get_version_info()

    for lang in languages:
        lang = lang.strip().lower()
        if lang not in GENERATORS:
            print(f"Warning: Unknown language '{lang}', skipping")
            continue
        filename_func, generator_func = GENERATORS[lang]
        filename = filename_func(PROJECT_PREFIX)
        output_file = os.path.join(script_dir, output_dir, filename)
        generator_func(version_info, output_file, PROJECT_PREFIX)

    print(f"Version: {version_info['version_str']}")
    return 0

if __name__ == '__main__':
    sys.exit(main())
