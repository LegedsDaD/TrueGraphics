# TrueGraphics Manual

Showcase app (recommended): run `python examples/showcase.py`.

## 1. API Documentation

### `truegraphics.App`
Application object responsible for window lifecycle, widget construction scope, and event loop startup.

Methods:
- `window(callable)` register the root builder (used as a decorator via `@app.window`)
- `add_window(callable, title="TrueGraphics", width=640, height=480) -> int` register an additional window builder (created when `run()` is called)
- `run()` build the widget tree and start the Win32 message loop
- `set_title(str)` set the initial window title (call before `run()`)
- `set_size(width, height)` set the initial window size (call before `run()`)
- `set_theme("dark" | "light")` set theme used for default widget styles
- `set_fullscreen(bool)` request fullscreen at startup
- `set_icon(path: str)` set window icon (best with `.ico`)
- `set_min_size(w, h)` set min track size
- `set_max_size(w, h)` set max track size
- `set_timeout(seconds, callback) -> int` run callback once after delay
- `set_interval(seconds, callback) -> int` run callback repeatedly
- `clear_timer(id)` cancel a timeout/interval
- `post(callback)` schedule a callback to run on the UI thread on the next frame
- `run_async(task, on_done=None)` run a background task and optionally schedule `on_done` back on the UI thread
- `on_shortcut("Ctrl+S", callback)` register a simple shortcut handler
- `enable_tray(icon_path, tooltip="TrueGraphics", on_click=None)` enable a system tray icon (best with `.ico`)
- `disable_tray()` remove tray icon

Notes:
- The window is resizable by default; container layouts are re-applied on resize.
- The renderer redraws continuously (timer-driven) and is double-buffered to avoid flicker.

### `truegraphics.Widget`
Base bound widget object.

Common methods:
- `set_id(str)`
- `set_text(str)`
- `set_size(width, height)`
- `set_position(x, y)`
- `set_background(color)`
- `set_text_color(color)`
- `set_border_color(color)`
- `set_border_radius(int)`
- `set_border_width(int)`
- `set_padding(int)`
- `set_margin(int)`
- `set_font(family: str)`
- `set_font_size(size: int)`
- `set_focusable(bool)`
- `on_hover(callback(bool))`
- `on_mouse_move(callback(x, y))`
- `on_key_down(callback(key_code))`
- `on_char(callback(codepoint))`
- `on_drop_files(callback(list[str]))`
- `animate(property, to, duration=0.3, easing="smoothstep")`
- `on_mouse_down(callback(x, y))`
- `on_mouse_up(callback(x, y))`

Properties:
- `text: str` (read-only)
- `id: str` (read-only)
- `x: int` (read-only)
- `y: int` (read-only)
- `width: int` (read-only)
- `height: int` (read-only)

### `truegraphics.Button`
Construct a button widget.

Signature:
```python
Button(text: str, on_click: Optional[Callable] = None)
```

### `truegraphics.Label`
Construct a label widget.

Signature:
```python
Label(text: str = "")
```

### `truegraphics.TextBox`
Basic text input widget (ASCII for now). Becomes focused when clicked.

Signature:
```python
TextBox(text: str = "")
```

### `truegraphics.Checkbox`
Clickable checkbox with an `on_change` callback.

Signature:
```python
Checkbox(text: str = "", checked: bool = False, on_change: Optional[Callable[[bool], None]] = None)
```

### `truegraphics.Container`
Construct a container widget.

Containers can be used as context managers to build nested UI:

```python
with tg.Container():
    tg.Button("Child")
```

Container layout configuration:
- `set_layout(tg.LayoutMode.Column | Row | Grid | None)`
- `set_align(tg.Align.Start | Center | End | Stretch)`
- `set_justify(tg.Justify.Start | Center | End | SpaceBetween)`
- `set_gap(int)`
- `set_grid_columns(int)` (for grids)

