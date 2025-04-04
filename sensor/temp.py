from flask import Flask, request, jsonify
from flask_cors import CORS
import firebase_admin
from firebase_admin import db, credentials

app = Flask(__name__)
CORS(app)

# Initialize Firebase Admin SDK with your credentials
cred = credentials.Certificate("./qr-reader-74e94-firebase-adminsdk-u67ki-373103a3e0.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://qr-reader-74e94-default-rtdb.firebaseio.com/'
})

sensor_data = {"turbidity": 0, "tds": 0}

@app.route('/update', methods=['POST'])
def update_data():
    global sensor_data
    data = request.json
    if "turbidity" in data and "tds" in data:
        new_data = {
            'turbidity': data['turbidity'],
            'tds': data['tds']
        }
        ref = db.reference('/data')
        ref.set(new_data)
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
