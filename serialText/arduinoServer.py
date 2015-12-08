# Flask server code - can set to print posted text or read from news/outlet
import serial
import json
import os
from flask import Flask, request

# Serial socket for Arduino interface
ser = serial.Serial('/dev/ttyACM0', 9600)
ser.write('Server Up and Ready for Messages \n')

# Flask app
app = Flask(__name__)
scriptPath = os.path.abspath(__file__)


def form_or_json(thisRequest):
    data = json.loads(thisRequest.data)
    return data if data is not None else thisRequest.form


@app.route("/")
def hello():
    return "Hello World!"


@app.route("/show", methods=['POST'])
def show():
    requestData = form_or_json(request)
    print requestData
    message = requestData["message"]
    return ser.write(message+'\n')

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80)
