# Documentation sur les designs patterns utiles

* decorator
* factory
* abstract factory
* builder
* bridge
* composite
* iterator
* template method
* adapter
* visitor
* strategy
* facade
* memento

## Decorator

### A quoi sert-il ?

Il permet d'ajouter des fonctionnalités à un objet de manière dynamique. Il est notamment utilisé comme alternative aux sous-classes car il a l'avantage d'être dynamique (les classes sont définies avant la compilation alors que Decorator permet d'ajouter des fonctionnalités pendant l'exécution).

### Bref exemple

On crée une classe Circle qui gère des cercles, sous-classe de Shape. Si on veut aussi pouvoir définir des formes colorées, on peut créer une classe fille ColoredCircle.
Si par la suite on veut gérer des carrés et des triangles, il faudra alors recréer des classes filles ColoredSquare etc...
Une classe ShapeDecorator peut-être crée avec une multitude de décorateur. Dans notre cas, la sous-classe ColoredShape pourra s'appliquer à toutes les formes.

    Circle circle; // On crée un objet cercle
    /*plusieurs instrucions*/
    ColoredShape colored_shape("red", &circle); //On le "décore" par la suite de manière dynamique

### sources/liens intéressants :

https://en.wikipedia.org/wiki/Decorator_pattern  
https://www.tutorialspoint.com/design_pattern/decorator_pattern.htm

## Factory

Le design pattern Factory permet de créer une classe _Factory_ qui est capable d'instancier plusieurs types différents d'objets héritant d'une même classe mère et d'en retouner une référence (ou un pointeur). L'idée est d'utiliser une méthode qui permet d'instancier l'objet.
- D'un point de vue développeur, celà cache les détails de la construction d'un objet pour l'utilisateur de la librairie, augmentant encore le principe d'encapsulation.
- Du point de vue de l'utilisatueur du programme, celà permet d'instancier un type d'objet différent selon un input donné.
Dans notre cas, ce DP peut être utile pour créer différents comptes (compte virtuel, compte normal) selon le choix de l'utilisateur.

### Bref exemple

On dispose d'une classe abstraite _Jouets_. Les classes _Poupée_, _Figurine_, _Velo_ en hérite. On créer une _Factory_ qui permet d'instancier une poupée, une figurine ou un vélo en fonction de l'input de l'utilisateur.

### sources/liens intéressants :

Très bien expliqué : https://www.geeksforgeeks.org/design-patterns-set-2-factory-method/  
UML : http://design-patterns.fr/fabrique  
Exemple des jouets (UML + code): https://youtu.be/XyNWEWUSa5E?t=284

## Abstract Factory

Il s'agit ici simplement de grouper plusieurs _Factory_ en les faisant hériter d'une _AbstractFactory_. La différence c'est que la Fabrique abstraite permet non pas d'instancier différents objets d'une même classe mère mais plusieurs objets de plusieurs classes mères. Celà permet de créer des interfaces pour créer des objets sans spécifier les classes concrètes qui sont derrières.

### Bref exemple

L'utilisateur veut créer soit une voiture normale, soit une voiture de luxe. Alors il utilise une NormalFactory ou une LuxeFactory, qui hérite d'une classe abstraite CarFactory.

### sources/liens intéressants :

UML : https://www.codingame.com/playgrounds/36103/design-pattern-factory-abstract-factory/design-pattern-abstract-factory (code java mais compréhensible)  
https://fr.wikipedia.org/wiki/Fabrique_abstraite  
Exemple voitures (UML + code) : https://www.youtube.com/watch?v=blDsmrsyOME

## Builder

Un patron de conception créationnel qui permet de créer des objets complexes à partir d'autres objets. Concrètement, c'est comme si on assemblait plusieurs objets pour les "monter" et n'en faire qu'un.

