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
