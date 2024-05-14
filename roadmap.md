**fini récemment**

(chantier : minimal usable Node/Script/main loop)
(chantier sprite text (no anim))
(Deque)
(inputs without just_released)
(tous les params char* : accepter ceux alloués sur pile, avec copie systématique)
(suite textraster avec code Ulrich)
(tous les identifiants en snake_case, types restent PascalCase)

**tâches actuelles**

- sprite svg (no anim) : true file parsing
- node2D (et autre?) : meilleur test ~instanceof / _className static par classe

**Futures tâches**

- fatal error => end loop + printf
- queue_free, remove child
- get node by name/path
- project files to test every methods
- get_path



**FUTURS CHANTIERS**

- MVC pour affichage : modèle/back-buffer côté cluige, vue à part ("server" en vocabulaire Godot docs.godotengine.org/en/stable/tutorials/performance/using_servers.html )
- sprite text : transparent characters, utiliser split et Deque
- camera
- next collection : Map
- signals
- collision rectangle
- idle cluige
- serialisation
- godot scene import
- process/main loop : BFS instead of DFS, simplify Node2D::postProcessNode()?
- input : just_released? => MVC pour inputs (input server, platform-specific)
- full transform2D?
- anim sprites?
- audio?
- color?
- localisation?

