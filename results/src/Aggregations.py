from collections.abc import Collection, Mapping

import pandas as pd


def _filterRows(
    data: pd.DataFrame,
    filters: Mapping[str, object] | None,
) -> pd.DataFrame:
    for column, value in (filters or {}).items():
        if isinstance(value, Collection) and not isinstance(value, (str, bytes)):
            data = data[data[column].isin(value)]
        else:
            data = data[data[column] == value]
    return data


def meanBy(
    data: pd.DataFrame,
    xColumn: str,
    yColumn: str,
    filters: Mapping[str, object] | None = None,
) -> pd.Series:
    data = _filterRows(data, filters)
    return data.groupby(xColumn)[yColumn].mean()


def successRatioBy(
    data: pd.DataFrame,
    xColumn: str,
    filters: Mapping[str, object] | None = None,
    statusColumn: str = "status",
    passedValue: str = "passed",
) -> pd.Series:
    data = _filterRows(data, filters)
    passedRows = data[statusColumn] == passedValue
    return passedRows.groupby(data[xColumn]).mean().mul(100).rename("successRatio")
