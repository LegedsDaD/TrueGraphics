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
