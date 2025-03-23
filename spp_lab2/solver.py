from Pyro4 import expose
import hashlib

class Solver:
    def __init__(self, workers=None, input_file_name=None, output_file_name=None):
        self.input_file_name = input_file_name
        self.output_file_name = output_file_name
        self.workers = workers
        print("Inited")

    def solve(self):
        print("Job Started")
        print("Workers %d" % len(self.workers))

        arr = self.read_input()
        step = int(len(arr) / len(self.workers))

        mapped = []
        for i in range(len(self.workers)):
            chunk = arr[i * step: (i + 1) * step]
            mapped.append(self.workers[i].mymap(chunk))
        result = self.myreduce(mapped)
        self.write_output(result)

    @staticmethod
    @expose
    def mymap(lines):
        hashed = []
        for line in lines:
            encoded = line.strip().encode('utf-8')
            hashed.append(hashlib.sha256(encoded).hexdigest())
        return hashed

    @staticmethod
    @expose
    def myreduce(mapped):
        print("reduce")
        output = []
        for group in mapped:
            output += group.value
        print("reduce done")
        return output

    def read_input(self):
        with open(self.input_file_name, 'r') as f:
            return [line.strip() for line in f]

    def write_output(self, output):
        with open(self.output_file_name, 'w') as f:
            for hashed in output:
                f.write(hashed + '\n')
        print("output done")
