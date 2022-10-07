import os 
os.chdir('C:/Users/camom/Documents/CamilleSouvigny/Wumpus_GameSolver/lib')

from gopherpysat import Gophersat


def defi_voc(n=4):
    voc = []
    for i in range(0, n):
        for j in range(0, n):
            voc.append("P[" + str(i) + ", " + str(j) + "]")
            voc.append("W[" + str(i) + ", " + str(j) + "]")
            voc.append("B[" + str(i) + ", " + str(j) + "]")
            voc.append("S[" + str(i) + ", " + str(j) + "]")
    return voc


def liste_voisins(case, n=4):
    liste = []
    if int(case[0]) - 1 >= 0:
        liste.append([case[0] - 1, case[1]])
    if int(case[0]) + 1 < n:
        liste.append([case[0] + 1, case[1]])
    if int(case[1]) - 1 >= 0:
        liste.append([case[0], case[1] - 1])
    if int(case[1]) + 1 < n:
        liste.append([case[0], case[1] + 1])
    return liste


def definitionregles(gs,nn=4):
    regle = []
    gs.push_pretty_clause(["-P" + str([0, 0])])
    gs.push_pretty_clause(["-W" + str([0, 0])])
    for i in range(0, nn):
        for j in range(0, nn):
            case = [i, j]
            voisins = liste_voisins(case, nn)
            rg1 = [] #Puit entouré de brise
            rg1.append("-B" + str(case))
            rg2 = [] #umpus entouré de puanteur
            rg2.append("-S" + str(case))
            for k in voisins:
                rg1.append("P" + str(k))
                rg2.append("W" + str(k))
                regle.append(["-P" + str(k), "B" + str(case)])
                regle.append(["-W" + str(k), "S" + str(case)])
                regle.append(["B" + str(k), "-P" + str(case)])
                regle.append(["S" + str(k), "-W" + str(case)])
            regle.append(rg1)
            regle.append(rg2)

            rg3 =[] #wumpus unique
            rg3.append("W" + str(case))
            for k in range(0, nn):
                for l in range(0, nn):
                    c=[k,l]
                    if(k!=i and l!=j):
                        rg3.append("-W" + str(c))
                        regle.append(rg3)
    return regle

def ajout_clauses(gs,size):
    for clause in definitionregles(gs,size):
        gs.push_pretty_clause(clause)
        

    
def ajout_de_deduction_clauses(gs,b,case):
    if b.count('B'):
        gs.push_pretty_clause(["B[" + str(case[0]) + ", " + str(case[1]) + "]"])
    else:
        gs.push_pretty_clause(["-B[" + str(case[0]) + ", " + str(case[1]) + "]"])
    
    if b.count('S'):
        gs.push_pretty_clause(["S[" + str(case[0]) + ", " + str(case[1]) + "]"]) 
    else:
        gs.push_pretty_clause(["-S[" + str(case[0]) + ", " + str(case[1]) + "]"])
    
    if b.count('P'):
        gs.push_pretty_clause(["P[" + str(case[0]) + ", " + str(case[1]) + "]"])
    else:
        gs.push_pretty_clause(["-P[" + str(case[0]) + ", " + str(case[1]) + "]"])
    
    if b.count('W'):
        gs.push_pretty_clause(["W[" + str(case[0]) + ", " + str(case[1]) + "]"])
    else:
        gs.push_pretty_clause(["-W[" + str(case[0]) + ", " + str(case[1]) + "]"])
    



def cartographier(ww,gs,size=4):
    carte=size*[0]
    for i in range(len(carte)):
        carte[i]=size*[0]
    nbcaseconnu=0
    while(nbcaseconnu<size*size):
        #On cherche toutes les cases sur à tester avec probe()
        trouver=0
        for i in range(0,size):
            for j in range(0,size):
                case = [i, j]
                if (carte[i][j]==0) :
                    gs.push_pretty_clause(["P" + str(case),"W" + str(case)])                           
                    if (not gs.solve()): #case sur
                        trouver=1
                        gs.pop_clause()
                        a,b,c =ww.probe(i,j) #rempli case pour getKnowledge On le stocke autre part ?
                        print(a,b,c)
                        carte[i][j]=b
                        nbcaseconnu+=1
                        ajout_de_deduction_clauses(gs,b,case)
  
                    gs.pop_clause()
        
        #on cautious_probe une case si on a rien réussi à probe
        if (trouver==0):
            arret=False
            for i in range(0,size):            
                for j in range(0,size):
                    if (carte[i][j]==0 or carte[i][j]=='?'):
                        a,b,c =ww.cautious_probe(i,j)
                        print(a,b,c)
                        carte[i][j]=b
                        nbcaseconnu+=1
                        case=[i,j]
                        ajout_de_deduction_clauses(gs,b,case)
                        arret=True
                        break
                if arret:
                    break  
    return carte