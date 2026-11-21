#!/usr/bin/env python3
"""Create a small SVG comparison using only Python's standard library.

This script intentionally does not treat a filled queue as a demonstrated CVE.
"""
import html
import pathlib
import re
import sys


def read_result(path: pathlib.Path):
    text = path.read_text(encoding="utf-8")
    lines = re.findall(r"^RESULT (.+)$", text, flags=re.MULTILINE)
    if len(lines) != 1:
        raise ValueError(f"Expected one RESULT line in {path}")
    return dict(re.findall(r"(\w+)=(\S+)", lines[0]))


def generate_svg(results):
    width, height = 850, 420
    plot_start, plot_end = 70, 790
    max_messages = max(1, max(int(r["messages"]) for _, r in results))
    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
             '<rect width="100%" height="100%" fill="#ffffff"/>',
             '<text x="34" y="36" font-size="22" font-family="sans-serif" fill="#0f172a">Message queue capacity: bounded tests</text>',
             '<text x="34" y="60" font-size="13" font-family="sans-serif" fill="#475569">Each bar shows messages accepted before EAGAIN (separate IPC APIs)</text>']
    bar_top, gap, bar_height = 98, 75, 30
    for i, (label, record) in enumerate(results):
        msg_count = int(record["messages"])
        length = (plot_end - plot_start - 170) * msg_count / max_messages
        y = bar_top + i * gap
        parts.append(f'<text x="{plot_start}" y="{y-10}" font-size="14" font-family="sans-serif" fill="#111827">{html.escape(label)}</text>')
        parts.append(f'<rect x="{plot_start}" y="{y}" width="{length:.1f}" height="{bar_height}" rx="4" fill="#2c65ac"/>')
        parts.append(f'<text x="{plot_start + length + 12:.1f}" y="{y+21}" font-size="14" font-family="sans-serif" fill="#111827">{msg_count} messages</text>')
    parts.append('<text x="34" y="400" font-size="12" font-family="sans-serif" fill="#64748b">Not a performance benchmark. Different message sizes and APIs are not directly comparable.</text>')
    parts.append('</svg>')
    return '\n'.join(parts) + '\n'


def main():
    if len(sys.argv) != 2:
        print("Usage: plot_results.py RESULTS_RUN_DIRECTORY", file=sys.stderr)
        return 2
    folder = pathlib.Path(sys.argv[1])
    inputs = [("System V: 4096-byte queue", "sysv_limits_4096.log"),
              ("System V: 2048-byte queue", "sysv_limits_2048.log"),
              ("POSIX: 4-message queue", "posix_limits_4.log"),
              ("POSIX: 2-message queue", "posix_limits_2.log")]
    results = [(name, read_result(folder / filename)) for name, filename in inputs]
    output = folder / "limit_comparison.svg"
    output.write_text(generate_svg(results), encoding="utf-8")
    print(f"Graph created: {output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