Convenience factories:
- `tg.Row()`
- `tg.Column()`
- `tg.Grid(columns)`
- `tg.Absolute()` (layout disabled; keeps `set_position` placements)

### `truegraphics.ScrollView`
Scrollable container widget. Use it as a context manager to add children, and scroll with the mouse wheel when the cursor is over it.

Signature:
```python
ScrollView()
```

Methods:
- `set_scroll_y(int)`
- `scroll_by(dy: int)`
Properties:
- `scroll_y: int` (read-only)

### `truegraphics.Canvas`
Construct a 2D canvas widget that records draw commands and renders them each frame.

Methods:
- `clear()`
- `draw_line(x1, y1, x2, y2, color="#FFFFFF", thickness=1)`
- `draw_rect(x, y, w, h, color="#FFFFFF", radius=0, filled=True, thickness=1)`
- `draw_circle(cx, cy, radius, color="#FFFFFF", filled=True, thickness=1)`
- `draw_text(x, y, text, color="#FFFFFF")`

### `truegraphics.StateInt`
Reactive integer state container.

Methods:
- `get() -> int`
- `set(value: int) -> None`
- `subscribe(callback: Callable[[int], None]) -> None`

Other state types:
- `StateFloat`
- `StateBool`
- `StateString`

Note: state changes do not automatically rebuild your widget tree; use `subscribe(...)` to update existing widget instances (for example, calling `label.set_text(...)`).

## 2. Engine Architecture

Subsystems:
- Core: configuration, app orchestration, timing, UUID generation
- Window: Win32 window, input events, DPI helpers
- Graphics: internal renderer and GPU abstraction placeholders
- Widgets: widget tree and primitive controls
- Layout: basic flex-like vertical stacking
- Style: theming and visual properties
- Animation: property tween bookkeeping
- State: reactive integer state
- Async: background job runner
- Plugin: dynamic module placeholder
- System: logging and profiler

## 3. Python API Reference

Primary import:
```python
import truegraphics as tg
```

Typical app:
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

## 4. Widget Guide

### Button
Interactive clickable widget with hover-aware style fields and click callback.

### Label
Text display widget.

### Container
Root/group widget for child composition.

### Canvas
Custom drawing placeholder.

## 5. Layout Guide

Layouts are configured by container widgets:

- `tg.Column()` (default for containers): stacks children vertically
- `tg.Row()`: lays out children horizontally
- `tg.Grid(cols)`: lays out children into a fixed column grid
- `tg.Absolute()`: disables layout for that container (children keep their `set_position` values)

Layout applies recursively and re-runs on window resize.

## 6. Animation Guide

The repository contains an animation subsystem (tween bookkeeping), but it is not currently integrated into the Python-facing runtime loop (there is no public Python API for animations yet).

## 7. Styling Guide

Colors can be provided as `"#RRGGBB"` / `"#RRGGBBAA"` or `(r, g, b)` / `(r, g, b, a)` tuples.

Common styling calls:
```python
btn = tg.Button("OK")
btn.set_background("#3A7AFE")
btn.set_text_color("#FFFFFF")
btn.set_border_radius(8)
btn.set_border_width(1)
btn.set_border_color("#1F2937")
btn.set_padding(10)
btn.set_margin(6)
```

## 9. Image Support

`truegraphics.Image` draws an image loaded from disk (PNG/JPG/etc) using Windows GDI+.

Signature:
```python
Image(path: str = "")
```

Methods:
- `set_source(path: str)`

## 10. Animations, Timers, Async

Animations (numeric widget properties):
```python
btn = tg.Button("Move me")
btn.animate("x", 300, duration=0.5)
```
Supported properties: `"x"`, `"y"`, `"width"`, `"height"`.

Timers:
```python
app.set_timeout(1.0, lambda: print("one second"))
timer_id = app.set_interval(0.5, lambda: print("tick"))
app.clear_timer(timer_id)
```

Async:
```python
def work():
    # do slow work here
    pass

app.run_async(work, on_done=lambda: print("done"))
```

