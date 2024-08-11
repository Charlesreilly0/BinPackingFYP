import unittest
from app import app, load_algorithms, save_algorithms, run_cpp_application

class FlaskAppTestCase(unittest.TestCase):

    # Set up the test client
    def setUp(self):
        self.app = app.test_client()
        self.app.testing = True

    # Test if the Flask app was set up correctly
    def test_index(self):
        response = self.app.get('/')
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'<!DOCTYPE html>', response.data)

    # Test getting algorithms when algorithms exist
    def test_get_algorithms(self):
        response = self.app.get('/get_algorithms')
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'algorithms', response.data)

        # Assuming that some algorithms are pre-loaded
        algorithms = load_algorithms()
        for algo in algorithms:
            self.assertIn(algo['name'].encode(), response.data)

    # Test adding a new algorithm
    def test_add_algorithm(self):
        data = {'name': 'TestAlgorithm', 'file': 'test_algo.py'}
        response = self.app.post('/add_algorithm', json=data)
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'success', response.data)

        # Verify that the algorithm was added
        response = self.app.get('/get_algorithms')
        self.assertIn(b'TestAlgorithm', response.data)

    # Test removing an algorithm
    def test_remove_algorithm(self):
        data = {'name': 'TestAlgorithm'}
        response = self.app.post('/remove_algorithm', json=data)
        self.assertEqual(response.status_code, 200)
        self.assertIn(b'success', response.data)

        # Verify that the algorithm was removed
        response = self.app.get('/get_algorithms')
        self.assertNotIn(b'TestAlgorithm', response.data)

    # Test running the C++ application
    def test_run_cpp_application(self):
        bin_capacity = 100
        best_known_bins = 10
        item_sizes = '20,30,50'
        output, errors = run_cpp_application(bin_capacity, best_known_bins, item_sizes)
        self.assertIsNotNone(output)
        self.assertEqual(errors, '')

if __name__ == '__main__':
    unittest.main()
