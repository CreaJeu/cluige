**fini récemment**

(chantier : minimal usable Node/Script/main loop)
(chantier sprite text (no anim))
(Deque)
(inputs without just_released)
(tous les params char* : accepter ceux alloués sur pile, avec copie systématique)
(suite textraster avec code Ulrich)
(tous les identifiants en snake_case, types restent PascalCase)
(sprite svg (no anim) : minimal file parsing)
(SortedDictionary)


**tâches actuelles**

- minimal godot scene import

**Futures tâches**

- rename SpriteText to Label ?
- sprite svg (no anim) : file parsing fix discontinued paths
- sprite svg : choose rastering algo from thickness
- sprite svg : optim big segments out of screen
- node2D (et autre?) : meilleur test ~instanceof / _className static par classe
- fatal error => end loop + printf
- queue_free, remove child
- get node by name/path
- project files to test every methods
- get_path



**FUTURS CHANTIERS**

- signals
- collision rectangle
- camera
- more godot scene import
- input : just_released? => MVC pour inputs (input server, platform-specific)
- memory pools (at least for pairs in dico)
- full transform2D?
- audio?
- MVC pour affichage : modèle/back-buffer côté cluige, vue à part ("server" en vocabulaire Godot docs.godotengine.org/en/stable/tutorials/performance/using_servers.html )
- sprite text : transparent characters, utiliser split et Deque
- idle cluige
- serialisation
- process/main loop : BFS instead of DFS, simplify Node2D::postProcessNode()?
- anim sprites?
- color?
- localisation?
- memory pool for pairs, for other ?
- deque : option deep copy ?

