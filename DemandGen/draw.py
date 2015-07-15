import sys
import os.path
import argparse
import re
from nvd3 import lineChart
import webbrowser

DELIM 	= " "
NEWLINE = "\n"

DEMAND_FORMAT1 = '(?P<src>[0-9]+)\s(?P<trg>[0-9]+)\s(?P<arr>[0-9]+)\s(?P<dur>[0-9]+)'
DEMAND_FORMAT2 = '(?P<id>[0-9]+)\s(?P<src>[0-9]+)\s(?P<trg>[0-9]+)\s(?P<arr>[0-9]+)\s(?P<dur>[0-9]+)'

DEMAND_FORMAT = ''

def def_parser():
    parser = argparse.ArgumentParser(description='Generating Service Requests!')
    parser.add_argument('-i', '--input', dest='i', help='Demands file (default is requests.txt)',
                        type=str, default='requests.txt')
    parser.add_argument('-l', '--log', dest='l', help='Log file name (default is log.txt)',
                        type=str, default='log.txt')
    parser.add_argument('-d', '--draw', dest='d', help='Draw file name (default is index.html)',
                        type=str, default='index.html')
    parser.add_argument('-n', '--no', dest='n', help='No id in request file',
                        action='store_true')
    return parser

def parse_args(parser):
    global DEMAND_FORMAT
    opts = vars(parser.parse_args(sys.argv[1:]))
    if not os.path.isfile(opts['i']):
        raise Exception('Demands file \'%s\' does not exist!' % opts['i'])
    DEMAND_FORMAT = DEMAND_FORMAT1 if opts['n'] else DEMAND_FORMAT2
    return opts

def generate(handle):
    content = handle.read()
    r = re.compile(DEMAND_FORMAT)
    workload = []
    for w in r.finditer(content):
        d = w.groupdict()
        arr = int(d['arr'])
        dep = int(d['dur']) + arr
        size = len(workload)
        if dep > size:
            workload.extend([0] * (dep - size))
        for i in range(max(0, arr - 1), dep):
            workload[i] += 1
    workload.append(0)

    return workload

def write_to_file(handle, workload):
    for i in range(0, len(workload)):
        handle.write("%d%s%d%s" % (i, DELIM, workload[i], NEWLINE))

def draw(handle, workload):
    x_data = range(0, len(workload))
    chart = lineChart(name="lineChart", width=1000, height=500)
    chart.add_serie(y=workload, x=x_data, name='Workload')
    chart.buildhtml()
    handle.write(str(chart))

def main():
    try:
        args = parse_args(def_parser())
        with open(args['i']) as handle:
            workload = generate(handle)
        path = os.path.abspath(args['l'])
        with open(path, 'w') as handle:
            write_to_file(handle, workload)
        path = os.path.abspath(args['d'])
        with open(path, 'w') as handle:
            draw(handle, workload)

        uri = "file://" + path
        webbrowser.open(uri, new=2)
    except argparse.ArgumentError:
        print(argparse.error)
    except Exception as exc:
        print(exc)

if __name__ == "__main__":
    main()