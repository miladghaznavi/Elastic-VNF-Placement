import sys
import os.path
import argparse
import csv
from StringIO import StringIO
import webbrowser
import random

from nvd3 import lineChart, lineWithFocusChart

DELIM = ","
NEWLINE = "\n"

ACCEPTANCE = "acceptance"
COST = "cost"
REJECTED = "rejected"
TIME = "time"
UTIL = "util"

ACCEPTANCE_FILE = "acceptance.txt"
COST_FILE = "cost.txt"
REJECTED_FILE = "rejected.txt"
TIME_FILE = "time.txt"
UTIL_FILE = "util.txt"
LINE_CHART = "line"
LINE_WITH_FOCUS_CHART = "focus"

COLOR_LIST = [
    '#2ECC71',
    '#3498DB',
    '#F1C40F',
    '#E67E22',
    '#C0392B',
]

def def_parser():
    parser = argparse.ArgumentParser(description='Draw graphs for EVNFP')
    parser.add_argument('-i', '--input', dest='i', help='Folder containing output folders',
                        type=str, required=True)
    parser.add_argument('-m', '--measure', dest='m',
                        help='Which measurement (one of "acceptance", "cost", "time", "util")',
                        type=str, required=True)
    parser.add_argument('-x', '--xfield', dest='x', help='Which field',
                        type=str, required=True)
    parser.add_argument('-y', '--yfield', dest='y', help='Which field',
                        type=str, required=True)
    parser.add_argument('-c', '--clean', dest='c', help='Clean data in y (default is None)',
                        type=int, default=None)
    parser.add_argument('-n', '--norm', dest='n', help='Normalize values by a time step',
                        type=int, default=None)
    parser.add_argument('-t', '--type', dest='t',
                        help='Chart type (default is %s). <%s, %s>' % (LINE_CHART, LINE_CHART, LINE_WITH_FOCUS_CHART),
                        type=str, default=LINE_CHART)
    parser.add_argument('-s', '--sample', dest='s',
                        help='Sampling of data. Note that sampling is done before normalizing!',
                        type=int, default=None)
    parser.add_argument('-e', '--exclude', dest='e', help='exclude these floders',
                        type=str, default=None)
    parser.add_argument('-l', '--log', dest='l', help='Log output path',
                        type=str, default=None)
    parser.add_argument('-d', '--draw', dest='d', help='Draw file name',
                        type=str, default=None)
    parser.add_argument('-b', '--browse', dest='b', help='Browse html file',
                        action='store_true', default=False)

    return parser


def parse_args(parser):
    opts = vars(parser.parse_args(sys.argv[1:]))
    if not os.path.isdir(opts['i']):
        raise Exception('Folder \'%s\' does not exist!' % opts['i'])
    if opts['l'] == None:
        opts['l'] = "%s.csv" % opts['y']
    if opts['d'] == None:
        opts['d'] = "%s.html" % opts['y']
    if opts['e'] != None:
        opts['e'] = opts['e'].lower().split(',')
    if opts['t'] != LINE_CHART and opts['t'] != LINE_WITH_FOCUS_CHART:
        raise Exception('No chart type "%s"' % opts['t'])
    return opts


def which_file(arg):
    if arg == ACCEPTANCE:
        result = ACCEPTANCE_FILE
    elif arg == COST:
        result = COST_FILE
    elif arg == TIME:
        result = TIME_FILE
    elif arg == UTIL:
        result = UTIL_FILE
    else:
        raise Exception("Not valid file option")
    return result


def parse_file(path, xfield, yfield):
    x = []
    y = []
    with open(path) as handle:
        content = handle.read()
        dictReader = csv.DictReader(StringIO(content))
        for row in dictReader:
            x.append(row[xfield])
            y.append(row[yfield])
    return {'x': x, 'y': y}


def normalize(data, step):
    start = 0
    y_sum = 0
    x_last = 0
    y_last = 0
    normal = {'x': [], 'y': []}
    for i in range(0, len(data['x'])):
        x = float(data['x'][i])
        y = float(data['y'][i])
        # if a step has not been passed yet
        if x - start < step or x == x_last:
            y_sum += y_last
        # if a step has passed
        else:
            # Until all steps in this durations are captured
            while start + step <= x:
                y_sum += float(start + step - x_last) / (x - x_last) * y_last
                normal['x'].append(start)
                normal['y'].append(y_sum)
                start += step
            # Leftover
            y_sum = (x - start) / (x - x_last) * y_last
        # Save
        x_last = x
        y_last = y
    normal['x'].append(normal['x'][-1] + step)
    normal['y'].append(y_sum)

    if normal['x'][-1] + step != data['x'][-1]:
        normal['x'].append(data['x'][-1])
        normal['y'].append(0)

    return normal


