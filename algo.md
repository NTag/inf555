#Preprocessing
On itère le paragraphe suivant sur les images.

##Traitement d'une image

###Génération de directions de projections
Nombre de directions : `d = 100`.   
Sélection de `d` points sur un maillage fin d'une sphère.   
*k-means* (algorithme de Lloyd avec `k=d`) pour déterminer `d` centroïdes correspondant aux directions. On retient les directions correspondantes de l'image (parmi l'ensemble des points-clés liés à cette image).

###Rendu du tracé de lignes
Projection des cartes de profondeur des modèles 3D vus depuis les `d` directions calculées auparavant.  
Application du filtre de Canny pour extraire les contours.  

###Calcul du vecteur caractéristique local
On tire au hasard 10^6/(Nd) points-clés sur l'image.   
Pour chaque point-clé, on calcule le vecteur caractéristique local (un vecteur de `n*n*d` composantes), et on le stocke dans une structure de données adaptée (`vector` ?).  


##Représentation

###Classification des vecteurs caractéristiques locaux
On effectue une classification non supervisée (*k-means*) sur les VCL précédemment calculées, avec `k=1000` (ou mieux `k=2500`).

###Construction des histogrammes visuels
Pour chaque modèle de la bibliothèque, et chaque vue de ce modèle, on calcule un histogramme visuel.  
Pour chaque vue, on crée un vecteur **h**, histogramme. On calcule le VCL et on note `j` la classe à laquelle il appartient. On incrémente alors la composante `j` du vecteur **h** d'une unité.  
On stocke l'histogramme ainsi calculé dans une structure de données inversée (pour chercher sur les histogrammes et trouver les vues correspondantes).



#Querying
Nothing yet...



#Idées pour l'implémentation

* Une classe `Histogram` pour stocker les histogrammes, mesurer la similarité, etc.  
* Une structure de table de hashage : `Histogram => Model3D` où le modèle 3D est représenté simplement par son nom de fichier OFF.
* *À compléter...* :p
