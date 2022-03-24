class No:
    def __init__(self, proximo_no=None):
        self.estado = [0]*81
        self.proximo = proximo_no

    def __repr__(self):
        return '%s' % (self.estado)
    
    
def linha(sudoku, key, x):
    if  key not in sudoku[x, :]:
        return True
    return False

def coluna(sudoku, key, y):
    if  key not in sudoku[:, y]:
        return True
    return False

def quadrante(sudoku, key, x, y):
    quad_x = x // 3
    quad_y = y // 3
    
    if key not in sudoku[quad_x * 3: (quad_x + 1) * 3, quad_y * 3: (quad_y + 1) * 3]:
        return True
    return False

def valido(sudoku, key, x, y):
    if linha(sudoku, key, x):
        if coluna(sudoku, key, y):
            if quadrante(sudoku, key, x, y):
                return True
    return False

def parser(sudoku, estado):
    for i in range(0, 81):
        if sudoku[i] == '.':
            estado[i] = 0
            
        else:
            estado[i] = int(sudoku[i])  
    
def imprimir(sudoku):
    print(sudoku)
    print("")
    
def done(sudoku):
    if 0 not in sudoku:
        return True
    return False

def possibilidades(sudoku, x, y):
    nums = list()
    
    for i in range(1, 10):
        if valido(sudoku, i, x, y):
            nums.append(i)
            
    return nums