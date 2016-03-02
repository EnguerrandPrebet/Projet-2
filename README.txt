PROJET 2 : Rendu 1
Enguerrand PREBET et Clément CHOUTEAU
18/12/16


Le programme gère l'entrée standard .cnf ainsi que la version conviviale .for, des options ont été rajoutées (lancer avec --help (ou -h) pour plus d'informations), un pré-traitement de l'entrée est effectué (clauses tautologiques, unitaires, et les variables ayant une seule polarité)
L'algorithme effectue un backtracking paresseux (sur la satifiabilité des clauses)
 
 
/Structure du code
On a pris pour convention d'utiliser l'anglais pour les noms techniques ainsi que les sorties de debug, mais de garder les commentaires en français, pour pouvoir taper des commentaires pertinents plus rapidement. 
De même, les classes commencent par une majuscule pour les différencier des variables, dont les mots sont séparés par un '_'.

Lorsque de nombreuses recherches sont nécessaires, la structure de set a été privilégiée, pour vérifier l'appartenance d'une variable à une clause par exemple, ainsi que pour faciliter la gestion des redondances. Dans le reste des cas, les vectors ont été utilisés par simplicité d'utilisation et facilité d'accès.

/Méthodologie
Pour la modularité, on a découpé
	- entrées/sorties
	- algorithmes
	- structures de données
	- outils (tseitin, fonctions annexes du solveur)

Pour l'algorithme du solveur on a d'abord codé l'algorithme en utilisant une API que l'on définit pendant que l'on code l'algorithme, puis on a obtenu l'API définitive.

//Améliorations
Dans l'avenir, on pense effectuer une évaluation moins paresseuse de la satisfiabilité des clauses, mais sans tomber dans l'excès de vérifications, au moins des watched litterals par exemple, cela nous obligera peut être à revoir les structures de données utilisées.


/Tests
Il réussit les tests sur le pré-traitement de la polarité unique, les cas triviaux (formule vide, clause vide) ainsi que des tests plus conséquents trouvés sur internet. Le test dubois20.cnf présente déjà des signes d'épuisement du solveur.
L'option de débug --debug permet de suivre facilement ce que fait l'algorithme.
Sur les 12 tests effectués, seul combinatorial.cnf a bloqué notre programme (limite de temps), la taille de l'entrée étant quelque peu exagérée au vue de la rudimentarité du programme.

//Améliorations
Dans l'avenir, on pense chercher des tests de performance plus conséquents, soit aléatoires, soit applicatifs. Une vérification automatique du programme avec les tests est envisagée (non régression).


/Répartition
Clément s'est chargé d'implémenter l'algo DPLL, et de l'architecture globale du projet.
Enguerrand s'est quant à lui occupé de la transformation de tseitin, la gestion des entrées, des classes, de ce README.
Le debug et le reste s'est effectué principalement à deux.
