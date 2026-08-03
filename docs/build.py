#!/usr/bin/env python3
"""
    Build the AMU docs: pull the theme submodule (first run) then run Doxygen.
    Works from any directory, on any platform. Requires git and doxygen on PATH.
"""
import os
import subprocess
import sys
from pathlib import Path

DOCS_DIR = Path(__file__).resolve().parent

def run(*cmd, env=None):
    print("+ " + " ".join(cmd))
    subprocess.run(cmd, cwd=DOCS_DIR, check=True, env=env)

def main():
    try:
        run("git", "submodule", "update", "--init", "--recursive")  # no-op once the theme is present

        # PROJECT_NUMBER reads $(AMULIB_VERSION) from the Doxyfile - keeps the docs version
        # tied to the same VERSION file the pre-push hook checks, instead of a 4th hardcoded copy.
        version = (DOCS_DIR.parent / "VERSION").read_text().strip()
        doxygen_env = {**os.environ, "AMULIB_VERSION": version}
        run("doxygen", "Doxyfile", env=doxygen_env)                 # output -> docs/html/index.html
    except FileNotFoundError as e:
        sys.exit(f"error: required tool not found on PATH: {e.filename}")
    except subprocess.CalledProcessError as e:
        sys.exit(e.returncode)

    print(f"Docs built -> {DOCS_DIR / 'html' / 'index.html'}")


if __name__ == "__main__":
    main()
