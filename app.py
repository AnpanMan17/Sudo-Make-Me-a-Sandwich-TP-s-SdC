import time
import random
from flask import Flask, render_template_string, jsonify, request

app = Flask(__name__)

DEVICE_PATH = "/dev/SdeC_SudoMakeMe"

# Variables globales para simular la inercia física a nivel de usuario
temp_actual = 22.0
presion_actual = 1011.0
canal_actual = 0

# Código HTML e interfaz (Se mantiene igual a tu versión con reverse: true)
HTML_INTERFACE = """
<!DOCTYPE html>
<html>
<head>
    <title>SdeC - Panel de Telemetría</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f9; color: #333; }
        .container { width: 70%; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0px 0px 10px rgba(0,0,0,0.1); }
        button { padding: 10px 20px; font-size: 16px; margin: 10px; cursor: pointer; border: none; border-radius: 4px; background: #007bff; color: white; }
        button.active { background: #28a745; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Monitoreo de Señales en Tiempo Real (Pi Zero 2W)</h2>
        <div>
            <button id="btn0" class="active" onclick="cambiarCanal(0)">Canal 0 (Temperatura)</button>
            <button id="btn1" onclick="cambiarCanal(1)">Canal 1 (Presión)</button>
        </div>
        <div style="width:100%; height:450px;">
            <canvas id="telemetryChart"></canvas>
        </div>
    </div>

    <script>
        let tiempoActual = 0;
        let canalActivo = 0;
        
        const ctx = document.getElementById('telemetryChart').getContext('2d');
        const chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [], 
                datasets: [{
                    label: 'Canal 0: Temperatura (°C)',
                    data: [], 
                    borderColor: '#007bff',
                    fill: false,
                    tension: 0.3 // Curvas más suaves en el gráfico
                }]
            },
            options: {
                indexAxis: 'y', 
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: { title: { display: true, text: 'Temperatura (°C)' } },
                    y: { title: { display: true, text: 'Tiempo transcurrido en segundos (Tiempo en Ordenadas)' }, beginAtZero: true, reverse: true }
                }
            }
        });

        function cambiarCanal(canal) {
            fetch('/cambiar_canal', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({canal: canal})
            }).then(() => {
                canalActivo = canal;
                if(canal === 0) {
                    chart.data.datasets[0].label = 'Canal 0: Temperatura (°C)';
                    chart.config.options.scales.x.title.text = 'Temperatura (°C)';
                } else {
                    chart.data.datasets[0].label = 'Canal 1: Presión (hPa)';
                    chart.config.options.scales.x.title.text = 'Presión (hPa)';
                }
                chart.data.labels = [];
                chart.data.datasets[0].data = [];
                tiempoActual = 0;
                chart.update();
                
                document.getElementById('btn0').className = canal === 0 ? 'active' : '';
                document.getElementById('btn1').className = canal === 1 ? 'active' : '';
            });
        }

        setInterval(() => {
            fetch('/data')
                .then(response => response.json())
                .then(res => {
                    tiempoActual += 1;
                    chart.data.labels.push(tiempoActual + "s");
                    chart.data.datasets[0].data.push(res.valor);
                    
                    if (chart.data.labels.length > 20) {
                        chart.data.labels.shift();
                        chart.data.datasets[0].data.shift();
                    }
                    chart.update();
                });
        }, 1000);
    </script>
</body>
</html>
"""

@app.route('/')
def index():
    return render_template_string(HTML_INTERFACE)

@app.route('/data')
def get_data():
    global temp_actual, presion_actual
    try:
        with open(DEVICE_PATH, 'r') as f:
            # Leemos el bit crudo (0 o 1) entregado por el kernel
            raw_bit = int(f.readline().strip())
    except Exception as e:
        print("Error leyendo el CDD:", e)
        raw_bit = 0

    # --- CORRECCIÓN DE ESCALAS Y PROCESAMIENTO A NIVEL DE USUARIO ---
    if canal_actual == 0:
        # Si el pin está en 1, el sistema se calienta hacia 28°C. Si está en 0, se enfría hacia 21°C
        target_temp = 28.0 if raw_bit == 1 else 21.0
        # Ecuación de diferencias (Filtro sintonizado para simular respuesta térmica real)
        temp_actual = temp_actual + 0.4 * (target_temp - temp_actual) + random.uniform(-0.1, 0.1)
        valor_final = round(temp_actual, 2)
    else:
        # Lo mismo para la Presión: oscila fluidamente entre 1008 y 1018 hPa
        target_presion = 1018.0 if raw_bit == 1 else 1008.0
        presion_actual = presion_actual + 0.5 * (target_presion - presion_actual) + random.uniform(-0.3, 0.3)
        valor_final = round(presion_actual, 2)

    return jsonify(valor=valor_final)

@app.route('/cambiar_canal', methods=['POST'])
def cambiar_canal():
    global canal_actual
    data = request.get_json()
    canal = data.get('canal', 0)
    try:
        with open(DEVICE_PATH, 'w') as f:
            f.write(str(canal))
        canal_actual = canal
    except Exception as e:
        print("Error al conmutar canal:", e)
    return jsonify(status="success")

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)