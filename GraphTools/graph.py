import os
from math import sqrt, floor
import json
import sys
import os.path
import argparse
import csv
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

SPINE_COLOR = 'gray'

BANDWIDTH_UNIT = 20
WINDOW = 2
SLFL = 'SLFL'
TIME = 'time'

MIGRATIONS       = 'migrations'
MIGRATIONS_LABEL = 'Migrations'
REASSIGNS        = 'reassigns'
REASSIGNS_LABEL  = 'Reassignments'

ACCEPTED_WORKLOAD   = 'Accepted-Workload'
ARR_ACCEPTANCE      = 'Arr-Acceptance'
VNF_UTIL            = 'VNF-Util'
LINKS_UTIL          = 'Links-Util'
HOSTS_UTIL          = 'Hosts-Util'
TOTAL_COST          = 'Total-Cost'
BANDWIDTH_COST      = 'Bandwidth-Cost'
HOSTS_COST          = 'Host-Cost'
MIGRATION_COST      = 'Migration'
OVERHEAD_COST       = 'Overhead-Cost'
REASSIGNS_COST      = 'Reassignment'
VNFS_COUNT          = 'VNFs-Count'
OVERHEAD            = 'Overhead'

STAT_FILE_PATH = 'statistics.json'

algs = {
    'SLFL',
    'Random',
    'FirstFit',
    # 'Lowerbond'
}

colors = {
    SLFL: '#298F24',
    'Random': '#1C60A0',
    'FirstFit': '#F26813',
    'Lowerbond': '#C21220',
    MIGRATIONS: '#C0392B',
    REASSIGNS: '#3498DB',
    MIGRATION_COST: '#C0392B',
    REASSIGNS_COST: '#3498DB',
}

styles = {
    SLFL: '-.',
    'Random': '--',
    'FirstFit': '-',
    'Lowerbond': '-.',
    MIGRATIONS: '//',
    REASSIGNS : '0',
    MIGRATION_COST: '-',
    REASSIGNS_COST: '--',
}

files = {
    ACCEPTED_WORKLOAD: '%s/demands.csv',
    ARR_ACCEPTANCE   : '%s/arrAccRatio.csv',
    VNF_UTIL         : '%s/facilityUtil.csv',
    LINKS_UTIL       : '%s/allLinksUtil.csv',
    HOSTS_UTIL       : '%s/allHostUtil.csv',
    TOTAL_COST       : '%s/totalCost.csv',
    BANDWIDTH_COST   : '%s/transCost.csv',
    HOSTS_COST       : '%s/installCost.csv',
    MIGRATION_COST   : '%s/migCost.csv',
    REASSIGNS_COST   : '%s/reasgnCost.csv',
    OVERHEAD_COST    : {
        MIGRATION_COST: '%s/migCost.csv',
        REASSIGNS_COST: '%s/reasgnCost.csv',
    },
    VNFS_COUNT       : '%s/vnfs.csv',
    OVERHEAD         : {
        MIGRATIONS   : '%s/migrations.csv',
        REASSIGNS    : '%s/reassigns.csv'
    },
}

def prepare_files(base_path):
    for key in files:
        if type(files[key]) is str:
            files[key] = files[key] % base_path
        else:
            for k2 in files[key]:
                files[key][k2] = files[key][k2] % base_path

def def_parser():
    parser = argparse.ArgumentParser(description='Graph all in a directory')
    parser.add_argument('-i', '--input', dest='i',
                        help='Input folder',
                        type=str, required=True)
    return parser


def parse_args(parser):
    opts = vars(parser.parse_args(sys.argv[1:]))
    if not os.path.isdir(opts['i']):
        raise IOError('Directory "%s" does not exists!' % opts['i'])
    return opts


