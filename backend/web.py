from flask import Flask, render_template
import requests
import statistics
from dotenv import load_dotenv
import os

load_dotenv()
app = Flask(__name__)


SHEETS_KEY = os.getenv('SHEETS_KEY')
if not SHEETS_KEY:
    raise RuntimeError("SHEETS_KEY not defined in .env")
SHEETS_API = f"https://script.google.com/macros/s/{SHEETS_KEY}/exec"


def compute_gas_quality(current, baseline):
    if baseline <= 0:
        return None, None

    ratio = current / baseline

    score = max(0, min(100, int(100 * ratio)))

    if score > 80:
        label = "GOOD"
    elif score > 50:
        label = "MODERATE"
    else:
        label = "BAD"

    return score, label

def get_data():
    r = requests.get(SHEETS_API)
    r.raise_for_status()
    raw = r.json()
    gas_values = [r["value"] for r in raw if r["sensor"] == "gas" and r["metric"] == "raw"]
    baseline = statistics.mean(gas_values[-20:]) if len(gas_values) >= 5 else None

    for r in raw:
        r["quality"] = None
        if r["sensor"] == "gas" and r["metric"] == "raw" and baseline:
            score, label = compute_gas_quality(r["value"], baseline)
            r["quality"] = label
            r["quality_score"] = score

    return raw







@app.route("/")
def index():
    data = get_data()
    return render_template("index.html", rows=data)



@app.route("/chart")
def chart():
    data = get_data()

    # === GRAFICE INDIVIDUALE ===
    series = {}
    for r in data:
        key = f"{r['node']}_{r['sensor']}_{r['metric']}"
        series.setdefault(key, []).append({
            "t": r["timestamp"],
            "v": r["value"]
        })

    # === GRAFIC COMBINAT TEMP + HUM (ex: DHT22 master) ===
    labels = []
    temp = []
    hum = []
    pressure = []

    for r in data:
        if r["node"] == "node1":
            if r["metric"] == "temperature":
                labels.append(r["timestamp"])
                temp.append(r["value"])
            elif r["metric"] == "humidity":
                hum.append(r["value"])
            elif r["metric"] == "pressure":
                pressure.append(r["value"])


    return render_template(
        "charts.html",
        series=series,
        labels=labels,
        temp=temp,
        hum=hum,
        pressure=pressure
    )


if __name__ == "__main__":
    app.run(debug=True)
