**fini récemment**

(chantier : minimal usable Node/Script/main loop)
(chantier sprite text (no anim))
(Deque)
(inputs without just_released)


**tâches actuelles**

- tous les membres en snake_case? (types peuvent rester PascalCase)
- tous les params char* : accepter ceux alloués sur pile, avec copie systématique

**Futures tâches**

- fatal error => end loop + printf
- node2D (et autre?) : meilleur test ~instanceof / _className static par classe
- queue_free, remove child
- get node by name/path



**FUTURS CHANTIERS**

- suite textraster avec code Ulrich
- sprite svg (no anim)
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
- localisation?

