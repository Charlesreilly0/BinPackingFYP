import sqlite3
import csv

def store_csv_in_database(file_path, table_name):
    conn = sqlite3.connect('input.db')
    cursor = conn.cursor()

    with open(file_path, newline='') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # Skip the header row

        for row in reader:
            if len(row) >= 5:
                instance = row[0]
                bin_capacity = row[1]
                num_items = row[2]
                best_known_bins = row[3]
                item_sizes = row[4]

                cursor.execute(f'''
                    INSERT INTO {table_name} (instance, bin_capacity, best_known_bins, item_sizes, num_items)
                    VALUES (?, ?, ?, ?, ?)
                ''', (instance, bin_capacity, best_known_bins, item_sizes, num_items))

    conn.commit()
    conn.close()

if __name__ == '__main__':
    # Path to your CSV file
    file_path = 'input-data/input.csv'  # or 'beasley_fixed_out.csv' based on your file
    table_name = 'beasley'  # or 'hard28' based on your desired table

    # Load CSV data into the database
    store_csv_in_database(file_path, table_name)
    print(f"Data from {file_path} has been loaded into the {table_name} table.")
