# Multimodal map inference setup

Provision these gitignored assets before running Finch2 (offline; tiles are never downloaded at runtime):

```text
models/Qwen3VL-8B-Instruct-Q4_K_M.gguf
models/mmproj-Qwen3VL-8B-Instruct-F16.gguf
data/maps/output_hh.tif
data/maps/tiles/osm_de/{z}/{x}/{y}.png
```

Gemma alternative (rename/replace the defaults in `runtime_config.cpp`):
`gemma-4-E4B-it-Q4_K_M.gguf` + `mmproj-F16.gguf`.

```bash
uv sync --project tools/map_renderer
```

Dummy drone uses Bern coverage (`46.982426`, `7.431551`). Artifacts land in `inference_runs/<id>/` (`prompt.txt`, `map.png`, …).
