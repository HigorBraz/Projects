import numpy as np

from utils import *

def heuristica(sudoku):   
    for value in range(1, 10):
        for x in range(0, 9):
            for y in range(0, 9):
                if sudoku[x][y] == 0:
                    if len(possibilidades(sudoku, x, y)) == value:
                        return x, y, possibilidades(sudoku, x, y)
                
    return -1, -1, []


def revise(sudoku):
    for x in range(0, 9):
        for y in range(0, 9):
            if sudoku[x][y] == 0:
                if len(possibilidades(sudoku, x, y)) == 0:
                    return False
            
    return True


def backtracking(sudoku):
    if done(sudoku):
        imprimir(sudoku)
        return True
    
    x, y, possibilities = heuristica(sudoku)
    
    if x > -1 and y > -1:
        for value in possibilities:
            sudoku[x][y] = value
            
            if revise(sudoku):
                if backtracking(sudoku):
                    return True
                
            
            sudoku[x][y] = 0
                
    return False
    

def main():  
    arquivo = open('sudoku.txt', 'r')
    for line in arquivo:
        estado = [0]*81
        parser(line, estado)
        estado = np.array(estado).reshape([9, 9])
        backtracking(estado)
        
    
if __name__ == "__main__":
    main()