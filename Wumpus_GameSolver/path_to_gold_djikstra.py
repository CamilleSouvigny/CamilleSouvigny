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
            
