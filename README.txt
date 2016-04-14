PROJET 2 : Rendu 3
Enguerrand PREBET et Clément CHOUTEAU
14/04/16


CE QUI SUIT N'EST PAS PROPRE AU RENDU 3

Le programme gère l'entrée standard .cnf ainsi que la version conviviale .for avec maintenant déductions, la possibilité d'utiliser des heuristiques (RAND,MOMS,DLIS), ainsi que les littéraux surveillés.
 
//Structure du code
On a gardé pour convention d'utiliser l'anglais pour les noms techniques ainsi que les sorties de debug, mais de garder les commentaires en français, pour pouvoir taper des commentaires pertinents plus rapidement. 
De même, les classes commencent par une majuscule pour les différencier des variables, dont les mots sont séparés par un '_'.

//Implémentation
Les clauses validées sont stoctées dans un tableau de listes (pour un "réveil" constant) indexé par les variables responsables de cette validation. Les clauses vivantes, ainsi que les variables vivantes dans celles-ci sont stockées dans une liste pour pouvoir ajouter et enlevé aisément.
L'affection quant à elle reste dans un tableau.
Pour les littéraux surveillés, les clauses sont fixés en tant que tableau parcouru par les 2 surveillés.


//Méthodologie
Pour la modularité, on a découpé
	- entrées/sorties
	- algorithmes
	- structures de données (dès que l'algorithme doit y accéder)
	- outils (tseitin)

Des outils d'optimisation ont été utilisés pour regarder si une partie du code n'était pas sursollicité, ce qui a permis des corrections simples, mais efficace.

//Améliorations
Actuellement, ce sont les modifications apportés par les paris et déductions aux clauses qui monopolisent une grande partie du temps d'execution. Ainsi, un principal axe de recherche d'amélioration peut se situer dans des structures de données plus complexes.

/Tests
Il réussit les tests sur le pré-traitement de la polarité unique, les cas triviaux (formule vide, clause vide, un backtrack) ainsi que des tests plus conséquents trouvés sur le lien fourni via le site.
L'option de débug --debug permet de suivre facilement ce que fait l'algorithme.
L'algo est bien plus performant que pour le rendu 1 puisque combinatorial.cnf qui ne terminait pas après plus de 10 min, est maintenant une question de secondes.
Plus d'informations dans tests/resol/README


CE QUI SUIT EST PROPRE AU RENDU 3

/Répartition
Enguerrand s'est chargé d'implémenter l'algorithme de recherche de l'UIP ainsi que la déduction de clause.
Clément s'est chargé d'afficher le graphe avec graphviz, de créer global.hpp et d'effectuer l'harmonisation des messages de debug ainsi que d'écrire ce readme.

//Travail effectué
L'affichage du graphe de Clause Learning fonctionne.
Le code couleurs est le suivant :
	Fond bleu : noeud à t courant
	Fond blanc : noeud à t antérieur
	Fond rouge : conflict
	Fond jaune : UIP
	Fond vert : variables ajoutées par clause learning
	Bordure rouge : noeud lié au conflict
Petit ajout de dernière minute : les arrêtes entre deux sommets liés au conflict sont rouges, cela facilite considérablement la lecture du graphe.

La déduction de clauses fonctionne également mais change peu les performances, environ +-20%.
Le calcul du graphe est effectué au dernier moment, donc aucun impact sur les performances si l'option -cl n'est pas activée.

//Améliorations
Le Makefile a été amélioré et effectue maintenant une compilation avec fichiers intermédiaires pour limiter le temps de compilation (quasiment instantané contre 3s-5s avant).

Un fichier global.hpp est maintenant chargé de gérer les options (variable globale) du programme.
	Il permet également de créer un système de messages d'erreurs / warning / error, facilement, exemple :
		Global::DEBUG(1) << "Reading complete !" << endl << "Launching DPLL Solver..." << endl;
	qui n'affiche le message que si le niveau de debug donné en ligne de commande vaut au moins l'entier indiqué
		Global::WARNING() << "Expected " << c << " clauses (" << actual_c << " found)" << endl;
	qui ajoute un entête "Warning: "

//Travail futur
Dans l'avenir, on pense chercher des tests de performance plus conséquents, soit aléatoires, soit applicatifs.
Automatiser l'affichage de divers graphes de performances et notamment un affichage 2D (couleurs) des performances du programme sur des formules aléatoires de taille (n, m) où n et m varient
afin de mieux voir l'influence des options sur des formules ayant beaucoup de (clauses | variables), notamment pour pouvoir faire le bilan sur notre implémentation des watched litterals.

Analyser de façon précise (gprofs, valgrind) le temps d'exécution des différentes fonctions du programme.
Utiliser des outils d'analyse de code pour trouver les fonctions / lignes trop longues pour rendre le code plus clair.

Le point suivant est sûrement le plus important :
	Rendre les API des classes Formula, Clause, ... plus claires et plus robustes.
	On a souvent des classes avec un rôle pas bien défini / des variables public / trop de setters, getters
