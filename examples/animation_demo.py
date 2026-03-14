import truegraphics as tg

app = tg.App()
app.set_title("Animation Demo")

@app.window
def main():
    label = tg.Label("Click the button to animate it")
    label.set_margin(6)

    btn = tg.Button("Animate")
    btn.set_margin(6)
    btn.on_hover(lambda h: btn.set_background("#3A7AFE") if h else btn.set_background("#374151"))

    def go():
        btn.animate("x", 320, duration=0.45)

    btn2 = tg.Button("Go", on_click=go)
    btn2.set_margin(6)

app.run()
