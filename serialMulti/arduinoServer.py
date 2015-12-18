# Flask server code - can set to print posted text or read from news/outlet
import serial
import json
import os
from flask import Flask, request

# Serial socket for Arduino interface
ser = serial.Serial('/dev/ttyACM0', 115200)
# TODO: Build class to wrap writing to arduino. Bit messy now
ser.write('t\n')
ser.write('Server Up and Ready for Messages\n')

# Flask app
app = Flask(__name__)
scriptPath = os.path.abspath(__file__)


def form_or_json(thisRequest):
    data = json.loads(thisRequest.data)
    return data if data is not None else thisRequest.form


@app.route("/")
def hello():
    return "Hello World!"


@app.route("/print_text", methods=['POST'])
def print_text():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    ser.write('t\n')
    ser.write(message+'\n')
    return ser.readline()


# Temp - needs profiling. Sending too many lines seems to
# jam up serial connection and make Arduino unresponsive.
# Need to check up on this on both sides
@app.route("/show_image", methods=['POST'])
def show_image():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    ser.write('i\n')
    ser.write(message+'\n')
    return ser.readline()


@app.route("/change_fgcolor", methods=['POST'])
def change_fg():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    ser.write('f\n')
    ser.write(message+'\n')
    return ser.readline()


@app.route("/change_bgcolor", methods=['POST'])
def change_bg():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    ser.write('b\n')
    ser.write(message+'\n')
    return ser.readline()

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
