import sqlite3

# Initialize the database connection
def init_db():
    conn = sqlite3.connect('input.db')
    cursor = conn.cursor()

    # Create table for Beasley dataset
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS beasley (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            instance TEXT,
            bin_capacity INTEGER,
            best_known_bins INTEGER,
            item_sizes TEXT,
            num_items INTEGER
        )
    ''')

    # Create table for another dataset (e.g., Hard28)
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS hard28 (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            instance TEXT,
            bin_capacity INTEGER,
            best_known_bins INTEGER,
            item_sizes TEXT,
            num_items INTEGER
        )
    ''')

    conn.commit()
    conn.close()

# Call the function to initialize the database
init_db()