def latexify(fig_width=None, fig_height=None, columns=1):
    """Set up matplotlib's RC params for LaTeX plotting.
    Call this before plotting a figure.

    Parameters
    ----------
    fig_width : float, optional, inches
    fig_height : float,  optional, inches
    columns : {1, 2}
    """
    assert (columns in [1, 2])

    if fig_width is None:
        fig_width = 3.39 if columns == 1 else 6.9  # width in inches

    if fig_height is None:
        golden_mean = (sqrt(5) - 1.0) / 2.0  # Aesthetic ratio
        fig_height = fig_width * golden_mean  # height in inches

    MAX_HEIGHT_INCHES = 8.0
    if fig_height > MAX_HEIGHT_INCHES:
        print("WARNING: fig_height too large:" + fig_height +
              "so will reduce to" + str(MAX_HEIGHT_INCHES) + "inches.")
        fig_height = MAX_HEIGHT_INCHES

    # fig_width += 2
    # fig_height += 2

    params = {
        'backend': 'ps',
        'text.latex.preamble': ['\usepackage{gensymb}', '\usepackage{wasysym}'],
        'axes.labelsize'     : 8,  # fontsize for x and y labels (was 10)
        'axes.titlesize'     : 8,
        'text.fontsize'      : 8,  # was 10
        'legend.fontsize'    : 8,  # was 10
        'xtick.labelsize'    : 8,
        'ytick.labelsize'    : 8,
        'text.usetex'        : True,
        'figure.figsize'     : [fig_width, fig_height],
        'font.family'        : 'serif'
    }

    matplotlib.rcParams.update(params)


def format_axes(ax):
    for spine in ['top', 'right']:
        ax.spines[spine].set_visible(False)

    for spine in ['left', 'bottom']:
        ax.spines[spine].set_color(SPINE_COLOR)
        ax.spines[spine].set_linewidth(0.5)

    ax.xaxis.set_ticks_position('bottom')
    ax.yaxis.set_ticks_position('left')

    for axis in [ax.xaxis, ax.yaxis]:
        axis.set_tick_params(direction='out', color=SPINE_COLOR)

    return ax


def ratio_graph(**kwargs):
    def percentage(x, pos):
        return '%d' % (float(x) * 100)
    data = np.genfromtxt(kwargs.get('input'), dtype=float, delimiter=',', names=True)

    fig, ax = plt.subplots()
    formatter = FuncFormatter(percentage)
    ax.yaxis.set_major_formatter(formatter)

    ax.autoscale_view()

    format_axes(ax)
    plt.xticks(rotation=30)
    x_label = kwargs.get('x_label', None)
    y_label = kwargs.get('y_label', None)
    if x_label:
        plt.xlabel(x_label)

    if y_label:
        plt.ylabel(y_label)

    plt.ylim(0, 1)
    plt.grid()
    for alg in algs:
        plt.plot(data[kwargs.get('x_field')], data[alg], styles[alg], color=colors[alg], label=alg)
    plt.legend()

    plt.tight_layout()

    plt.savefig(kwargs.get('output'))
    if kwargs.get('show', False):
        os.system("open " + kwargs.get('output'))

    return data

def normal_graph(**kwargs):
    data = np.genfromtxt(kwargs.get('input'), dtype=float, delimiter=',', names=True)

    fig, ax = plt.subplots()

    if kwargs.get('y_formatter', None) is not None:
        formatter = FuncFormatter(kwargs.get('y_formatter'))
        ax.yaxis.set_major_formatter(formatter)

    format_axes(ax)
    plt.xticks(rotation=30)
    x_label = kwargs.get('x_label', None)
    y_label = kwargs.get('y_label', None)

    if x_label:
        plt.xlabel(x_label)

    if y_label:
        plt.ylabel(y_label)

    if kwargs.get('x_lim1', None) is not None and \
        kwargs.get('x_lim2', None) is not None:
        plt.xlim(kwargs.get('x_lim1'), kwargs.get('x_lim2'))

    if kwargs.get('y_lim1', None) is not None and \
        kwargs.get('y_lim2', None) is not None:
        plt.ylim(kwargs.get('y_lim1'), kwargs.get('y_lim2'))

    plt.grid()

    for alg in algs:
        plt.plot(data[kwargs.get('x_field')], data[alg], styles[alg], color=colors[alg], label=alg)
    plt.legend()

    plt.tight_layout()

    plt.savefig(kwargs.get('output'))
    if kwargs.get('show', False):
        os.system("open " + kwargs.get('output'))

    return data


