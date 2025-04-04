from flask import Flask, jsonify, render_template, request
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index2.html')

sensor_data = {"turbidity": 0, "tds": 0}

@app.route('/update', methods=['POST'])
def update_data():
    global sensor_data
    data = request.json
    if "turbidity" in data and "tds" in data:
        sensor_data["turbidity"] = data["turbidity"]
        sensor_data["tds"] = data["tds"]
        print(f"Updated Data: {sensor_data}")
        return jsonify({"status": "success"}), 200
    return jsonify({"status": "error", "message": "Invalid data"}), 400

@app.route('/data', methods=['GET'])
def get_data():
    return jsonify(sensor_data)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
