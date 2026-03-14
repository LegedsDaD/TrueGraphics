import truegraphics as tg

app = tg.App()
app.set_title("Canvas Demo")

@app.window
def main():
    tg.Label("Canvas demo")
    canvas = tg.Canvas()
    canvas.set_size(520, 300)
    canvas.draw_line(20, 20, 240, 40, "#3A7AFE", 3)
    canvas.draw_rect(20, 60, 160, 80, "#22C55E", radius=12, filled=True)
    canvas.draw_rect(200, 60, 160, 80, "#FFFFFF", radius=12, filled=False, thickness=2)
    canvas.draw_circle(120, 200, 40, "#F97316", filled=True)
    canvas.draw_circle(260, 200, 40, "#E11D48", filled=False, thickness=3)
    canvas.draw_text(20, 260, "Hello Canvas", "#E5E7EB")

app.run()
