import subprocess
import argparse
import sys
import os
from pathlib import Path
import re
import time
from typing import List, Iterator

class Colors:
    FATAL = '\033[95m'    # Purple
    ERROR = '\033[91m'    # Red
    WARN = '\033[93m'     # Yellow
    INFO = '\033[92m'    # Green
    DEBUG = '\033[94m'     # Blue
    TRACE = '\033[97m'    # White
    RESET = '\033[0m'     # Reset color


def is_piped() -> bool:
    return not sys.stdout.isatty()


def highlight_matches(line: str, patterns: List[re.Pattern], use_color: bool) -> str:
    if not use_color or not patterns:
        return line

    result = line
    for pattern in patterns:
        matches = list(pattern.finditer(line))
        if matches:
            offset = 0
            for match in matches:
                start, end = match.span()
                start += offset
                end += offset
                result = f"{result[:start]}\033[7m{result[start:end]}\033[27m{result[end:]}"
                offset += 8

    return result


def colorize(line: str, use_color: bool, patterns: List[re.Pattern] = None) -> str:
    if not use_color:
        return line

    colored_line = line
    if re.search(r'(\[CRITICAL\]|\[FATAL\])', line, re.IGNORECASE):
        colored_line = f"{Colors.FATAL}{line}{Colors.RESET}"
    elif re.search(r'(\[ERROR\])', line, re.IGNORECASE):
        colored_line = f"{Colors.ERROR}{line}{Colors.RESET}"
    elif re.search(r'\[WARN(ING)?\]', line, re.IGNORECASE):
        colored_line = f"{Colors.WARN}{line}{Colors.RESET}"
    elif re.search(r'\[INFO\]', line, re.IGNORECASE):
        colored_line = f"{Colors.INFO}{line}{Colors.RESET}"
    elif re.search(r'\[DEBUG\]', line, re.IGNORECASE):
        colored_line = f"{Colors.DEBUG}{line}{Colors.RESET}"
    elif re.search(r'\[TRACE\]', line, re.IGNORECASE):
        colored_line = f"{Colors.TRACE}{line}{Colors.RESET}"

    return highlight_matches(colored_line, patterns, use_color)

def get_log_files(base_log: Path) -> List[Path]:
    """Get all rotated log files in order."""
    files = []
    if base_log.exists():
        files.append(base_log)
    
    i = 1
    while True:
        rotated = base_log.with_suffix(f'.{i}.log')
        if not rotated.exists():
            break
        files.append(rotated)
        i += 1
    
    return sorted(files, reverse=True)


def wait_for_new_file(filepath: Path, max_wait: int = 30) -> bool:
    start_time = time.time()
    while time.time() - start_time < max_wait:
        if filepath.exists():
            return True
        time.sleep(0.1)
    return False

def follow_file(filepath: Path) -> Iterator[str]:
    position = 0
    current_file = filepath
    current_inode = None if not filepath.exists() else os.stat(filepath).st_ino

    while True:
        if not current_file.exists():
            print(f"\nLog file rotated, waiting for new file...", file=sys.stderr)
            if wait_for_new_file(filepath):
                current_file = filepath
                current_inode = os.stat(filepath).st_ino
                position = 0
            else:
                print(f"Timeout waiting for new log file", file=sys.stderr)
                break

        try:
            with open(current_file, 'r') as f:
                # Check if file has been rotated by comparing inodes
                try:
                    new_inode = os.stat(current_file).st_ino
                    if current_inode is not None and new_inode != current_inode:
                        position = 0
                        current_inode = new_inode
                except FileNotFoundError:
                    continue

                f.seek(position)
                while True:
                    line = f.readline()
                    if not line:
                        position = f.tell()
                        break
                    yield line.rstrip()

                time.sleep(0.1)

        except FileNotFoundError:
            continue
        except Exception as e:
            print(f"Error reading log file: {e}", file=sys.stderr)
            break


def filter_logs(filters: List[str], follow: bool = False, output_file: str = None) -> None:
    base_log = Path('DuetScreen.log')
    log_files = get_log_files(base_log)
    
    if not log_files:
        print("No log files found", file=sys.stderr)
        return

    patterns = [re.compile(f, re.IGNORECASE) for f in filters]
    use_color = not (is_piped() or output_file)

    output = open(output_file, 'w') if output_file else sys.stdout
    try:
        if not follow:
            for log_file in log_files:
                with open(log_file, 'r') as f:
                    for line in f:
                        if not filters or any(p.search(line) for p in patterns):
                            print(colorize(line.rstrip(), use_color, patterns), file=output)
            return

        for line in follow_file(base_log):
            if not filters or any(p.search(line) for p in patterns):
                print(colorize(line, use_color, patterns), file=output)
                if output_file:
                    output.flush()
    finally:
        if output_file:
            output.close()

def main():
    parser = argparse.ArgumentParser(description='Filter DuetScreen log files')
    parser.add_argument('--follow', '-f', action='store_true', help='Follow the log file in real-time')
    parser.add_argument('--output', '-o', help='Output file path')
    parser.add_argument('filters', nargs='*', help='Filter patterns')
    args = parser.parse_args()

    try:
        filter_logs(args.filters, args.follow, args.output)
    except KeyboardInterrupt:
        sys.exit(0)

if __name__ == '__main__':
    main()