def accepted_workload(file_path, bandwidth_unit):
    def bandwidth_workload(x, pos):
        'The two args are the value and tick position'
        return '%d' % (int(x) * bandwidth_unit)
    data = normal_graph(**{
        'input': file_path,
        'output': 'workload.eps',
        'x_label': 'Time (s)',
        'y_label': 'Accepted workload (Mb)',
        'x_field': TIME,
        'x_lim1': 0,
        'x_lim2': 35000,
        'y_formatter': bandwidth_workload,
        # 'show': True
    })

    return stats(data)


def arr_acceptance(file_path):
    data = ratio_graph(**{
        'input': file_path,
        'output': 'acceptance.eps',
        'x_field': TIME,
        'x_label': 'Time (s)',
        # 'y_label': 'Accepted workload',
        # 'show': True
    })

    return stats(data)

def vnf_util(file_path):
    data = ratio_graph(**{
        'input': file_path,
        'output': 'facilityUtil.eps',
        'x_field': TIME,
        'x_label': 'Time (s)',
        # 'y_label': 'VNF resources utilization',
        # 'show': True
    })

    return stats(data)

def links_util(file_path):
    data = ratio_graph(**{
        'input': file_path,
        'output': 'linksUtil.eps',
        'x_field': TIME,
        'x_label': 'Time (s)',
        # 'y_label': 'Links utilization',
        # 'show': True
    })

    return stats(data)

def hosts_util(file_path):
    data = ratio_graph(**{
        'input': file_path,
        'output': 'hostsUtil.eps',
        'x_field': TIME,
        'x_label': 'Time (s)',
        # 'y_label': 'Hosts Processor utilization',
        # 'show': True
    })
    return stats(data)

def total_cost(file_path, ylim1, ylim2):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.2f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'total_cost.eps',
        'x_label': 'Time (s)',
        'y_label': 'Cost (\cent per 200 sec.)',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        'y_lim1': ylim1,
        'y_lim2': ylim2,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def bandwidth_cost(file_path, ylim1, ylim2):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.2f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'bandwidth_cost.eps',
        'x_label': 'Time (s)',
        'y_label': 'Cost (\cent per 200 sec.)',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        'y_lim1': ylim1,
        'y_lim2': ylim2,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def hosts_cost(file_path, ylim1, ylim2):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.2f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'hosts_cost.eps',
        'x_label': 'Time (s)',
        'y_label': 'Cost (\cent per 200 sec.)',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        'y_lim1': ylim1,
        'y_lim2': ylim2,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def vnfs_count(file_path):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.2f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'vnfs_count.eps',
        'x_label': 'Time (s)',
        'y_label': 'Number of Facilities',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def migration_cost(file_path):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.7f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'mig_cost.eps',
        'x_label': 'Time (s)',
        'y_label': 'Cost',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def reassignment_cost(file_path):
    def dollar(y, pos):
        'The two args are the value and tick position'
        return '%.3f' % (float(y) / 100.0)
    data = normal_graph(**{
        'input': file_path,
        'output': 'reasgn_cost.eps',
        'x_label': 'Time (s)',
        'y_label': 'Cost',
        'x_field': TIME,
        # 'x_lim1': 0,
        # 'x_lim2': 35000,
        # 'y_formatter': dollar,
        # 'show': True
    })

    return stats(data)

def overhead_cost(files):
    data = { }
    count = { }
    for ff in files:
        with open(files[ff]) as handle:
            data[ff] = [row for row in csv.DictReader(handle)]
        count[ff] = count_in_win(data[ff], WINDOW)

    fig, ax = plt.subplots()
    format_axes(ax)
    plt.xticks(rotation=30)
    plt.grid()

    labels = { }
    for ff in files:
        labels[ff] = ff
        plt.plot(count[ff]['x'], count[ff]['y'], styles[ff], color=colors[ff], label=ff)

    plt.legend()
    plt.yscale('log', nonposy='clip')
    plt.ylabel('\cent per 200 sec.')
    plt.tight_layout()

    plt.savefig('overhead_cost.eps')
    # os.system("open overhead_cost.eps")

    result = {}
    for ff in files:
        result[ff] = {
            'min' : np.min (count[ff]['y']),
            'max' : np.max (count[ff]['y']),
            'mean': np.mean(count[ff]['y']),
            'std' : np.std (count[ff]['y']),
            'sum' : np.sum (count[ff]['y']),
        }

    return result