## 10.1 Clipboard and Shortcuts

Clipboard helpers:
```python
tg.clipboard_set_text("hello")
print(tg.clipboard_get_text())
```

`TextBox` supports Ctrl+C/Ctrl+X/Ctrl+V (whole-text copy/cut/paste for now).

App shortcuts:
```python
app.on_shortcut("Ctrl+S", lambda: print("save"))
```

## 10.2 Common Widgets (Group 2A)

`Slider(min=0.0, max=1.0, value=0.0, on_change=None)`:
```python
tg.Slider(0, 100, 25, on_change=lambda v: print(v))
```

`ProgressBar(value=0.0)`:
```python
bar = tg.ProgressBar(0.3)
bar.value = 0.7
```

`Dropdown(items=[], selected=0, on_change=None)`:
```python
tg.Dropdown(["Low", "Medium", "High"], selected=1, on_change=lambda i: print(i))
```

`Tabs()` with pages created by `tabs.tab(title)`:
```python
tabs = tg.Tabs()
with tabs.tab("One"):
    tg.Label("Tab one")
with tabs.tab("Two"):
    tg.Label("Tab two")
```

`RadioButton(text="", group="default", checked=False, on_change=None)`:
```python
tg.RadioButton("Option A", group="mode", checked=True)
tg.RadioButton("Option B", group="mode")
```

`TextArea(text="")`:
```python
ta = tg.TextArea("Line 1\nLine 2")
ta.set_size(520, 220)
```

`PasswordBox(text="")`:
```python
tg.PasswordBox("secret")
```

`ListView(items=[], on_select=None)`:
```python
tg.ListView(["One", "Two", "Three"], on_select=lambda i: print("selected", i))
```

`Table(headers=[], rows=[])`:
```python
tg.Table(["Name", "Value"], [["A", "1"], ["B", "2"]])
```

`TreeView(items=[], on_select=None)` where items are `(text, level)` pairs:
```python
items = [("Root", 0), ("Child 1", 1), ("Child 2", 1)]
tg.TreeView(items, on_select=lambda i: print("selected", i))
```

`ScrollView()`:
```python
scroll = tg.ScrollView()
scroll.set_size(520, 240)
with scroll:
    for i in range(30):
        tg.Label(f"Row {i}")
```

Horizontal scroll: hold Shift while scrolling the mouse wheel.

## 10.3 Dialog Helpers

```python
tg.message_box("Hello", "Title")
path = tg.open_file_dialog()
save = tg.save_file_dialog()
```

## 11. Platform Features

## 11.1 Resources and Fonts

Resource paths:
```python
tg.add_resource_path("assets")
print(tg.resolve_resource("logo.png"))
```

Load a font file (process-private) and use it by family name:
```python
tg.load_font("assets/MyFont.ttf")

lbl = tg.Label("Hello")
lbl.set_font("My Font Family Name")
lbl.set_font_size(18)
```

## 11.2 Drag and Drop (files)

Register a file-drop callback on any widget:
```python
label = tg.Label("Drop files here")
label.on_drop_files(lambda files: label.set_text(str(files)))
```

## 11.3 Multi-window and System Tray

Additional windows (registered before `run()`):
```python
app = tg.App()

@app.window
def main():
    tg.Label("Main")

def second():
    tg.Label("Second window")

app.add_window(second, title="Second", width=420, height=220)
app.run()
```

System tray icon:
```python
app = tg.App()
app.enable_tray("assets/app.ico", "TrueGraphics", on_click=lambda: tg.message_box("clicked", "tray"))

@app.window
def main():
    tg.Label("Running")

app.run()
```

## 11.4 Known Gaps

- No IME / complex text shaping.
- Text measurement/wrapping is not layout-aware yet.
- Accessibility is not implemented.

## 8. Extensibility

Plugins and async tasks have dedicated engine modules so the architecture can scale without breaking the public Python API.
