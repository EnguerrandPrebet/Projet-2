PROJET 2 : Rendu 2
Enguerrand PREBET et Clément CHOUTEAU
22/03/16


Le programme gère l'entrée standard .cnf ainsi que la version conviviale .for avec maintenant déductions, la possibilité d'utiliser des heuristiques (RAND,MOMS), ainsi que les littéraux surveillés.
 
 
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

//Améliorations
Dans l'avenir, on pense chercher des tests de performance plus conséquents, soit aléatoires, soit applicatifs. Une vérification automatique du programme avec les tests est envisagée (non régression).


/Répartition
Nous avons essayé d'intervertir les rôles pour ce 2e rendu.
Ainsi Enguerrand s'est chargé de la transformation de l'architecture du projet pour le rendre plus modulable, de l'implémentation des déductions et des littéraux surveillés, et toujours de ce README.
Clément s'est quant à lui occupé d'implémenter les heuristiques, d'adapter tseitin aux nouvelles structures de données, ainsi qu'à la lisibilité du code.
Le debug et le reste (les script de tests, par exemple) s'est effectué principalement à deux.
