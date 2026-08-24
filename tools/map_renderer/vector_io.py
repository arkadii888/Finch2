"""GeoPackage layer discovery and bbox queries."""

from pathlib import Path

import pyogrio
import shapely

from map_style import VECTOR_LAYER_NEEDLES


def vector_source(path: Path) -> str:
    resolved = path.expanduser().resolve()
    if not resolved.is_file():
        raise FileNotFoundError(f"GeoPackage not found: {path}")
    suffixes = "".join(resolved.suffixes).lower()
    if suffixes.endswith(".zip"):
        return f"/vsizip/{resolved}"
    return str(resolved)


def list_vector_layers(source: str) -> list[str]:
    listed = pyogrio.list_layers(source)
    if listed is None or len(listed) == 0:
        return []
    return [str(row[0]) for row in listed]


def choose_layer(layers: list[str], needles: tuple[str, ...]) -> str | None:
    for needle in needles:
        match = next((name for name in layers if name.lower().endswith(needle)), None)
        if match is not None:
            return match
    return None


def read_layer(
    source: str,
    layer: str,
    bounds: tuple[float, float, float, float],
    columns: tuple[str, ...] = (),
):
    meta, _, geometry, field_data = pyogrio.raw.read(
        source,
        layer=layer,
        bbox=bounds,
        columns=list(columns) or None,
    )
    if geometry is None or len(geometry) == 0:
        return None, {}
    geoms = shapely.from_wkb(geometry, on_invalid="ignore")
    names = [str(name) for name in meta.get("fields", ())]
    fields = {name: field_data[index] for index, name in enumerate(names)}
    return geoms, fields


def features(
    source: str,
    layers: list[str],
    key: str,
    bounds: tuple[float, float, float, float],
    columns: tuple[str, ...] = (),
):
    layer = choose_layer(layers, VECTOR_LAYER_NEEDLES[key])
    if layer is None:
        return None, {}
    return read_layer(source, layer, bounds, columns)


def where(geoms, classes, wanted) -> list:
    names = {wanted} if isinstance(wanted, str) else wanted
    return [
        geom for geom, raw_class in zip(geoms, classes, strict=False) if str(raw_class) in names
    ]
