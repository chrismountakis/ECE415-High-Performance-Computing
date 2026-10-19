"""Measure every version in src/ and write the results spreadsheet.

Works like benchmark.py: every program is run once as a warm-up (not counted),
then RUNS times; the fastest and slowest run are dropped and the mean and
standard deviation of the rest are reported.

The source folders are not touched. Each folder is copied to a temporary
directory together with input.grey / golden.grey from data/, built there
with its own makefile (common/ twice: OPT=-O0 and the default -O3 -ffast-math),
and every run is checked for an exact result (PSNR = inf).

Output: results/HPC_Lab1_results.xlsx, with one sheet per series
(all individual runs, mean, std and speedup as spreadsheet formulas, and a
chart) and a summary sheet with the mean, std and speedups of every version.

    python3 scripts/benchmark.py            # measure and write
    python3 scripts/benchmark.py --rebuild  # rewrite from the runs already in the file
"""
import os
import re
import shutil
import statistics
import subprocess
import sys
import tempfile

from openpyxl import Workbook, load_workbook
from openpyxl.chart import BarChart, Reference
from openpyxl.chart.error_bar import ErrorBars
from openpyxl.chart.data_source import NumDataSource, NumRef
from openpyxl.styles import Alignment, Font, PatternFill
from openpyxl.utils import get_column_letter

RUNS = 12
LAB = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(LAB, "src")
DATA = os.path.join(LAB, "data")
OUT = os.path.join(LAB, "results", "HPC_Lab1_results.xlsx")
TIME_RE = re.compile(r"Total time\s*=\s*([0-9.eE+-]+)\s*seconds")

# (sheet name, folder, make arguments, baseline mean in ms for the speedup column)
SERIES = [
    ("Common -O0",             "common", ["OPT=-O0"], None),
    ("Common -O3 -ffast-math", "common", [],          None),
    ("O0 chain",               "O0",     [],          "Common -O0"),
    ("O3 chain",               "O3",     [],          "Common -O3 -ffast-math"),
]

def programs_from_makefile(path):
    with open(path) as f:
        for line in f:
            if line.startswith("EXECUTABLES"):
                return line.split("=", 1)[1].split()
    raise ValueError(f"No EXECUTABLES line found in {path}")


def run_once(workdir, prog):
    out = subprocess.run([f"./{prog}"], cwd=workdir, capture_output=True, text=True, check=True).stdout
    if "inf" not in out:
        sys.exit(f"{prog}: output is not exact (PSNR != inf)\n{out}")
    return float(TIME_RE.search(out).group(1)) * 1000.0


def measure():
    results = {}
    with tempfile.TemporaryDirectory() as tmp:
        for name, folder, make_args, _ in SERIES:
            work = os.path.join(tmp, name.replace(" ", "_"))
            shutil.copytree(os.path.join(SRC, folder), work)
            for g in ("input.grey", "golden.grey"):
                shutil.copy(os.path.join(DATA, g), work)
            subprocess.run(["make", "clean"], cwd=work, capture_output=True)
            subprocess.run(["make", *make_args], cwd=work, check=True, capture_output=True)
            results[name] = {}
            for prog in programs_from_makefile(os.path.join(work, "makefile")):
                run_once(work, prog)                      # warm-up, not counted
                times = [run_once(work, prog) for _ in range(RUNS)]
                results[name][prog] = times
                kept = sorted(times)[1:-1]
                print(f"{name:24s} {prog:30s} mean {statistics.mean(kept):9.3f} ms, "
                      f"std {statistics.stdev(kept):7.3f} ms", flush=True)
    return results


def kept_stats(times):
    kept = sorted(times)[1:-1]
    return statistics.mean(kept), statistics.stdev(kept)


def load_runs(path):
    """Read the individual runs back from an existing results workbook."""
    wb = load_workbook(path)
    results = {}
    for name, _, _, _ in SERIES:
        ws = wb[name[:31]]
        results[name] = {}
        for row in ws.iter_rows(min_row=2, max_col=1 + RUNS, values_only=True):
            if row[0] and all(isinstance(v, (int, float)) for v in row[1:]):
                results[name][row[0]] = list(row[1:])
    return results


