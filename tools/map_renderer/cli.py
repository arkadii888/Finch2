#!/usr/bin/env python3
"""Command-line entry point for offline drone map rendering."""

from __future__ import annotations

import argparse
import sys
from collections.abc import Sequence
from pathlib import Path

from render import render_snapshot


def main(argv: Sequence[str] | None = None) -> int:
    """Render one map image for multimodal inference."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--lat", required=True, type=float)
    parser.add_argument("--lon", required=True, type=float)
    parser.add_argument("--dem", required=True, type=Path)
    parser.add_argument("--tiles", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--half-window-m", default=1_000.0, type=float)
    parser.add_argument("--zoom", default=15, type=int)
    arguments = parser.parse_args(argv)

    try:
        image = render_snapshot(
            latitude_deg=arguments.lat,
            longitude_deg=arguments.lon,
            half_window_m=arguments.half_window_m,
            dem_path=arguments.dem,
            cache_dir=arguments.tiles,
            zoom=arguments.zoom,
        )
        arguments.output.parent.mkdir(parents=True, exist_ok=True)
        image.save(arguments.output, format="PNG")
        return 0
    except (OSError, TypeError, ValueError) as error:
        print(f"Map rendering failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
