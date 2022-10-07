import os 
os.chdir('C:/Users/camom/Documents/CamilleSouvigny/Wumpus_GameSolver')

import path_to_gold_djikstra
import solver_rules_strategy


from wumpus_client import WumpusWorldRemote

#from wumpus import WumpusWorld
#ww = WumpusWorld()

from pprint import pprint
from requests.exceptions import HTTPError
import random

os.chdir('C:/Users/camom/Documents/CamilleSouvigny/Wumpus_GameSolver/lib')
from gopherpysat import Gophersat
gophersat_exec="./gophersat.exe"




__author__ = "Sylvain Lagrue"
__copyright__ = "Copyright 2020, UTC"
__license__ = "LGPL-3.0"
__version__ = "0.2"
__maintainer__ = "Sylvain Lagrue"
__email__ = "sylvain.lagrue@utc.fr"
__status__ = "dev"



def treat_map_1(wwr: WumpusWorldRemote):
    size = wwr.current_size

    ### Solving Wumpus
    
    gs=Gophersat(gophersat_exec, defi_voc(size))
    ajout_clauses(gs,size)
    carte=cartographier(wwr,gs,size)
    

    status, msg = wwr.end_map()
    print(msg)
    print(wwr.get_gold_infos())

    ### Path to gold
    
    phase, pos = wwr.get_status()
    print("status:", phase, pos)
    i, j = wwr.get_position()
    print(f"you are in  ({i},{j})")
    
    initial=(i,j)
    Final=etat_finaux(carte)
    chemin=[]
    
    chemin=recherche_or(chemin,carte,initial,Final)
    print("Path to gold ! ")
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
    
    # You have to be in (0,0) or dead to call maze_completed
    res = wwr.maze_completed()
    print("Es-tu de retour en (0,0) ?")
    print(res)
    
    phase, pos = wwr.get_status()
    print("status:", phase, pos)

    print("End of gold search")


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


def remote_solver():
    # Connexion to server
    server = "http://lagrue.ninja"
    groupe_id = "PRJ52"  
    names = "Camille Souvigny"  

    try:
        wwr = WumpusWorldRemote(server, groupe_id, names)
    except HTTPError as e:
        print(e)
        print("Try to close the server (Ctrl-C in terminal) and restart it")
        return

    # Maze Creation
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
    remote_solver()

