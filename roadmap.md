
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
(queue_free, remove child, get node by name/path, get_path)
(`~auto` test project)
(camera)
(minimal godot scene import : tscn format parsing, packed scene generation)
(more godot scene import : instanciate() ok for some node classes in their current state)
(tscn import : move test functions from gamejam to test project)
(process/main loop : still DFS, better game loop order)
(enter_tree(), ready(), exit_tree())
(more godot scene import : camera (not clock, no such node in godot, so, never found in .tscn))


**tâches actuelles**

- bug erase SpriteText when text changes
- more godot scene import : per-scene-dico id>uid, global-dico uid>PackedScene, project.godot parsing, script attachment, global dico script-name>script-constructor + system to auto register scripts into it, inputs, other deserialize()


**Futures tâches**

- replace all space-indent with tab-indent
- wanted_frame_seconds rename as wanted_seconds_per_frame or like godot
- sprite svg (no anim) : file parsing fix discontinued paths
- sprite svg : choose rastering algo from thickness
- sprite svg : optim big segments out of screen (cf. futurs chantiers)
- node2D (et autre?) : meilleur test ~instanceof / _className static par classe
- fatal error => end loop + printf
- Deque::sort()
- reallocate in Deque::grow()
- more-like-godot-todos
    - refactor utils to iCluige.iGlobals, match contents whith godot Globals, move check_malloc() in there
    - godotify move_local(vec2) : move_local_x/y(dx/dy) + translate(Vec2) + global_translate(Vec2)
    - rename SpriteText to Label ?
    - rename Path2D to SvgPath2D (confusion with godot Node2D subclass Path2D)
    - inherited_visibility (car parent invisible => branche invisible), màj dans bake()
    - process_mode (disabled, pausable, etc. : enum in godot instead of just a 'active' bool field)



**FUTURS CHANTIERS**

- signals
- collision rectangle
- Node2D scale, with consequences on SpriteText/Label and SpriteSVG, no more scale in SpriteSVG
- optim draw/erase sprites (faire Node2D::scale d'abord)
    Node2D::bake() : benefit from DFS, tmp_global_pos of parent already up to date (nearest Node2D ancestor)
    spriteText
        lrintf : pourquoi pas le résultat total au lieu de orig.x?
        au lieu re-transform chaque char
            calc en 1 seule fois deux vecteurs unitaires u v
                sreen_text_orig--->curr_char = col*v + lin*u
    clip drawing inside visible window (cf. futures tâches)
- utf8 in SpriteText
- input : just_released? => MVC pour inputs (input server, platform-specific)
- memory pools for pairs in dico
- full transform2D?
- audio?
- MVC pour affichage : modèle/back-buffer côté cluige, vue à part ("server" en vocabulaire Godot docs.godotengine.org/en/stable/tutorials/performance/using_servers.html )
- sprite text : transparent characters, utiliser split et Deque
- idle cluige
- serialisation
- anim sprites?
- color?
- localisation?
- memory pool for other than pairs in dico ?
- deque : option deep copy ?
- other Node2D subclasses? see https://docs.godotengine.org/en/stable/classes/class_node2d.html e.g. Line2D, Marker2D, Polygon2D
- add draw() function in scripts like in godot?



**bugs**

- erase sometimes skipped when keyboard autorepeat, even if code in erase() is commented to erase everything each frame
- spriteText not correctly erased when text changes
