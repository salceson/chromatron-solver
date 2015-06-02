#!/usr/bin/env python

# judge.py
# APTO2015

from sys import *


viewer = True

if( viewer ):
  import Image
  import ImageDraw




# Opis mozliwych urzadzen


# nazwy urzadzen i liczby typow
NAMES = ["LU", "LP", "ES", "LK", "RU", "BL", "EM", "TG" ]
TYPES = [  8 ,  4  ,  8  ,   8 ,   4 ,  1  ,   1 ,   1  ]   


# kierunki w ramach bloku
# 7  6  5
# 0     4
# 1  2  3

direction = [(-1,0),(-1,1),(0,1),(1,1),(1,0),(1,-1),(0,-1),(-1,-1)]
dir_translate = [4, 5, 6, 7, 0, 1, 2, 3]

#       0   1   2   3   4   5   6   7   8 
ru0 = [[4],[ ],[ ],[ ],[0],[ ],[ ],[ ]] 
ru1 = [[ ],[5],[ ],[ ],[ ],[1],[ ],[ ]] 
ru2 = [[ ],[ ],[6],[ ],[ ],[ ],[2],[ ]] 
ru3 = [[ ],[ ],[ ],[7],[ ],[ ],[ ],[3]] 

RU = [ru0,ru1,ru2,ru3]


#
em0 = [[4],[5],[6],[7],[0],[1],[2],[3]]
EM = [em0]

#
tg0 = [[4],[5],[6],[7],[0],[1],[2],[3]]
TG = [tg0]

#       0   1   2   3   4   5   6   7
lu0 = [[ ],[7],[ ],[ ],[ ],[ ],[ ],[1]]
lu1 = [[2],[ ],[0],[ ],[ ],[ ],[ ],[ ]]
lu2 = [[ ],[3],[ ],[1],[ ],[ ],[ ],[ ]]
lu3 = [[ ],[ ],[4],[ ],[2],[ ],[ ],[ ]]
lu4 = [[ ],[ ],[ ],[5],[ ],[3],[ ],[ ]]
lu5 = [[ ],[ ],[ ],[ ],[6],[ ],[4],[ ]]
lu6 = [[ ],[ ],[ ],[ ],[ ],[7],[ ],[5]]
lu7 = [[6],[ ],[ ],[ ],[ ],[ ],[0],[ ]]

LU  = [lu0, lu1, lu2, lu3, lu4, lu5, lu6, lu7]



#       0   1   2   3   4   5   6   7
lk0 = [[7],[6],[ ],[ ],[ ],[ ],[1],[0]]
lk1 = [[1],[0],[7],[ ],[ ],[ ],[ ],[2]]
lk2 = [[3],[2],[1],[0],[ ],[ ],[ ],[ ]]
lk3 = [[ ],[4],[3],[2],[1],[ ],[ ],[ ]]
lk4 = [[ ],[ ],[5],[4],[3],[2],[ ],[ ]]
lk5 = [[ ],[ ],[ ],[6],[5],[4],[3],[ ]]
lk6 = [[ ],[ ],[ ],[ ],[7],[6],[5],[4]]
lk7 = [[5],[ ],[ ],[ ],[ ],[0],[7],[6]]

LK  = [lk0, lk1, lk2, lk3, lk4, lk5, lk6, lk7]




#       0   1     2   3     4   5     6   7
lp0 = [[4],[5,7],[ ],[5,7],[0],[1,3],[ ],[1,3]]
lp1 = [[2,4],[5],[0,6],[],[0,6],[1],[2,4 ],[]]
lp2 = [[],[3,5],[6],[1,7],[],[1,7],[2],[3,5]]
lp3 = [[4,6],[],[4,6],[7],[0,2],[],[0,2],[3]]

LP  = [lp0, lp1, lp2, lp3]


# 
es0 = [ [],[],[],[],[],[],[],[] ]
es1 = [ [],[],[],[],[],[],[],[] ]
es2 = [ [],[],[],[],[],[],[],[] ]
es3 = [ [],[],[],[],[],[],[],[] ]
es4 = [ [],[],[],[],[],[],[],[] ]
es5 = [ [],[],[],[],[],[],[],[] ]
es6 = [ [],[],[],[],[],[],[],[] ]
es7 = [ [],[],[],[],[],[],[],[] ]

ES = [es0,es1,es2,es3,es4,es5,es6,es7]


#
bl0 = [ [],[],[],[],[],[],[],[] ]

BL = [bl0]




