#!/usr/bin/env python

import random

DATA = []
HALO_DATA = []
MOV = []
AVG = []
ROWS = 5
COLUMNS = 10
HALO_COLUMNS = 8

def initData():
    zeros = []

    for i in range(0, COLUMNS):
        zeros.append(0.0)
        AVG.append(0.0)
                
    for i in range(0, ROWS):
        row = []
        haloRow = []

        for j in range(0, HALO_COLUMNS):
            haloRow.append(0.0)

        for j in range(0, COLUMNS):
            cell = random.random() * 100
            row.append(cell)
            haloRow.append(cell)
            
        DATA.append(row)
        HALO_DATA.append(haloRow)
        MOV.append(zeros)
                
def showData():
    for y in range(0, ROWS):
        for x in range(0, COLUMNS):
            print "%07.4f " % DATA[y][x],

        print
    print

def showHaloData():
    for y in range(0, ROWS):
        for x in range(0, COLUMNS + HALO_COLUMNS):
            print "%07.4f " % HALO_DATA[y][x],

        print
    print

def showMov():
    for y in range(0, ROWS):
        for x in range(0, COLUMNS):
            print "%07.4f " % MOV[y][x],

        print
    print

def showColAverage():
    for x in range(0, COLUMNS):
        print "%07.4f " % AVG[x],

def calculateMovingAverage():
    for y in range(0, ROWS):
        movRow = []
        for x in range(0, COLUMNS):            
            movCell  =  (HALO_DATA[y][x + HALO_COLUMNS - 0] + HALO_DATA[y][x + HALO_COLUMNS - 1] +
                         HALO_DATA[y][x + HALO_COLUMNS - 2] + HALO_DATA[y][x + HALO_COLUMNS - 3] +
                         HALO_DATA[y][x + HALO_COLUMNS - 4] + HALO_DATA[y][x + HALO_COLUMNS - 5] +
                         HALO_DATA[y][x + HALO_COLUMNS - 6] + HALO_DATA[y][x + HALO_COLUMNS - 7] +
                         HALO_DATA[y][x + HALO_COLUMNS - 8]) / 9.0
            movRow.append(movCell)
        MOV[y] = movRow

def calculateColumnAverage():
    for x in range(0, COLUMNS):
        colSum = 0
        for y in range(0, ROWS):
            colSum = colSum + DATA[y][x]

        colAvg = colSum / ROWS
        
        AVG[x] = colAvg
    
if __name__ == "__main__":
    initData()
    showData()
    showHaloData()    
    calculateMovingAverage()
    calculateColumnAverage()
    showMov()
    showColAverage()
