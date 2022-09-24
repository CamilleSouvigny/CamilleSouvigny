from lib.wumpus_client import WumpusWorldRemote
from pprint import pprint
from requests.exceptions import HTTPError
import random


__author__ = "Sylvain Lagrue"
__copyright__ = "Copyright 2020, UTC"
__license__ = "LGPL-3.0"
__version__ = "0.2"
__maintainer__ = "Sylvain Lagrue"
__email__ = "sylvain.lagrue@utc.fr"
__status__ = "dev"



################################################################################################
######################## Fonctions de la phases 1 du projet ############################
######################################################################################################

from lib.gopherpysat import Gophersat
gophersat_exec="./lib/gophersat.exe"


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



################################################################################################
######################## Fonctions de la phases 2 du projet #################################
########################  Choix d'implémenter A*  ##########################################
#################################################################################################



def etat_finaux(carte):
    Final=[]
    for i in range(0,len(carte)):
        for j in range(0,len(carte)):   
            if carte[i][j].count('G'):
                if not (carte[i][j].count('W') or carte[i][j].count('P')):
                    Final.append((i,j))
    return Final


def liste_successeurs(case, liste, n=4):
    l= []
    if (int(case[0])-1>= 0 and not (liste[int(case[0])-1][case[1]].count('P') or liste[int(case[0])- 1][case[1]].count('W'))):
        l.append((case[0] - 1, case[1]))
    if (int(case[0]) + 1 < n and not (liste[int(case[0])+ 1][case[1]] .count('P') or liste[int(case[0]) + 1][case[1]].count('W'))):
        l.append((case[0] + 1, case[1]))
    if (int(case[1]) - 1 >= 0 and not (liste[case[0]][case[1] - 1].count('P') or liste[case[0]][case[1] - 1].count('W'))):
        l.append((case[0], case[1] - 1))
    if (int(case[1]) + 1 < n and not ( liste[case[0]][case[1] + 1].count('P') or liste[case[0]][case[1] + 1].count('W'))):
        l.append((case[0], case[1] + 1))
    return l


def heuristique(etat,final):
    l=abs(int(etat[0])-int(final[0]))+abs(int(etat[1])-int(final[1]))
    return l


def trier_liste(l):
    li=[]
    li=sorted(l)
    return li


def but_multiple_heuristique_manhattan(etat,Final):
    #distance à l'état final le plus proche
    destination=Final[0]
    temporair=abs(int(etat[0])-int(destination[0]))+abs(int(etat[1])-int(destination[1]))
    for final in Final:
        l=abs(int(etat[0])-int(final[0]))+abs(int(etat[1])-int(final[1]))
        if l<temporair:
            temporair=l
            destination=final
    return temporair,destination



  
def recuperer_chemin(chemin,out,predecesseur,initial):
    while(out!=initial):
        chemin.append(out)
        out=predecesseur[out]
    chemin.reverse()
    return chemin




def recherche_or(chemin,liste,initial,Final):

    l=[]
    predecesseur={}
    dParcourue={}
    dParcourue[initial]=0

    dist_final,destination=but_multiple_heuristique_manhattan(initial,Final)

    triplet_initial=(dist_final+dParcourue[initial],initial[0],initial[1])
    l.append(triplet_initial)

    while (l):
        l=trier_liste(l)
        out=l.pop(0)
        
        case=(out[1],out[2])
        for k in liste_successeurs(case,liste, len(liste)):
             
            if k not in predecesseur:
                dParcourue[k]=dParcourue[case]+1
                ajout=(heuristique(k,destination)+dParcourue[k],k[0],k[1])
                l.append(ajout)
                predecesseur[k]=case
                                                   
            if (k in Final):
                Final.remove(k)
                tmp=[]
                tmp=recuperer_chemin(tmp,k,predecesseur,initial)      
                for k in tmp:
                    chemin.append(k)
                
                
                if not len(Final)==0:
                    recherche_or(chemin,liste,k,Final)
            
                    
            if (len(Final)==0 or out[0] > len(liste)*len(liste)-1 ):
                return chemin


    a,b=but_multiple_heuristique_manhattan(initial,Final)
    if (len(l)==0 and b in Final) :
        print("CASE D'OR INACCESSIBLE")
        print(Final)
        Final.remove(b)
        print(Final)
        if len(Final)==0:
            return chemin
                
              


