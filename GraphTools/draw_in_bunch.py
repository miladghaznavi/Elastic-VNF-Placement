import subprocess
import argparse
import sys
import os.path

SUCCESS_CODE = 0
DRAW_CMD1 = "python ./draw.py -i {i} -m {m} -x {x} -y {y} -s {s}"
DRAW_CMD2 = "python ./draw.py -i {i} -m {m} -x {x} -y {y} -n {n}"
DRAW_CMD3 = "python ./draw.py -i {i} -m {m} -x {x} -y {y}"
SAMPLING = 10
WINDOW = 200  # 200 seconds

COST_DOMAIN = 'cost'
UTIL_DOMAIN = 'util'
ACCP_DOMAIN = 'acceptance'

run_dict = {
    COST_DOMAIN:[
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'vnfs',
            's': SAMPLING
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'migrations',
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'reassigns',
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'totalCost',
            'n': WINDOW
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'installCost',
            'n': WINDOW
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'transCost',
            'n': WINDOW
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'migCost',
            'n': WINDOW
        },
        {
            # 'i': PATH,
            'm': 'cost',
            'x': 'time',
            'y': 'reasgnCost',
            'n': WINDOW
        }
    ],
    UTIL_DOMAIN: [
        {
            # 'i': PATH,
            'm': 'util',
            'x': 'time',
            'y': 'facilityUtil',
            's': SAMPLING
        },
        {
            # 'i': PATH,
            'm': 'util',
            'x': 'time',
            'y': 'allHostUtil',
            's': SAMPLING
        },
        {
            # 'i': PATH,
            'm': 'util',
            'x': 'time',
            'y': 'allLinksUtil',
            's': SAMPLING
        },
    ],
    ACCP_DOMAIN: [
        {
            # 'i': PATH,
            'm': 'acceptance',
            'x': 'time',
            'y': 'demands',
        },
        {
            # 'i': PATH,
            'm': 'acceptance',
            'x': 'time',
            'y': 'arrAccRatio',
            's': SAMPLING
        },
        {
            # 'i': PATH,
            'm': 'acceptance',
            'x': 'time',
            'y': 'rejected',
            's': SAMPLING
        },
    ]
}


def def_parser():
    parser = argparse.ArgumentParser(description='Draw in bunch')
    parser.add_argument('-i', '--input', dest='i',
                        help='Input folder',
                        type=str, required=True)
    parser.add_argument('-c', '--cost', dest='c',
                        help='Draw cost grpahs',
                        action='store_true', default=None)
    parser.add_argument('-u', '--util', dest='u', help='Utilization graphs',
                        action='store_true', default=None)
    parser.add_argument('-p', '--accept', dest='p', help='Acceptance',
                        action='store_true', default=None)
    parser.add_argument('-a', '--all', dest='a', help='Draw all graphs',
                        action='store_true', default=None)
    parser.add_argument('-e', '--exclude', dest='e', help='exclude these floders',
                        type=str, default=None)
    return parser


def parse_args(parser):
    opts = vars(parser.parse_args(sys.argv[1:]))
    if not os.path.isdir(opts['i']):
        raise Exception('Directory "%s" does not exists!' % opts['i'])

    if opts['c'] is None and \
                    opts['u'] is None and \
                    opts['p'] is None and \
                    opts['a'] is None:
        opts['a'] = True

    if opts['e'] != None:
        opts['e'] = opts['e'].lower().split(',')
    return opts


def run_cmd(cmd):
    process = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    process.wait()
    if process.returncode == SUCCESS_CODE:
        print process.stdout.read()
    else:
        print process.stderr.read()


def main():
    try:
        args = parse_args(def_parser())
        for domain in run_dict:
            if not args['a'] and (domain == COST_DOMAIN and not args['c']):
                continue
            if not args['a'] and (domain == UTIL_DOMAIN and not args['u']):
                continue
            if not args['a'] and (domain == ACCP_DOMAIN and not args['p']):
                continue

            for dd in run_dict[domain]:
                tmp = dd
                tmp['i'] = args['i']
                try:
                    cmd = DRAW_CMD1.format(**tmp)
                except KeyError:
                    try:
                        cmd = DRAW_CMD2.format(**tmp)
                    except KeyError:
                        cmd = DRAW_CMD3.format(**tmp)
                if args['e'] is not None:
                    cmd += ' -e ' + ','.join(args['e'])
                print cmd
                run_cmd(cmd)

    except argparse.ArgumentError as exc:
        print(exc)


if __name__ == "__main__":
    main()
