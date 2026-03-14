# TrueGraphics

TrueGraphics is a small Windows-only GUI experiment: a C++ core (Win32 + GDI) with Python bindings built using `pybind11`.

## Overview

- Windows-only Win32 window + message loop
- Lightweight GDI renderer (double-buffered)
- Python-first authoring model (`@app.window` builder function)
- Nested UI via container context managers (`with tg.Row(): ...`)
- Basic layouts: `Row`, `Column`, `Grid`, `Absolute`
- Basic styling setters on widgets (colors, radius, borders, padding/margin)
- Canvas drawing API (`draw_line/rect/circle/text`)
- Image widget (`tg.Image("path")`) via GDI+
- Animations (`widget.animate(...)`), timers (`app.set_timeout/set_interval`), and background tasks (`app.run_async`)
- Scrolling: `tg.ScrollView()` + mouse wheel
- Multi-window apps via `app.add_window(...)`
- System tray icon via `app.enable_tray(...)`
- Resources: `tg.add_resource_path(...)`, `tg.resolve_resource(...)`, `tg.load_font(...)`
- Widgets: `Button`, `Label`, `TextBox`, `PasswordBox`, `TextArea`, `Checkbox`, `RadioButton`, `Slider`, `ProgressBar`, `Dropdown`, `Tabs`, `ListView`, `Table`, `TreeView`, `Container`, `ScrollView`, `Canvas`, `Image`
- State: `StateInt`, `StateFloat`, `StateBool`, `StateString` (all with `subscribe(...)`)
- Buildable with `pip install .` (requires a C++ toolchain)

## Installation

```bash
pip install .
```

Requirements (Windows):
- Python 3.9+
- CMake 3.20+
- A C++ toolchain: MSVC (Visual Studio Build Tools) or MinGW-w64 (MSYS2)

MinGW note:
- MinGW builds work, but image support requires `gdiplus` (linked automatically via CMake when using a toolchain that provides it).

## Example Usage

Minimal app:
```python
import truegraphics as tg

app = tg.App()

@app.window
def main():
    btn = tg.Button("Hello World")
    btn.set_background("#3A7AFE")
    btn.set_text_color("#FFFFFF")

app.run()
```

Layout example:
```python
import truegraphics as tg

app = tg.App()
app.set_title("Row / Grid Demo")

@app.window
def main():
    with tg.Row() as row:
        row.set_gap(12)
        row.set_align(tg.Align.Center)
        tg.Button("OK")
        tg.Button("Cancel")

    with tg.Grid(2) as grid:
        grid.set_gap(10)
        tg.TextBox("Type here...")
        tg.Checkbox("Enable feature", checked=True)

app.run()
```

Counter app (manual UI update via `StateInt.subscribe`):
```python
import truegraphics as tg

app = tg.App()
counter = tg.StateInt(0)
label = None

@app.window
def main():
    global label

    def update_label(value: int):
        if label is not None:
            label.set_text(f"Count: {value}")

    counter.subscribe(update_label)

    tg.Button("Increment", on_click=lambda: counter.set(counter.get() + 1))
    label = tg.Label(f"Count: {counter.get()}")

app.run()
```

Canvas example:
```python
import truegraphics as tg

app = tg.App()

@app.window
def main():
    canvas = tg.Canvas()
    canvas.set_size(520, 300)
    canvas.draw_line(20, 20, 240, 40, "#3A7AFE", 3)
    canvas.draw_rect(20, 60, 160, 80, "#22C55E", radius=12, filled=True)
    canvas.draw_circle(120, 200, 40, "#F97316", filled=True)
    canvas.draw_text(20, 260, "Hello Canvas", "#E5E7EB")

app.run()
```

Image example:
```python
import truegraphics as tg

app = tg.App()
app.set_title("Image Demo")

@app.window
def main():
    img = tg.Image("assets/logo.png")
    img.set_size(320, 180)

app.run()
```

Multi-window example:
```python
import truegraphics as tg

app = tg.App()
app.set_title("Main window")

@app.window
def main():
    tg.Label("Main")

def second():
    tg.Label("Second window")

app.add_window(second, title="Second", width=420, height=220)
app.run()
```

System tray example:
```python
import truegraphics as tg

app = tg.App()
app.enable_tray("assets/app.ico", "TrueGraphics", on_click=lambda: tg.message_box("Tray clicked", "Tray"))

@app.window
def main():
    tg.Label("Running (see tray icon)")

app.run()
```

## Showcase Example

Run the full showcase (widgets, layouts, canvas, events, async, drag-drop, multi-window, tray):

```bash
python examples/showcase.py
```

## Current Limitations (important)

- Windows only (Win32 + GDI).
- Rendering is GDI-based (no GPU backend yet).
- Text rendering/editing is basic (no IME, no word-wrapping; caret/selection are approximate).
- Clipboard is text-only for now (CF_TEXT).
- Drag-and-drop supports file drops (WM_DROPFILES) only.
- Resource management is intentionally small: search paths + font file loading.
- System tray support is minimal (one tray icon per window; basic right-click menu).
- Not all “standard toolkit widgets” exist yet (this is still early-stage).

## Known Gaps

- No IME / complex text shaping.
- No layout-driven text measurement or wrapping.
- No accessibility and no cross-platform backend.

## Architecture Overview

- `engine/include/truegraphics`: public engine headers
- `engine/src`: engine implementation
- `bindings/module.cpp`: pybind11 module
- `python/truegraphics`: Python package surface
- `examples`: runnable examples

## Notes

This version targets Windows and uses an internal Win32-backed event loop with an engine-side scene/widget model. Rendering is implemented as a lightweight internal renderer abstraction that currently uses GDI-based drawing while preserving the C++ engine architecture for future GPU backend expansion.

## Releases (Wheels + PyPI Trusted Publishing)

This repo includes GitHub Actions workflows to build Windows wheels and publish them to PyPI using PyPI “Trusted Publishing” (OIDC):

- CI wheel builds: `.github/workflows/ci.yml`
- Publish on tags `v*`: `.github/workflows/publish.yml`
