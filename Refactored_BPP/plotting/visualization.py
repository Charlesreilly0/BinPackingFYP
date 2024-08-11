import sqlite3
import matplotlib.pyplot as plt

def fetch_data_from_db(table_name):
    conn = sqlite3.connect('results.db')
    cursor = conn.cursor()

    # Query to fetch data from the specified table
    cursor.execute(f"SELECT instance, output FROM {table_name}")
    rows = cursor.fetchall()

    conn.close()
    
    # Processing the fetched data to return in a dictionary format
    data = {}
    for row in rows:
        instance, output = row
        data[instance] = float(output)  # Convert output to float or appropriate type based on data

    return data

def generate_bar_chart(data):
    labels = data.keys()
    values = data.values()

    plt.bar(labels, values)
    plt.xlabel('Instance')
    plt.ylabel('Output Value')
    plt.title('Output Values by Instance')
    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.savefig('frontend/plot.png')

if __name__ == '__main__':
    # Fetch data from the database
    table_name = 'beasley'  # or 'hard28' based on which data you want to plot
    data = fetch_data_from_db(table_name)
    
    # Generate the bar chart with the fetched data
    generate_bar_chart(data)