### Advantages of Builder Design Pattern
* The parameters to the constructor are reduced and are provided in highly readable method calls.
* Builder design pattern also helps in minimizing the number of parameters in constructor and thus there is no need to pass in null for optional parameters to the constructor.
* Immutable objects can be build without much complex logic in object building process.
### Disadvantages of Builder Design Pattern
* The number of lines of code increase at least to double in builder pattern, but the effort pays off in terms of design flexibility and much more readable code.

### Bref exemple
Class Account { string id; string nom; string prenom; string email; string rue; string noPostal; string ville; }
/*constructeur*/ Account(const string id, const string n, const string p, const string e… )
Implémentation de l’exemple : https://www.youtube.com/watch?v=xy6FUwftz1Q

### sources/liens intéressants :
https://www.geeksforgeeks.org/builder-design-pattern/
https://www.youtube.com/watch?v=mFCk31FoUg4

## Bridge

Le pont est un patron de conception de la famille structuration, qui permet de découpler l'interface d'une classe et son implémentation.
La partie concrète (implémentation réelle) peut alors varier, indépendamment de celle abstraite (définition virtuelle), tant qu'elle respecte le contrat de réécriture associé qui les lie (obligation de se conformer aux signatures des fonctions/méthodes, et de leur fournir un corps physique d'implémentation).

### When we need bridge design pattern
The Bridge pattern is an application of the old advice, “prefer composition over inheritance”.
It becomes handy when you must subclass different times in ways that are orthogonal with one another.
The pattern reduces the number of classes to be implemented.

### sources/liens intéressants :
https://www.geeksforgeeks.org/bridge-design-pattern/
Really long but the best video on yt: https://www.youtube.com/watch?v=F1YQ7YRjttI  
https://blog.usejournal.com/design-patterns-a-quick-guide-to-bridge-pattern-9ebf6a77baed

## Composite

### A quoi sert-il ?

Il permet de hierarchiser des objets sous forme d'arborescence et de faire en sorte que le client puisse utiliser l'objet de manière uniforme, quelque soit sa place dans la hiérarchie. En gros il permet de créer une classe qui contient une liste d'objet du même type.
Dans notre cas il peut être utile pour gérer l'arborescence de nos comptes. Un compte virtuel est un compte qui contient une liste de compte, c'est bien adapté au design pattern Composite.

### Bref exemple

Pour représenter une arborescence des répertoires et des fichiers, le design pattern Composite est très utile :
on crée une classe Component et deux sous-classes File et Directory. Les objets File sont les extrémités de l'arborescence.
Grâce à l'implémentation de Composite, on peut définir une méthode printDirectory() adaptée aux répertoires et aux fichiers qui affiche l'arborescence de l'objet en question.
L'implémentation (voir lien) de la méthode printDirectory() utilise alors le principe de récursion.

### sources/liens intéressants :

https://fr.wikipedia.org/wiki/Composite_(patron_de_conception)  
https://www.tutorialspoint.com/composite-design-pattern-in-cplusplus

## Template Method

### A quoi sert il ?

Un patron de méthode définit le squelette d'un algorithme à l'aide d'opérations abstraites dont le comportement concret se trouvera dans les sous-classes, qui implémenteront ces opérations.

C'est une methode général qui permet de générer un "template" par défaut. 
Ce template possède une certaine structure ("mise en page") que l'on a défini dans la Classe methode et certains espaces sont non remplis et défini par ce que l'on rentre dans les sous classes. 
Cela permet d'avoir une mise en page uniforme et de ne pas avoir à réécrire à chaque fois l'organisation et la présentation et donc de ne pas avoir à dupliquer le code pour des choses qui ne changent pas.

### Attention :
à utiliser seulement si on est sûr que ça ne changera pas, que l'implémentation sera bien la même pour tout car on crée les différentes opérations dans un ordre précis et immuable.
Alternative StrategyPattern : on défini les éléments un à un et on peut choisir quand les utiliser et dans quel ordre.

### sources/liens intéressants :
https://www.youtube.com/watch?v=7ocpwK9uesw

## Adapter

