from collections.abc import Callable
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


Transform = Callable[[pd.DataFrame], pd.Series]
RESULTS_DIR = Path(__file__).resolve().parents[1]


class Plot:
    def __init__(self, title: str, xLabel: str, yLabel: str) -> None:
        self.title = title
        self.xLabel = xLabel
        self.yLabel = yLabel
        self.lines: list[tuple[pd.Series, str]] = []

    def add(self, csvPath: str | Path, transform: Transform, label: str) -> None:
        data = pd.read_csv(RESULTS_DIR / csvPath)
        self.lines.append((transform(data).sort_index(), label))

    def draw(self) -> None:
        figure, axis = plt.subplots(figsize=(10, 6))

        for data, label in self.lines:
            axis.plot(
                data.index,
                data,
                marker="o",
                label=label,
            )

        axis.set_title(self.title)
        axis.set_xlabel(self.xLabel)
        axis.set_ylabel(self.yLabel)
        axis.grid(True, alpha=0.3)
        axis.legend()
        figure.tight_layout()
        plt.show()
