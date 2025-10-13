#!/usr/bin/env python3
import sys
import json
from pathlib import Path

def trim(s: str) -> str:
    return s.strip()

def parse_yaml_i18n(path: Path) -> dict:
    data = {}
    lang = None
    in_top = False
    pending_multiline_key = None
    pending_indent = None
    pending_value_lines = []

    lines = path.read_text(encoding='utf-8').splitlines()

    i = 0
    while i < len(lines):
        raw = lines[i]
        i += 1
        # remove BOM on very first line if present
        if i == 1 and raw.startswith('\ufeff'):
            raw = raw.lstrip('\ufeff')
        line = raw.rstrip('\r')
        stripped = line.strip()
        if not stripped or stripped.startswith('#'):
            continue

        # If we are collecting a folded multiline (>-) value
        if pending_multiline_key is not None:
            # A continuation line must be more indented than pending_indent
            current_indent = len(line) - len(line.lstrip(' \t'))
            if current_indent > pending_indent:
                # Append content with space folding
                pending_value_lines.append(line.strip())
                continue
            else:
                # finalize folded value
                folded = ' '.join(l for l in pending_value_lines if l is not None)
                data[pending_multiline_key] = folded
                pending_multiline_key = None
                pending_indent = None
                pending_value_lines = []
                # fallthrough to process this line as a new record
                stripped = line.strip()
                if not stripped:
                    continue

        if not in_top:
            # expect a top-level key like: en-GB:
            if stripped.endswith(':'):
                lang = stripped[:-1].strip()
                in_top = True
            # else ignore until we find it
            continue

        # we expect indented key: value
        if line[0] not in (' ', '\t'):
            # new top level / end of section
            break

        # Remove one level of indentation (2 spaces or a tab count as indent here)
        body = line.lstrip(' \t')
        if ':' not in body:
            # malformed, skip
            continue
        key, val = body.split(':', 1)
        key = key.rstrip()
        val = val.lstrip(' ')  # remove one leading space before value

        # Handle folded scalar >-
        if val.startswith('>-'):
            pending_multiline_key = key
            pending_indent = len(line) - len(line.lstrip(' \t'))
            pending_value_lines = []
            continue

        # YAML null marker
        if val == '~':
            data[key] = None
            continue

        # Strip single or double quotes if present
        if len(val) >= 2 and ((val[0] == "'" and val[-1] == "'") or (val[0] == '"' and val[-1] == '"')):
            val = val[1:-1]
            if val is None:
                val = ''
            # In single-quoted YAML, doubled '' becomes single '
            # Our simplistic parser doesn't track which quote was used; attempt replacement safely
            val = val.replace("''", "'")

        # Preserve literal backslashes: ensure JSON will keep them by escaping when dumping
        data[key] = val

    # finalize any pending folded
    if pending_multiline_key is not None:
        folded = ' '.join(l for l in pending_value_lines if l is not None)
        data[pending_multiline_key] = folded

    if lang is None:
        # fallback: wrap as unknown
        return {"i18n": data}
    return {lang: data}


def main():
    if len(sys.argv) < 3:
        print("Usage: yml_to_json_i18n.py <input.yml> <output.json>")
        sys.exit(2)
    in_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2])
    obj = parse_yaml_i18n(in_path)
    out_path.write_text(json.dumps(obj, ensure_ascii=False, indent=2), encoding='utf-8')

if __name__ == '__main__':
    main()
