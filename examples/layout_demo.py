import truegraphics as tg

app = tg.App()
app.set_title("Layout Demo")

@app.window
def main():
    tg.Label("TrueGraphics Layout Demo").set_margin(6)

    with tg.Row() as row:
        row.set_gap(12)
        row.set_align(tg.Align.Center)
        tg.Button("OK").set_margin(2)
        tg.Button("Cancel").set_margin(2)

    scroll = tg.ScrollView()
    scroll.set_size(520, 220)
    scroll.set_margin(6)

    with scroll:
        with tg.Grid(2) as grid:
            grid.set_gap(10)
            tg.TextBox("Type here...").set_margin(4)
            tg.Checkbox("Enable feature", checked=True).set_margin(4)
            tg.Button("Apply").set_margin(4)
            tg.Button("Reset").set_margin(4)

        # Add enough content to demonstrate scrolling
        for i in range(12):
            tg.Label(f"Item {i+1}").set_margin(4)

app.run()
