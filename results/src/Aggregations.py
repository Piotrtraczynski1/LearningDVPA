from collections.abc import Collection, Mapping

import pandas as pd
import numpy as np


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
    
def printMeanDecrease(
    path: str,
    filters: Mapping[str, object] | None = None
) -> np.float64:
    meanEQ = meanBy(pd.read_csv(path),"useSrs","equivalenceQueryCount",filters)
    meanDecrease = (meanEQ[0] - meanEQ[1])/meanEQ[0]
    print(f"Average procentage reduction in equivalence queries {meanDecrease*100:.2f}%")

def printMeanLearningTimeDecrease(
    path: str,
    filters: Mapping[str, object] | None = None
) -> np.float64:
    meanTime = meanBy(pd.read_csv(path),"useSrs","learnerTimeUs",filters)
    meanDecrease = (meanTime[0] - meanTime[1])/meanTime[0]
    print(f"Average procentage reduction in learning time {meanDecrease*100:.2f}%")
    
def printMeanAccuracyGain(
    path: str
) -> None:
    meanAccuracy = meanBy(pd.read_csv(path),"useSrs","equivalenceQueryCount")
    meanAccuracyGain = (meanEQ[0] - meanEQ[1])/meanEQ[0]
    print(f"Average procentage reduction in equivalence queries {meanDecrease*100:.2f}%")    

def printAccuracyStats(
    data: pd.DataFrame,
    comment: str
) -> None:
    series = data.groupby(["useSrs","status"])["runIndex"].count()   
    accuracyNoSRS = series[(0,'passed')]/(series[(0,'passed')]+series[(0,'validation_failed')])
    accuracyWithSRS = series[(1,'passed')]/(series[(1,'passed')]+series[(1,'validation_failed')])
    
    print(f"Accuracy without SRS {accuracyNoSRS*100:.2f}% and with SRS {accuracyWithSRS*100:.2f}% {comment}")
    print(f"Average accuracy gain {((accuracyWithSRS - accuracyNoSRS)/accuracyNoSRS)*100:.2f}% {comment}")
    
        
def printAccuracy(
    path: str
) -> None:
    data = pd.read_csv(path)
    dataSmall = data[data["numOfStates"]<=15]
    dataBig = data[data["numOfStates"]>20]
    
    printAccuracyStats(data, " (all automata)")
    print()
    printAccuracyStats(dataSmall, " (automata with at most 15 states)")
    print()
    printAccuracyStats(dataBig, " (automata with more than 20 states)")
