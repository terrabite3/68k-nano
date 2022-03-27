#!/usr/bin/env python3
# might not be a perfectly portable shebang

import os
from pathlib import PurePath
import shlex
import subprocess
import sys

tag = '68k-tools:0.0.4'

repo_root = PurePath(os.path.abspath(os.path.dirname(__file__))).parent.parent

cwd = os.getcwd()
uid = os.getuid()
gid = os.getgid()

prologue = f"docker run -v {repo_root}:{repo_root} -w {cwd} -u {uid}:{gid} {tag} m68k-elf-ar"

args = [
    *(shlex.split(prologue)),
    *sys.argv[1:]]

try:
    subprocess.run(args, check=True)
except subprocess.CalledProcessError as ex:
    print(f"Error: command exited with code {ex.returncode}.\n" +
        f"Args: {args}\nOutput:\n{ex.output.decode('UTF-8')}", file = sys.stderr)
    sys.exit(ex.returncode)