def clean(logs, what):
    for alg in logs:
        i = 0
        while True:
            if i >= len(logs[alg]['y']):
                break
            if float(logs[alg]['y'][i]) <= what:
                del logs[alg]['x'][i]
                del logs[alg]['y'][i]
                i -= 1
            i += 1


def x_axis_scale(logs):
    result = 0
    for alg in logs:
        for x in logs[alg]['x']:
            result = max(float(x), result)

    # result = math.ceil(float(result) / 10) * 10
    return [0, result]


def y_axis_scale(logs):
    result = 0
    for alg in logs:
        for y in logs[alg]['y']:
            result = max(float(y), result)

    # result = math.ceil(float(result) / 10) * 10
    return [0, result]


def sample_indices(size, step):
    indices = []
    ii = 0
    while ii < size:
        indices.append(random.randint(ii, min(ii + step, size - 1)))
        ii += step

    # always save last index
    if indices[len(indices) - 1] != size - 1:
        indices.append(size - 1)

    return indices


def sampling(logs, step):
    samples_found = False
    indices = []
    for alg in logs:
        if not samples_found:
            indices = sample_indices(len(logs[alg]['x']), step)
            samples_found = True
        logs[alg]['x'] = [logs[alg]['x'][i] for i in indices]
        logs[alg]['y'] = [logs[alg]['y'][i] for i in indices]

    return logs

def log(handle, logs, xfield):
    minsize = float('inf')

    handle.write(xfield)
    for alg in logs:
        handle.write(DELIM + alg)
        minsize = min(minsize, len(logs[alg]['x']))

    for i in range(0, minsize):
        xwritten = False
        handle.write(NEWLINE)
        for alg in logs:
            if not xwritten:
                handle.write(str(logs[alg]['x'][i]))
                xwritten = True
            handle.write(DELIM + str(logs[alg]['y'][i]))


def draw(handle, type, logs, **kwargs):
    kwparams = {
        'name': "lineChart",
        'width': 1000,
        'height': 500,
        'chart_attr': {
            'forceX': x_axis_scale(logs),
            'forceY': y_axis_scale(logs),
            # 'yAxis.scale': y_axis_scale(logs),
            'xAxis.axisLabel': ('"%s"' % kwargs['xlabel']),
            'yAxis.axisLabel': ('"%s"' % kwargs['ylabel']),
            'useInteractiveGuideline': 'true',
            # 'x2Axis.scale': xAxisScale(logs),
            # 'y2Axis.scale': yAxisScale(logs),
        },
    }
    chart = None
    if type == LINE_CHART:
        chart = lineChart(**kwparams)
    elif type == LINE_WITH_FOCUS_CHART:
        chart = lineWithFocusChart(**kwparams)

    chart.show_labels = True

    count = 0
    for alg in logs:
        extra_serie = {
            "tooltip": {"y_start": kwargs['ylabel'] + "is ", "y_end": "!"},
        }
        chart.add_serie(y=logs[alg]['y'], x=logs[alg]['x'], color=COLOR_LIST[count], extra=extra_serie, name=alg)
        count += 1

    chart.buildhtml()
    handle.write(str(chart))

def main():
    try:
        args = parse_args(def_parser())
        base_dir = args['i']
        xfield = args['x']
        yfield = args['y']
        type = args['t']

        dirs = next(os.walk(base_dir))[1]
        wh_file = which_file(args['m'])

        # Read data from file
        logs = {}
        for dr in dirs:
            if args['e'] and dr.lower() in args['e']:
                continue
            logs[dr] = parse_file(os.path.join(base_dir, dr, wh_file), xfield, yfield)
            if args['n'] is not None:
                logs[dr] = normalize(logs[dr], args['n'])

        # Sampling
        if args['s'] is not None:
            logs = sampling(logs, args['s'])

        # Cleaning
        if args['c']:
            clean(logs, args['c'])

        # Store results
        path = os.path.abspath(args['l'])
        with open(path, 'w') as handle:
            log(handle, logs, xfield)
        path = os.path.abspath(args['d'])
        with open(path, 'w') as handle:
            draw(handle, type, logs, xlabel=xfield, ylabel=yfield)

        if args['b']:
            uri = "file://" + path
            webbrowser.open(uri, new=2)
        print("Done!")
    except argparse.ArgumentError as exc:
        print(exc)

if __name__ == "__main__":
    main()