propagate = {"LU":LU, "LP":LP, "EM":EM, "ES":ES, "BL":BL, "TG":TG, "RU":RU, "LK":LK}








##########################################
#
# funkcje pomocnicze


def error( s ):
  print "BLAD: %s" % s 
  exit(-1)


def report( s ):
  return


##########################################
#
# wczytywanie planszy



# wczytaj plansze
# zwraca (w,h,n, urzadzenia)
def readBoard( f ):
  try:
    s = f.readline()
    (W,H) = s.split()    
    W = int(W)
    H = int(H)
    N = int(f.readline())
    devices = []
    for i in range(N):
      dev = f.readline().split()
      dev[1] = int(dev[1])
      dev[2] = int(dev[2])
      dev[3] = int(dev[3])
      devices += [dev]

    return (W,H,N, devices)

  except: 
   error( "Blad podczas wczytywania planszy" )  





# tworzy opis planszy na podstawie wejscia z pliku
# 
# pojedyncza komorka planszy
# (typ, kierunek, swiatlo[3], parametr)
#
def createBoard( B ):
  W = B[0]
  H = B[1]
  N = B[2]
  devices = B[3]
  unused = []
  board = []

  # stworz pusta plansze bez swiatla
  for h in range(H+2):
    row = []
    for w in range(W+2):
      light = [[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0]]
      field = ["EM", 0, light, ""]
      row += [field]
    board += [row]

  # licznik nieuzytych urzadzen
  count = {}
  for n in NAMES:
    count[n] = 0

  # zrodla swiatla
  lights = []



  # umiesc urzadzenia na planszy
  for dev in devices:  
    t = dev[0] # typ
    x = dev[1]
    y = dev[2]
    d = dev[3] # kierunek

    if( x == 0 and y == 0 ):
      count[t] += 1
    else:
      if( board[y][x][0] != "EM" ):
        error( "Bledny opis planszy: dwa urzadzenia w tym samym miejscu" )
      board[y][x][0] = t
      board[y][x][1] = d
      board[y][x][3] = dev[4]

       # oswietlenie lasera
      try:
        if( t == "ES" ):
          board[y][x][3] = dev[4]
          for c in range(3):
            if( dev[4][c] == "1" ):
              lights += [(x,y,d,c)]    # opis oswietlania: wspolrzedne, kierunek skad swieci, kolor
              board[y][x][2][c][d] = 1
      except:
        error( "Bledny opis swiatla lasera" )
        
        

  return (W,H,board, count, lights)




# sprawdza, czy plansza wejsciowa i z rozwiazaniem
# sa ze soba zgodne
def boardsConsistent( Bin, Bsol ):
  W = Bin[0]
  H = Bin[1]
  report( "Sprawdzam plansze" )
  if( W != Bsol[0] or H != Bsol[1] ):
    error("Niezgodny rozmiar plansz")

  board_in  = Bin[2]
  board_sol = Bsol[2]
  count_in  = Bin[3]

  # sprawdz, czy zgadzaja sie urzadzenia
  for y in range(1,H+1):
    for x in range(1,W+1):
      if( board_in[y][x][0] != board_sol[y][x][0] ):
        # niezgodne urzadzenia: mozliwe tylko, jesli na wejsciu bylo puste miejsce
        if( board_in[y][x][0] == "EM" ):
          count_in[ board_sol[y][x][0] ] -= 1
        else:
          error("Niezgone pola (rozne urzadzenia: (%d,%d))" % (x,y))

      elif( board_in[y][x][1] != board_sol[y][x][1] ):
        # pola zgodne, ale kierunek nie
        error("Niezgodne pola w rozwiazaniu i wejsciu (rozny kierunek urzadzen (%d,%d))" % (x,y))

      elif( board_in[y][x][3] != board_sol[y][x][3] ):
        # niezgodne parametry -- np. kolor lasera
        error("Niezgodne parametry pola w rozwiazaniu i wejsciu (rozny kolor (%d,%d))" % (x,y))

  # czy nie uzyto za duzo urzadzen?
  for n in NAMES:
    if( count_in[n] < 0 ):   
      error("Uzyto za duzo urzadzen")

  return True




##########################################
#
# generowanie przebiegu swiatla




