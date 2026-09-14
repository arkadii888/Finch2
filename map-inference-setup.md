# Multimodal map inference setup

Provision these gitignored assets before running Finch2. Maps are rendered
offline from a local GeoPackage and DEM; tiles are never downloaded at runtime.

```text
models/qwen3-vl-8b-instruct.Q4_K_M.gguf
models/qwen3-vl-8b-instruct.BF16-mmproj.gguf
data/maps/switzerland.tif
data/maps/switzerland.gpkg
```

Use the matching vision-tuned BF16 projector exported with the fine-tuned
`v3_mix_from_base` GGUF. Do not pair that GGUF with the base `mmproj-F16.gguf`.

```bash
uv sync --project tools/map_renderer
```

The model emits flight-only pixel `go_to` coordinates: integers `x,y` in
`[0, 1000]`, origin at the top-left, x right / east, y down / south, map center
near `(500, 500)`. Finch2 converts those points to WGS84 using the rendered
`±1000 m` window before executing `takeoff`, `go_to`, `rtl`, and `land`.
Missions are limited to 26 waypoints. Checkpoint 350 is marked
`deployable: false`; this runtime wiring is not a flight-readiness decision.

Dummy drone uses Bern coverage (`46.982426`, `7.431551`). Each request writes
`inference_runs/<id>/` with `prompt.txt`, `map.png`, `raw_response.txt`, and
materialized `btree.json`.
