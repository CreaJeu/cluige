
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
(more godot import)
	per-scene-dico id>uid, global-dico uid>PackedScene
	test [pre]delete tscnparser/packedscene => no, maybe one day
	test with .tscn spread in sub-sub-dirs => ok godot always use absolute path from /res for ext_resource descriptors
	test import tscn with a label at arbitrary position
	update test_node_against_ps to include instance->instance->... => tant pis, complexité de test disproportionnée
(linux)
(more godot scene import : script attachment, script factory + system to force to register them)
(project.godot parsing : first interesting params, inputs)
(bugs : see below)


**tâches actuelles**

- re-test windows


**Futures tâches**

- update Readme
- replace all space-indent with tab-indent + check EOL? + (Node.c +?) ths_Node => this_Node
- checked_malloc() : separate sizeof(...) as another arg (to help never forget)
- macro CLUIGE_BREAKPOINT() => utils_breakpoint_trick()
- hunt every
	for(...; i < iCluige.iDeque.size( ...
	printf()
	todo
- fatal error => end loop + printf
- sprite svg (no anim) : file parsing fix discontinued paths
- sprite svg : choose rastering algo from thickness
- sprite svg : optim big segments out of screen (cf. futurs chantiers)
- more-like-godot-todos
    - wanted_frame_seconds rename as wanted_seconds_per_frame or like godot
    - refactor utils to iCluige.iGlobals, match contents whith godot Globals, move check_malloc() in there
    - godotify move_local(vec2) : move_local_x/y(dx/dy) + translate(Vec2) + global_translate(Vec2)
    - rename SpriteText to Label ?
    - rename Path2D to SvgPath2D (confusion with godot Node2D subclass Path2D)
    - inherited_visibility (car parent invisible => branche invisible), màj dans bake()
    - process_mode (disabled, pausable, etc. : enum in godot instead of just a 'active' bool field)
- Deque::sort()
- custom app icon
- reallocate in Deque::grow()
- node2D (et autre?) : meilleur test instanceof / class_name static par classe
- add types in Variant (in union, VariantType, from_val(), compare()...) : char*, Pair*



**FUTURS CHANTIERS**

- signals
- collision rectangle + layers
- all valgrind ok cluige and test_cluige
- groups (Node functions + tscn import + instantiate)
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
- input :
	parse other than 'unicode' from project.godot + mouse + joystick
	generate godot default even if absent from projetc.godot ?
	just_released? => MVC for inputs (input server, platform-specific)
- autoload
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
things not well erased when running test_cluige quickly by keeping keys pressed to keep moving camera or sprite when also pressing 'N' for next phase


**fixed bugs**
(spriteText not correctly erased when text changes)
(erase sometimes skipped when keyboard autorepeat, even if code in erase() is commented to erase everything each frame)
(bug camera-test no more showing instructions)
