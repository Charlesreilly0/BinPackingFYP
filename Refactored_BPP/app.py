from flask import Flask, request, jsonify, send_from_directory, render_template
import subprocess
import os
import json
import csv
import sqlite3  # Import sqlite3 for database operations

app = Flask(__name__, static_folder='frontend', template_folder='frontend')

ALGORITHM_FILE = 'algorithms.json'

def load_algorithms():
    if os.path.exists(ALGORITHM_FILE):
        with open(ALGORITHM_FILE, 'r') as file:
            data = json.load(file)
            return data['algorithms']
    return []

def save_algorithms(algorithms):
    with open(ALGORITHM_FILE, 'w') as file:
        json.dump({'algorithms': algorithms}, file)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/static/<path:path>')
def send_static(path):
    return send_from_directory('frontend', path)

@app.route('/get_algorithms', methods=['GET'])
def get_algorithms():
    algorithms = load_algorithms()
    return jsonify({'algorithms': algorithms})

@app.route('/add_algorithm', methods=['POST'])
def add_algorithm():
    data = request.get_json()
    new_algorithm = {'name': data['name'], 'file': data['file']}
    algorithms = load_algorithms()
    if new_algorithm['name'] not in [algo['name'] for algo in algorithms]:
        algorithms.append(new_algorithm)
        save_algorithms(algorithms)
        return jsonify({'status': 'success'})
    return jsonify({'status': 'error', 'message': 'Algorithm already exists'})

@app.route('/remove_algorithm', methods=['POST'])
def remove_algorithm():
    data = request.get_json()
    algorithm_name = data['name']
    algorithms = load_algorithms()
    algorithms = [algo for algo in algorithms if algo['name'] != algorithm_name]
    save_algorithms(algorithms)
    return jsonify({'status': 'success'})

@app.route('/submit', methods=['POST'])
def submit():
    data_input_method = request.form['dataInputMethod']
    format_type = request.form.get('formatType', 'Beasley')  # Default to Beasley if not specified
    output = None
    errors = None
    
    if data_input_method == 'raw':
        item_sizes = request.form['itemSizes']
        bin_capacity = request.form['binCapacity']
        best_known_solution = request.form['bestKnownSolution']
        algorithms = request.form.get('algorithms', 'FirstFit')  # Default to FirstFit if not specified
        data_structure = request.form.get('dataStructure', 'OOP')  # Default to OOP if not specified
        
        output, errors = run_cpp_application(bin_capacity, best_known_solution, item_sizes, algorithms, data_structure)
        save_to_database('raw', 'RawInstance', bin_capacity, best_known_solution, item_sizes, len(item_sizes.split(',')), output, errors)

    if errors:
        return jsonify({'status': 'error', 'message': errors})

    return jsonify({'status': 'success', 'output': output})

def clean_item_sizes(item_sizes):
    # Properly format item sizes for the application by removing brackets and spaces
    return item_sizes.replace('[', '').replace(']', '').replace(' ', '')

def run_cpp_application(bin_capacity, best_known_bins, item_sizes, algorithms, data_structure):
    # Command to run the C++ application
    command = [
        "./build/BinPackingGame",  # Path to the executable
        str(bin_capacity),
        str(best_known_bins),
        clean_item_sizes(item_sizes),
        algorithms,  # Pass the algorithm(s) as a string, e.g., "FirstFit,BestFit"
        data_structure,  # Pass the data structure type, e.g., "OOP", "AVLTree", "MTP"
        "N"  # Assuming you don't want to print the configuration
    ]
    print("Running command:", " ".join(command))

    # Execute the command and capture the output
    try:
        output = subprocess.run(command, text=True, capture_output=True, check=True)
        return output.stdout, output.stderr
    except subprocess.CalledProcessError as e:
        return e.stdout, e.stderr

def save_to_database(table_name, instance, bin_capacity, best_known_bins, item_sizes, num_items, output, errors):
    conn = sqlite3.connect('results.db')
    cursor = conn.cursor()
    
    cursor.execute(f'''
        INSERT INTO {table_name} (instance, bin_capacity, best_known_bins, item_sizes, num_items, output, errors)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    ''', (instance, bin_capacity, best_known_bins, item_sizes, num_items, output, errors))

    conn.commit()
    conn.close()

def test_bin_packing_on_csv(file_path, format_type):
    with open(file_path, newline='') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # Skip the header row
        for row in reader:
            if len(row) >= 5:  # Ensure there are enough columns
                if format_type == "Beasley":
                    instance = row[0]
                    num_items = row[2]
                    bin_capacity = row[1]
                    best_known_bins = row[3]
                    item_sizes = row[4]
                    output, errors = run_cpp_application(bin_capacity, best_known_bins, item_sizes)
                    save_to_database('beasley', instance, bin_capacity, best_known_bins, item_sizes, num_items, output, errors)
                elif format_type == "Hard28":
                    instance = row[0]
                    instance_id = row[1]
                    bin_capacity = row[2]
                    number_of_items = row[3]
                    best_known_bins = row[4]
                    item_sizes = row[5]
                    output, errors = run_cpp_application(bin_capacity, best_known_bins, item_sizes)
                    save_to_database('hard28', instance, bin_capacity, best_known_bins, item_sizes, instance_id, output, errors)

if __name__ == '__main__':
    app.run(debug=True)