def computeLights( B ):
  W = B[0]
  H = B[1]
  board = B[2]
  lights = B[4]

  while ( lights ):
    (x, y, d, c) = lights.pop(0)


    # sprawdz gdzie swiecimy
    (dx,dy) = direction[d]
    x += dx
    y += dy
    d = dir_translate[d] # oswietlenie nowego pola z kierunku

    if( (x == 0) or (y == 0) or (x == W+1) or (y == H+1) ):
      continue  # swiatlo poza plansza

    # generuj przejscie swiatla po komorce planszy
    board[y][x][2][c][d] = 1

    prop = propagate[ board[y][x][0] ][ board[y][x][1] ][d]
    for nd in prop:
      if( board[y][x][2][c][nd] != 1 ):
        board[y][x][2][c][nd] = 1
        lights.append( (x,y,nd,c) )




def checkTargets( B ):
  W = B[0]
  H = B[1]
  board = B[2]

  for y in range(1,H+1):
    for x in range(1,W+1):
      if( board[y][x][0] == "TG" ):
        color = "000"
        for d in range(8):
          if( board[y][x][2][0][d] == 1 ):
            color = "1"+color[1:]
          if( board[y][x][2][1][d] == 1 ):
            color = color[0]+"1"+color[2]
          if( board[y][x][2][2][d] == 1 ):
            color = color[0:2]+"1"

        if( board[y][x][3] != color ):
          return False

  return True

  
  





##########################################
#
# rysowanie planszy




def printBoard( B ):
  W = B[0]
  H = B[1]
  board = B[2]           
  for y in range(1,H+1):
    s = ""
    for x in range(1,W+1):
      s += board[y][x][0] + " "
    print s
 



def loadImages( ):
  imgs = {}
  try:
    for n in range(len(NAMES)):
      for j in range(TYPES[n]): 
        name = NAMES[n]+str(j)
        imgs[ name ] = Image.open( "img/"+name+".png" )
  except:
    error( "Blad wczytywania obrazow urzadzen." ) 
  return imgs





def drawBoard( B ):
  devW = 26
  devH = 26

  W = B[0]
  H = B[1]
  board = B[2]

  report( "Generowanie obrazu planszy" )
  imgs = loadImages()

  boardImg = Image.new( "RGB", (devW*W, devH*H) )
  draw = ImageDraw.Draw( boardImg )


  # wypelnij plansze urzadzeniami
  for y in range(1,H+1):
    for x in range(1,W+1):
      device = board[y][x][0] + str(board[y][x][1])
      boardImg.paste( imgs[ device ], ((x-1)*devW, (y-1)*devH) )
      if( board[y][x][0]=="TG" ):
        r = int(board[y][x][3][0])
        g = int(board[y][x][3][1])
        b = int(board[y][x][3][2])
        draw.ellipse( ( (x-1)*devW+3, (y-1)*devH+3 ,(x)*devW-4, (y)*devH-4), fill=(255*r,255*g,255*b))

  # narysuj przebieg swiatla
  for y in range(1,H+1):
    for x in range(1,W+1):
      for d in range(8):
        if( board[y][x][2][0][d] == 0 and board[y][x][2][1][d] == 0 and board[y][x][2][2][d] == 0 ):
          continue
        rx = x-1
        ry = y-1
        color = (255*board[y][x][2][0][d],255*board[y][x][2][1][d],255*board[y][x][2][2][d])

        cx = rx*devW + devW/2
        cy = ry*devH + devH/2
        tx = cx+direction[d][0]*devW/2
        ty = cy+direction[d][1]*devH/2
        for a in [-1,0,1]:
          for b in [-1,0,1]:
            draw.line( [cx+a,cy+b, tx+a, ty+b], fill=color, width=1 )
  
 
  # to be implemented

  boardImg.show()
  return boardImg



##########################################
#
# program glowny



def main():
  if( len(argv) != 3 ):
    error("Wywolanie: judge.py input output")

  try:
    f = open( argv[1], "r" )
    board_input = readBoard( f )
  except:
    error("Nie udalo sie wczytac planszy z opisem problemu")

  try:
    f = open( argv[2], "r" )
    board_solution = readBoard( f )
  except:
    error("Nie udalo sie wczytac planszy z opisem rozwiazania")

  Bin  = createBoard( board_input )
  Bsol = createBoard( board_solution )
  boardsConsistent( Bin, Bsol )

  # na tym etapie wiemy, ze rozwiazanie jest poprawna plansza dla zadania
  # i trzeba obliczyc oswietlenie

  computeLights( Bsol )

  # oswietlenie obliczone, pora narysowac plansze
  if( viewer ):
    image = drawBoard( Bsol )
    image.save( argv[2]+".png" )
  if( checkTargets( Bsol ) ):
    print "OK"
    exit(0)
  else: 
    print "Cele nieoswietlone"
    exit(-1)



try:
  main()
except Exception:
  error("Nieznany blad sedziego")