def count_in_win(ff_data, window):
    result = {'x' : [], 'y' : []}
    last_x = 0
    sum_y = 0
    for row in ff_data:
        x = row[TIME]
        y = row[SLFL]
        if last_x + window > int(x):
            sum_y += float(y)
        else:
            result['x'].append(last_x)
            result['y'].append(sum_y)

            last_x = floor(float(x) / window) * window
            sum_y = float(y)

    result['x'].append(last_x)
    result['y'].append(sum_y)

    return result

def overhead(files):
    data = { }
    count = { }
    for ff in files:
        with open(files[ff]) as handle:
            data[ff] = [row for row in csv.DictReader(handle)]
        count[ff] = count_in_win(data[ff], WINDOW)

    labels = { }
    for ff in files:
        if ff == MIGRATION_COST or REASSIGNS_COST:
            continue
        labels[ff] = ff
        plt.bar(count[ff]['x'], count[ff]['y'], label=labels[ff], color=colors[ff], edgecolor=colors[ff], hatch=styles[ff])


    plt.legend(loc='best')
    plt.xticks(rotation=30)
    # plt.tight_layout()
    plt.yscale('log', nonposy='clip')
    plt.savefig('overhead.eps')
    # os.system("open overhead.eps")

    result = {}
    for ff in files:
        result[ff] = {
            'min' : np.min (count[ff]['y']),
            'max' : np.max (count[ff]['y']),
            'mean': np.mean(count[ff]['y']),
            'std' : np.std (count[ff]['y']),
            'sum' : np.sum (count[ff]['y']),
        }

    return result

def stats(data):
    result = {}

    for alg in algs:
        result[alg] = {
            'min' : np.min (data[alg]),
            'max' : np.max (data[alg]),
            'mean': np.mean(data[alg]),
            'std' : np.std (data[alg]),
            'sum' : np.sum (data[alg]),
        }

    return result

def store_stats(statistics):
    pp = json.dumps(statistics, indent=4)
    with open(STAT_FILE_PATH, 'w') as handle:
        handle.write(pp)
    print pp


def main():

    try:
        args = parse_args(def_parser())

        # Prepare files
        prepare_files(args['i'])

        # Latexify
        latexify()

        # Init statistics
        statistics = {}

        # Workload and Utilization
        statistics[ACCEPTED_WORKLOAD] = accepted_workload(files[ACCEPTED_WORKLOAD], BANDWIDTH_UNIT)
        statistics[ARR_ACCEPTANCE]    = arr_acceptance(files[ARR_ACCEPTANCE])
        statistics[VNF_UTIL]          = vnf_util(files[VNF_UTIL])
        statistics[LINKS_UTIL]        = links_util(files[LINKS_UTIL])
        statistics[HOSTS_UTIL]        = hosts_util(files[HOSTS_UTIL])

        # Costs
        statistics[TOTAL_COST]        = total_cost(files[TOTAL_COST], 0, 150)
        statistics[BANDWIDTH_COST]    = bandwidth_cost(files[BANDWIDTH_COST], 0, 150)
        statistics[HOSTS_COST]        = hosts_cost(files[HOSTS_COST], 0, 150)
        statistics[VNFS_COUNT]        = vnfs_count(files[VNFS_COUNT])
        statistics[MIGRATION_COST]    = migration_cost(files[MIGRATION_COST])
        statistics[REASSIGNS_COST]    = reassignment_cost(files[REASSIGNS_COST])
        statistics[OVERHEAD_COST]     = overhead_cost(files[OVERHEAD_COST])

        # Overhead
        statistics[OVERHEAD]          = overhead(files[OVERHEAD])

        # Store statistics
        store_stats(statistics)

    except argparse.ArgumentError as exc:
        print(exc)

if __name__ == "__main__":
    main()
