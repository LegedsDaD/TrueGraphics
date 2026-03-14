import truegraphics as tg
import random

app = tg.App()
app.set_title("TrueGraphics Mini Desktop")
app.set_size(900, 600)

counter = tg.StateInt(0)

desktop_status = None
counter_label = None


@app.window
def main():
    global desktop_status, counter_label

    with tg.Absolute():

        # Desktop Title
        title = tg.Label("TrueGraphics Desktop Environment")
        title.set_position(20, 20)
        title.set_size(400, 40)

        # Status Bar
        desktop_status = tg.Label("Status: Desktop Ready")
        desktop_status.set_position(20, 520)
        desktop_status.set_size(500, 30)

        # Canvas (acts like desktop wallpaper area)
        desktop_canvas = tg.Canvas()
        desktop_canvas.set_position(300, 80)
        desktop_canvas.set_size(560, 400)
        desktop_canvas.draw_rect(0, 0, 560, 400, "#111827", radius=18, filled=True)

        # App Window Label
        window_label = tg.Label("Counter App Window")
        window_label.set_position(40, 80)
        window_label.set_size(200, 30)

        # Counter label
        counter_label = tg.Label(f"Counter: {counter.get()}")
        counter_label.set_position(40, 120)
        counter_label.set_size(200, 30)

        def update_counter(value: int):
            counter_label.set_text(f"Counter: {value}")

        counter.subscribe(update_counter)

        # Button actions
        def increment():
            counter.set(counter.get() + 1)
            desktop_status.set_text("Status: Increment clicked")

        def decrement():
            counter.set(counter.get() - 1)
            desktop_status.set_text("Status: Decrement clicked")

        def reset():
            counter.set(0)
            desktop_status.set_text("Status: Counter reset")

        def randomize():
            counter.set(random.randint(0, 100))
            desktop_status.set_text("Status: Random value generated")

        # Large visible buttons
        btn_inc = tg.Button("Increment", on_click=increment)
        btn_inc.set_position(40, 170)
        btn_inc.set_size(200, 50)

        btn_dec = tg.Button("Decrement", on_click=decrement)
        btn_dec.set_position(40, 240)
        btn_dec.set_size(200, 50)

        btn_reset = tg.Button("Reset Counter", on_click=reset)
        btn_reset.set_position(40, 310)
        btn_reset.set_size(200, 50)

        btn_rand = tg.Button("Random Value", on_click=randomize)
        btn_rand.set_position(40, 380)
        btn_rand.set_size(200, 50)


app.run()
