from __future__ import annotations

import os
import time

import truegraphics as tg


def _assets_dir() -> str:
    return os.path.join(os.path.dirname(__file__), "assets")


def _maybe_load_demo_font() -> None:
    font_path = os.path.join(_assets_dir(), "DemoFont.ttf")
    if os.path.exists(font_path):
        tg.load_font(font_path)


app = tg.App()
app.set_title("TrueGraphics Showcase")
app.set_size(1100, 720)
app.set_theme("dark")

tg.add_resource_path(_assets_dir())
_maybe_load_demo_font()

app.enable_tray("app.ico", "TrueGraphics", on_click=lambda: tg.message_box("Tray clicked", "TrueGraphics"))


@app.window
def main() -> None:
    status: tg.Label | None = None
    tabs: tg.Tabs | None = None

    def set_status(text: str) -> None:
        if status is not None:
            status.set_text(text)

    with tg.Column() as root:
        root.set_gap(12)
        root.set_padding(12)

        with tg.Row() as header:
            header.set_gap(12)
            header.set_align(tg.Align.Center)

            title = tg.Label("TrueGraphics Showcase")
            title.set_font_size(22)
            title.set_text_color("#FFFFFF")

            status = tg.Label("Ready")
            status.set_text_color("#A3A3A3")
            status.set_size(520, 28)

        tabs = tg.Tabs()
        tabs.set_size(1060, 610)

    app.on_shortcut("Ctrl+S", lambda: tg.message_box("Ctrl+S pressed", "Shortcut"))
    app.on_shortcut("Esc", lambda: tg.message_box("Esc pressed", "Shortcut"))

    if tabs is None:
        return

    # --- Widgets
    with tabs.tab("Widgets") as page:
        page.set_gap(12)
        page.set_padding(12)

        counter = tg.StateInt(0)
        counter_label = tg.Label("Count: 0")
        counter_label.set_text_color("#E5E7EB")

        def on_counter(v: int) -> None:
            counter_label.set_text(f"Count: {v}")

        counter.subscribe(on_counter)

        with tg.Row() as row:
            row.set_gap(12)
            row.set_align(tg.Align.Center)

            inc = tg.Button("Increment", on_click=lambda: counter.set(counter.get() + 1))
            inc.set_background("#3A7AFE")
            inc.set_text_color("#FFFFFF")
            inc.set_border_radius(10)

            tg.Button("MessageBox", on_click=lambda: tg.message_box("Hello!", "TrueGraphics")).set_border_radius(10)

            dd = tg.Dropdown(["Low", "Medium", "High"], selected=1, on_change=lambda i: set_status(f"Dropdown index: {i}"))
            dd.set_size(220, 42)

        tg.Checkbox("Enable feature", checked=True, on_change=lambda v: set_status(f"Checkbox: {v}"))
        tg.RadioButton("Option A", group="mode", checked=True, on_change=lambda v: set_status(f"Radio A: {v}"))
        tg.RadioButton("Option B", group="mode", checked=False, on_change=lambda v: set_status(f"Radio B: {v}"))

        slider = tg.Slider(0, 100, 25, on_change=lambda v: set_status(f"Slider: {v:.0f}"))
        slider.set_size(420, 36)

        bar = tg.ProgressBar(0.25)
        bar.set_size(420, 20)

        def sync_progress() -> None:
            bar.value = max(0.0, min(1.0, slider.value / 100.0))

        slider.on_mouse_up(lambda _x, _y: sync_progress())

        with tg.Absolute() as anim:
            anim.set_size(520, 70)
            anim.set_background("#111827")
            anim.set_border_radius(12)
            anim.set_border_width(1)

            def animate_once() -> None:
                moving.animate("x", 320, duration=0.6, easing="smoothstep")
                app.set_timeout(0.7, lambda: moving.animate("x", 12, duration=0.6, easing="smoothstep"))

            moving = tg.Button("Animate me", on_click=animate_once)
            moving.set_position(12, 14)
            moving.set_size(160, 42)
            moving.set_border_radius(10)
            tg.Label("Click the button").set_position(190, 22)

    # --- Text / Input
    with tabs.tab("Text / Input") as page:
        page.set_gap(12)
        page.set_padding(12)

        tg.Label("Tab / Shift+Tab: focus traversal (best-effort).").set_text_color("#A3A3A3")
        tg.Label("TextBox/TextArea/PasswordBox support basic caret + selection + Ctrl+A/C/X/V.").set_text_color("#A3A3A3")

        tb = tg.TextBox("Edit me")
        tb.set_size(520, 42)

        pw = tg.PasswordBox("secret")
        pw.set_size(520, 42)

        ta = tg.TextArea("TextArea supports basic multi-line editing.\nArrow keys move caret (approx).")
        ta.set_size(520, 220)

    # --- Data Views
    with tabs.tab("Data Views") as page:
        page.set_gap(12)
        page.set_padding(12)

        with tg.Row() as row:
            row.set_gap(12)

            lv = tg.ListView(["Alpha", "Beta", "Gamma"], on_select=lambda i: set_status(f"ListView index: {i}"))
            lv.set_size(320, 180)

            table = tg.Table(["Name", "Value"], [["A", "1"], ["B", "2"], ["C", "3"]])
            table.set_size(520, 220)

        tree = tg.TreeView([("Root", 0), ("Child 1", 1), ("Child 2", 1), ("Leaf", 2)], on_select=lambda i: set_status(f"TreeView index: {i}"))
        tree.set_size(520, 220)

    # --- Canvas / Image / Scroll
    with tabs.tab("Canvas + Scroll") as page:
        page.set_gap(12)
        page.set_padding(12)

        canvas = tg.Canvas()
        canvas.set_size(900, 380)
        canvas.draw_line(20, 20, 300, 50, "#3A7AFE", 3)
        canvas.draw_rect(20, 80, 240, 120, "#22C55E", radius=14, filled=True)
        canvas.draw_rect(280, 80, 240, 120, "#E11D48", radius=14, filled=False, thickness=3)
        canvas.draw_circle(660, 140, 54, "#F97316", filled=True)
        canvas.draw_text(20, 230, "Canvas: line/rect/circle/text", "#E5E7EB")

        img_path = tg.resolve_resource("logo.png")
        if os.path.exists(img_path):
            tg.Image("logo.png").set_size(220, 120)
        else:
            tg.Label("Put examples/assets/logo.png to demo tg.Image(...)").set_text_color("#A3A3A3")

        scroll = tg.ScrollView()
        scroll.set_size(900, 140)
        with scroll:
            for i in range(30):
                w = tg.Label(f"Scrollable row {i}")
                w.set_position(12, 12 + i * 28)

        tg.Label("Shift + mouse wheel: horizontal scroll inside ScrollView (when content is wider).").set_text_color("#A3A3A3")

    # --- Platform (resources, dialogs, async, drag-drop)
    with tabs.tab("Platform") as page:
        page.set_gap(12)
        page.set_padding(12)

        drag = tg.Label("Drop files here (WM_DROPFILES)")
        drag.set_background("#111827")
        drag.set_border_radius(12)
        drag.set_padding(12)
        drag.set_size(900, 90)
        drag.on_drop_files(lambda files: drag.set_text("Dropped:\n" + "\n".join(files)))

        with tg.Row() as row:
            row.set_gap(12)
            tg.Button("Open File", on_click=lambda: set_status(f"open_file_dialog: {tg.open_file_dialog()}")).set_border_radius(10)
            tg.Button("Save File", on_click=lambda: set_status(f"save_file_dialog: {tg.save_file_dialog()}")).set_border_radius(10)
            tg.Button("Copy Time", on_click=lambda: tg.clipboard_set_text(time.strftime("%H:%M:%S"))).set_border_radius(10)
            tg.Button("Paste", on_click=lambda: set_status(f"Clipboard: {tg.clipboard_get_text()}")).set_border_radius(10)

        def run_background() -> None:
            set_status("Background task running...")

            def task() -> None:
                time.sleep(1.5)

            def done() -> None:
                set_status("Background task done")

            app.run_async(task, done)

        tg.Button("Run Async Task", on_click=run_background).set_border_radius(10)

    def tick() -> None:
        status.set_text(time.strftime("Time: %H:%M:%S"))

    app.set_interval(1.0, tick)


def inspector_window() -> None:
    with tg.Column() as col:
        col.set_gap(10)
        col.set_padding(12)

        tg.Label("Inspector Window").set_font_size(18)
        tg.Label("Created via app.add_window(...)").set_text_color("#A3A3A3")

        info = tg.Label("Clipboard: (press Paste in main Platform tab)")
        info.set_size(380, 60)
        info.set_background("#111827")
        info.set_border_radius(12)
        info.set_padding(10)

        tg.Button("Show Clipboard", on_click=lambda: info.set_text("Clipboard:\n" + tg.clipboard_get_text())).set_border_radius(10)
        tg.Button("MessageBox", on_click=lambda: tg.message_box("Hello from second window", "TrueGraphics")).set_border_radius(10)


app.add_window(inspector_window, title="Inspector", width=440, height=320)

if __name__ == "__main__":
    app.run()
