from arduino.app_utils import App, Bridge
import time

class Minuteur:
    def __init__(self):
        self.duree_s = 0
        self.running = False
        self.paused = False
        self.done = False
        self._start_t = 0.0
        self._paused_elapsed = 0.0

    def launch(self, duree_s):
        self.duree_s = int(duree_s)
        self._start_t = time.monotonic()
        self._paused_elapsed = 0.0
        self.running = True
        self.paused = False
        self.done = False

    def stop(self):
        self.running = False
        self.paused = False
        self.done = False
        self._paused_elapsed = 0.0
        self._start_t = 0.0
        self.duree_s = 0

    def update(self):
        if not self.running:
            return False
        if (time.monotonic() - self._start_t) >= self.duree_s:
            self.done = True
            self.running = False
            return True
        return False

    def elapsed(self):
        if self.running:
            return time.monotonic() - self._start_t
        if self.paused:
            return self._paused_elapsed
        if self.done:
            return float(self.duree_s)
        return 0.0

    def remaining(self):
        if self.running:
            return max(0.0, self.duree_s - self.elapsed())
        if self.paused:
            return max(0.0, self.duree_s - self._paused_elapsed)
        if self.done:
            return 0.0
        return float(self.duree_s)

timer = Minuteur()
last_sent = None

def api_temps(secondes):
    timer.launch(secondes)

def stop_timer():
    timer.stop()

Bridge.provide("api_temps", api_temps)
Bridge.provide("stop_timer", stop_timer)

def loop():
    global last_sent #tuple

    if timer.running:
        if timer.update():
            print("Timer terminé")
            Bridge.call("minuteur", int(timer.elapsed()), int(timer.remaining()))
            Bridge.call("fin")
            last_sent = (int(timer.elapsed()), int(timer.remaining()))
            return

        ecoule = int(timer.elapsed())
        restant = int(timer.remaining())
        current = (ecoule, restant)

        if current != last_sent:
            print(f"Ecoulé: {ecoule}s | Restant: {restant}s")
            Bridge.call("minuteur", ecoule, restant)
            last_sent = current

    time.sleep(0.2)

App.run(user_loop=loop)