def write_workbook(results):
    wb = Workbook()
    wb.remove(wb.active)
    bold = Font(bold=True)
    head = PatternFill("solid", fgColor="DDEBF7")
    first_run, last_run = 2, 1 + RUNS                    # columns B .. M
    c_mean, c_std, c_speed = last_run + 1, last_run + 2, last_run + 3
    L = get_column_letter

    # Mean, std and speedups are written as numbers, not formulas: openpyxl stores
    # no computed values and Numbers does not recalculate them on open.
    stats = {name: {p: kept_stats(t) for p, t in results[name].items()} for name, *_ in SERIES}
    baseline = {name: stats[base or name]["00_sobel_orig"][0] for name, _, _, base in SERIES}

    for name, folder, _, base in SERIES:
        ws = wb.create_sheet(name[:31])
        header = ["Executable"] + [f"Run {i}" for i in range(1, RUNS + 1)] + \
                 ["Mean (ms)", "Std. dev. (ms)", "Speedup vs baseline"]
        ws.append(header)
        for c in range(1, len(header) + 1):
            ws.cell(1, c).font = bold
            ws.cell(1, c).fill = head
            ws.cell(1, c).alignment = Alignment(horizontal="center", wrap_text=True)
        for prog, times in results[name].items():
            mean, std = stats[name][prog]
            ws.append([prog] + [round(t, 3) for t in times] +
                      [round(mean, 3), round(std, 3), round(baseline[name] / mean, 2)])
            r = ws.max_row
            for c in range(first_run, c_std + 1):
                ws.cell(r, c).number_format = "0.000"
            ws.cell(r, c_speed).number_format = '0.00"×"'
        ws.column_dimensions["A"].width = 30
        for c in range(first_run, c_speed + 1):
            ws.column_dimensions[L(c)].width = 11
        ws.freeze_panes = "B2"
        ws.cell(ws.max_row + 2, 1,
                f"Each version: 1 warm-up run (not counted), then {RUNS} runs. "
                "Mean and std. dev. use the runs left after dropping the fastest and the slowest. "
                "Speedup is relative to 00_sobel_orig at the same compiler flags.")

        rows = ws.max_row - 2
        chart = BarChart()
        chart.type = "bar"
        chart.style = 10
        chart.title = f"Average runtime, 4096x4096 ({name})"
        chart.y_axis.title = "Time (ms)"
        chart.x_axis.title = None
        data = Reference(ws, min_col=c_mean, min_row=1, max_row=rows)
        cats = Reference(ws, min_col=1, min_row=2, max_row=rows)
        chart.add_data(data, titles_from_data=True)
        chart.set_categories(cats)
        std_ref = f"'{ws.title}'!${L(c_std)}$2:${L(c_std)}${rows}"
        err = NumDataSource(numRef=NumRef(f=std_ref))
        chart.series[0].errBars = ErrorBars(errDir="y", errBarType="both", errValType="cust",
                                            plus=err, minus=err)
        chart.legend = None
        chart.height = 2 + 0.8 * (rows - 1)
        chart.width = 24
        ws.add_chart(chart, f"A{ws.max_row + 3}")

    # summary: every kept version with its mean, std and speedups
    ws = wb.create_sheet("Summary", 0)
    cols = ["Series", "Executable", "Mean (ms)", "Std. dev. (ms)",
            "Speedup vs previous step", "Speedup vs baseline"]
    ws.append(cols)
    for c in range(1, len(cols) + 1):
        ws.cell(1, c).font = bold
        ws.cell(1, c).fill = head
        ws.cell(1, c).alignment = Alignment(horizontal="center", wrap_text=True)
    for name, _, _, base in SERIES:
        prev = None
        if base:                                         # a chain continues from 08
            prev = stats[base][list(stats[base])[-1]][0]
        for prog, (mean, std) in stats[name].items():
            ws.append([name, prog, round(mean, 1), round(std, 2),
                       round(prev / mean, 2) if prev else None,
                       round(baseline[name] / mean, 2)])
            r = ws.max_row
            ws.cell(r, 3).number_format = "0.0"
            ws.cell(r, 4).number_format = "0.00"
            ws.cell(r, 5).number_format = '0.00"×"'
            ws.cell(r, 6).number_format = '0.00"×"'
            prev = mean
    ws.cell(ws.max_row + 2, 1,
            "Baseline: 00_sobel_orig at the same compiler flags. The O0 and O3 chains "
            "continue from 08_sobel_int_psnr of the matching common series.")
    ws.column_dimensions["A"].width = 24
    ws.column_dimensions["B"].width = 30
    for c in "CDEF":
        ws.column_dimensions[c].width = 16
    wb.save(OUT)


def main():
    if "--rebuild" in sys.argv:                          # reuse the runs already in OUT
        results = load_runs(OUT)
    else:
        for g in ("input.grey", "golden.grey"):
            if not os.path.exists(os.path.join(DATA, g)):
                sys.exit(f"missing {os.path.join(DATA, g)}")
        load = os.getloadavg()[0]
        if load > 2.0:
            print(f"warning: load average is {load:.1f}, close other programs for stable numbers")
        results = measure()
    write_workbook(results)
    print(f"written {OUT}")


if __name__ == "__main__":
    main()
