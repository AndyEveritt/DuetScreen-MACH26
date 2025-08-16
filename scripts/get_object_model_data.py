#!/usr/bin/env python3
"""
Fetch selected Duet object model sections and save them as JSON files under tests/object_model.

Usage:
  python3 scripts/get_object_model_data.py --ip 192.168.1.123
  python3 scripts/get_object_model_data.py --ip http://duet.local
"""

from __future__ import annotations

import argparse
import json
import requests
import sys
from pathlib import Path
from typing import Dict, Tuple, Any


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Fetch Duet object model data to tests/object_model JSON files")
    target = parser.add_mutually_exclusive_group(required=True)
    target.add_argument("--ip", help="IP address or hostname of the Duet, e.g. 192.168.1.50 or duet.local")
    parser.add_argument("--prefix", default="http://", help="Protocol prefix for --ip if needed (default: http://)")
    parser.add_argument("--out", default="tests/object_model", help="Output directory for JSON files")
    args = parser.parse_args(argv)

    base_url = args.ip
    if not base_url.startswith(args.prefix):
        base_url = f"{args.prefix}{base_url}"

    request_params: Tuple[Tuple[str, str], ...] = (
        ("network", "v"),
        ("boards", "v"),
        ("move", "vn"),
        ("heat", "v"),
        ("tools", "v"),
        ("spindles", "v"),
        ("directories", "v"),
        ("fans", "v"),
        ("job", "vn"),
        ("sensors", "v"),
        ("state", "vn"),
        ("volumes", "v"),
        ("", "d99f"),
    )

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)

    # Fetch and write each request
    failures = 0
    for key, flags in request_params:
        try:
            result = requests.get(f"{base_url}/rr_model", params={'key': key, 'flags': flags}).text
        except Exception as e:  # network or API error
            print(f"ERROR: get_model failed for key='{key}' flags='{flags}': {e}", file=sys.stderr)
            failures += 1
            continue

        # Build output filename e.g., model_network_v.json, model_move_vn.json, model_all_d99f.json
        name = key if key else "all"
        filename = out_dir / f"model_{name}_{flags}.json"
        try:
            with open(filename, "w") as f:
                f.write(result)
        except Exception as e:
            print(f"ERROR: failed to write {filename}: {e}", file=sys.stderr)
            failures += 1

    print(f"Done. Wrote {(len(request_params) - failures)} files to {out_dir}")
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