def course_vers_or(ww,chemin):
    for i,j in chemin: 
        status, msg, cost = ww.go_to(i,j)
        print(msg)
    
        if status == "[KO]":
            print("t'es mort !")
            print("press enter for the next maze !!")
            input()
            status, msg, size = ww.next_maze()
            continue
            


################################################################################################
#################################### TEST TEST TEST TEST #######################################
################################################################################################




def treat_map_1(wwr: WumpusWorldRemote):
    size = wwr.current_size

    ###################
    ##### PHASE 1 #####
    ###################
    gs=Gophersat(gophersat_exec, defi_voc(size))
    ajout_clauses(gs,size)
    carte=cartographier(wwr,gs,size)
    

    status, msg = wwr.end_map()
    print(msg)
    print(wwr.get_gold_infos())

    ###################
    ##### PHASE 2 #####
    ###################
    
    phase, pos = wwr.get_status()
    print("status:", phase, pos)
    i, j = wwr.get_position()
    print(f"Vous êtes en ({i},{j})")
    
    initial=(i,j)
    Final=etat_finaux(carte)
    chemin=[]
    
    chemin=recherche_or(chemin,carte,initial,Final)
    print("CHEMIN VERS L'OR")
    print(chemin)
    
    course_vers_or(wwr,chemin)
    
    
    i, j = wwr.get_position()
    print(f"Vous êtes en ({i},{j})" " / Retour en (0,0)")
    
    depart_retour=(i,j)
    Final=[]
    Final.append((0,0))
    chemin=[]
    chemin_retour=recherche_or(chemin,carte,depart_retour,Final)
    print(chemin_retour)
    
    course_vers_or(wwr,chemin_retour)
    
    # Vous devez être en (0,0) ou mort pour avoir le droit d'appeler maze_completed
    res = wwr.maze_completed()
    print("Es-tu de retour en (0,0) ?")
    print(res)
    
    phase, pos = wwr.get_status()
    print("status:", phase, pos)

    print("FIN DE LA RECHERCHE D'OR")


def treat_map_gen(wwr: WumpusWorldRemote):
    size = wwr.current_size

    for i in range(size):
        for j in range(size):
            if random.choice([True, False]):
                res = wwr.cautious_probe(i, j)
            else:
                res = wwr.probe(i, j)
            print(res)

    status, msg = wwr.end_map()
    print(msg)
    pprint(wwr.get_gold_infos())

    print(wwr.get_status())

    res = wwr.maze_completed()

    print(res)


def test_remote4():
    # Connexion au server
    server = "http://lagrue.ninja"
    groupe_id = "PRJ52"  
    names = "Camille Souvigny et Fanny Gondouin"  

    try:
        wwr = WumpusWorldRemote(server, groupe_id, names)
        # version pour avoir les logs de requêtes
        # wwr = WumpusWorldRemote(server, groupe_id, names, log=True)
    except HTTPError as e:
        print(e)
        print("Try to close the server (Ctrl-C in terminal) and restart it")
        return

    # Création du labyrinthe
    status, msg, size = wwr.next_maze() 
    maze = 1

    while status == "[OK]":
        print(wwr.get_status())
        pprint(wwr.get_gold_infos())

        """if maze == 1:
            treat_map_1(wwr)
        else:
            treat_map_gen(wwr)"""

        treat_map_1(wwr)        

        status, msg, size = wwr.next_maze()
        maze += 1
        print(status, msg, size)


if __name__ == "__main__":
    test_remote4()