c'est un design pattern de type structure. 
Le pattern Adaptateur permet de transformer une interface d'une classe en une autre conforme à celle attendue par le client. 
L’adaptateur fait fonctionner ensemble des classes qui n'auraient pas pu fonctionner sans lui, à cause d'une incompatibilité d'interfaces.
 
Il peut, par exemple, servir pour utilser la même methode entre classe similaire mais non identiques. L'intention étant de ne pas changer le comportement de la classe ou de ne pas ajouter des comportement à la classe.
c'est un peu comme l'idée des adaptateur de prises UE/USA ou UE/UK. Ou encore des chargeurs de téléphones avec plusieurs embouts (usbc, universel,etc..) pour faire en sorte que tout téléphone puisse être recharger avec ce chargeur bien que le port ne soit pas le meme pour tous.

### sources/liens intéressants :
https://badger.developpez.com/tutoriels/dotnet/patterns/adaptateur/
https://www.youtube.com/watch?v=2PKQtcJjYvc

## Visitor

### A quoi sert-il ?
Le pattern visitor permet d'ajouter des méthodes à des classes de types différents, sans pour autant entrainer une grande modification de ces classes. Le pattern Visiteur permet une séparation précise entre données et traitements. Il est le compagnon idéal du pattern Composite. Dans ce DP, on a deux types de hiérarchie : la hiérarchie des objets support de données et la hiérarchie des Visiteurs (contenant les traitements sur les données)

### Avantages :
-le code est plus clair (des fonctionnalités différentes se trouvent dans des Visiteurs différents)
-des équipes différentes peuvent travailler sur des fonctionnalités différentes sans gêner les autres équipes
-on n'est pas obligé de tout recompiler à chaque ajout d'une fonctionnalité (seul le code du Visiteur est recompilé)

### sources/liens intéressants :
Explication/ conception très claire : https://fr.wikipedia.org/wiki/Visiteur_(patron_de_conception)
Un exemple : https://www.youtube.com/watch?v=pL4mOUDi54o

## Strategy

### A quoi sert-il ?
Le pattern stratégie est utilisé lorsqu'un objet doit effectuer différentes actions, on fonction de la situation dans laquelle il se trouve (en fonction d'un état particulier, de la valeur d'une variable...). Ce DP encapsule les différentes version possible de l'algorithme dans une classe (en tant qu'objet). Il suffit ensuite de moduler les classes appelées par le client pour changer l'algorithme en fonction de la situation.

### Un Bref Exemple
Lorsqu'on achète un produit sur internet, on l'ajoute au panier.
Au moment de payer, le site demande d'effectuer l'action abstraite de "payer". Il est possible de payer de différentes manières : directement par carte, par Paypal... La méthode "payer" peut être implémentée de différentes manières, d'où l'intérêt du Pattern Strategy.

### sources/liens intéressants :
https://fr.wikipedia.org/wiki/Strat%C3%A9gie_(patron_de_conception)
Page 134 du poly

## Facade

On souhaite cacher la conception et l'interface à l'uttilisateur. PK ? : Car le système d'interdépendance entre les classes rend l'uttilisation complexe pour l'uttilisateur. L'accès à l'uttilisateur est simplifié et est restreint en conséquence

### sources/liens intéressants :
https://medium.com/@andreaspoyias/design-patterns-a-quick-guide-to-facade-pattern-16e3d2f1bfb6


## Memento


restaurer un état précédent d'un objet (retour arrière) sans violer le principe d'encapsulation. On crée un objet gardien, qui agit sur l'objet créateur. 
Le gardien demande au créateur, lors de chaque action, un objet mémento qui sauvegarde l'état de l'objet créateur avant la modification.
Le créateur envoie les informtation par méthode 
les classes de l'exemple :
class Caretaker
class Memento
class ConcreteMemento : public Memento 
class Originator 

### sources/liens intéressants :
https://refactoring.guru/design-patterns/memento/cpp/example

