# Flask server code - can set to print posted text or read from news/outlet
import serial
import signal
import json
import os
from flask import Flask, request


# modified http://stackoverflow.com/questions/2281850/
# timeout-function-if-it-takes-too-long-to-finish
# will only work in unix systems
class timeout:

    def __init__(self, seconds=1, error_message='Timeout'):
        self.seconds = seconds
        self.error_message = error_message

    def handle_timeout(self, signum, frame):
        raise RuntimeError(self.error_message)

    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.seconds)

    def __exit__(self, type, value, traceback):
        signal.alarm(0)


# TODO: input sanitization
class InoSerial:
    '''Class for wrapping serial control, intended to abstract
    away overhead (at time of writing, mostly pre-sending data types
    waiting for a reply, and then returning the reply) '''

    def __init__(self, device, baudrate):
        self.device = device
        self.baudrate = baudrate
        self.ser = serial.Serial(device, baudrate)
        if self.listen_nl() == "Ready\n":
            pass
        else:
            raise RuntimeError('Initialize message not received.\
                Try resetting the serial connection?')

    def listen_nl(self, wait_sec=5):
        with timeout(seconds=wait_sec):
            reply = ''
            while True:
                bytesToRead = self.ser.inWaiting()
                reply += self.ser.read(bytesToRead)
                if reply[-1] == "\n":
                    return reply

    def push(self, msg):
        # add newline
        msg = "{0}\n".format(msg)
        return self.listen_nl()

    # TODO: timeout ability (handle by resetting connection?)
    def push_wait(self, msg, til):
        # add newlines
        msg = "{0}\n".format(msg)
        til = "{0}\n".format(til)
        self.ser.write(msg)
        if self.listen_nl() == til:
            return 0

    def send_text(self, text, fg=None, bg=None):
        if fg is not None:
            self.ch_fgcolor(fg)
        if bg is not None:
            self.ch_bgcolor(bg)
        self.push_wait('t', 't')
        text_reply = self.push(text+'\n')
        # TODO (dpdt on queryability in .ino) - set to original color
        return text_reply

    # Temp - needs profiling. Sending too many lines seems to
    # jam up serial connection and make Arduino unresponsive.
    # Need to check up on this on both sides
    def show_img(self, img_str):
        self.push_wait('i', 'i')
        return self.push(img_str)

    def ch_fgcolor(self, hexcolor):
        self.push_wait('f', 'f')
        return self.push(hexcolor)

    def ch_bgcolor(self, hexcolor):
        self.push_wait('b', 'b')
        return self.push(hexcolor)

    def ch_brightness(self, brightness):
        self.push_wait('l', 'l')
        return self.push(brightness)

    def ch_txthold(self, holdtime):
        self.push_wait('s', 's')
        return self.push(holdtime)

# initiate Arduino interface
ino = InoSerial('/dev/ttyACM0', 115200)
ino.send_text('Server Up and Ready for Messages')

# Flask app
app = Flask(__name__)
scriptPath = os.path.abspath(__file__)


def form_or_json(thisRequest):
    data = json.loads(thisRequest.data)
    return data if data is not None else thisRequest.form


@app.route("/")
def hello():
    return "Hello World!"


# TODO: replace all serial calls with object wrapped
@app.route("/print_text", methods=['POST'])
def print_text():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.send_text(message)


@app.route("/show_image", methods=['POST'])
def show_image():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.show_img(message)


@app.route("/change_bright", methods=['POST'])
def change_bright():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.ch_brightness(message)


@app.route("/change_text_delay", methods=['POST'])
def change_text_delay():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.ch_txthold(message)


@app.route("/change_fgcolor", methods=['POST'])
def change_fg():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.ch_fgcolor(message)


@app.route("/change_bgcolor", methods=['POST'])
def change_bg():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ino.ch_bgcolor(message)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
