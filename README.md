# cluige
Cluige, for **Command Line Interface Game Engine,** is an **ASCII/unicode/console/terminal minimalist game engine.** It provides only core features like scene tree, game clock, sprites, or keyboard input. More advanced features are meant to be kept separate as third-party plugins. Please contact us to share your plugins; we'll maintain a list of them somewhere. This project is just beginning, we hope it will grow with time.

 
### Low-tech

Why text/ASCII/unicode/console/terminal/CLUI? Because it **runs on much, much less powerful devices!** It could, one day, even run on e-readers with passive display and almost no CPU. With current environmental and resource-consumption challenges, it should be a primary concern for every today's projects. An ASCII display is exaggerated, but this exaggeration is a choice to demonstrate that real needs (for example, the deep cerebral things that make us like games) can be satisfied by virtuous low-tech. Not a regression, **low-tech is sustainable progress.**

At first we plan to remain on 2D games. Maybe we'll work later on very basic 3D features, but not now. Non low-tech features, like heavy physics or lighting, will not be added nor encouraged.


### Mods and plugins

We want cluige games to be easily and highly modifiable by comunity mods. When implemented (see 'roadmap.md'), the mod system will allow any nodes to be added, removed or modified, including their attached scripts, as well as any cluige parameter. Cluige has the MIT open-source license, allowing any game, from open-source to closed-source commercial. **Even closed-source games will be modable**: only their scripts will be closed-source. The cluige team's position is to encourage the modding comunity to reverse-engineer them. Authors of games must accept that philosophy.

Plugins are encouraged. They will be the official way to add not-so-minimalist features, like sound. We'll maintain a list of them somewhere, either team cluige plugins or from the community.


### Inspired by Godot

Cluige is designed as close to the Godot game engine as possible. Everything in your game is made of Nodes, sometimes with attached scripts, organized in trees. Of course, cluige has far fewer features, but the same design philosophy is followed. **Many function and type names are the same.**

Cluige provides a **Godot project importer,** so your game designers can make the whole game in the Godot editor and give it directly to cluige. The importer supports:
- `.tscn` files, including user scripts parameters
- `godot.project` files, including a few engine options (like screen size or FPS) and inputs (inputs have limitations in console/terminal, see 'roadmap.md')
- any data that makes sense to cluige (other data is just ignored; for example, node subtypes unknown to cluige are not added to the cluige tree)
- (see 'roadmap.md' to know what is already implemented or planned in cluige)

The pilosophy about **scripts** is that game devs write them **in C** (or C++ or any other language if they want to do the binding with C). We don't plan to provide a GDScript interpreter. But since Godot is there and GDScript so easy, our suggestion is to first write them in GDScript, test and validate them in Godot, and then easily write their C/C++ equivalent for cluige. See more details in the "USE" section below.

In a nutshell: first make your game in Godot, including scripts. Be sure to use only Godot features that have an equivalent in cluige. Be aware of input limitations in console/terminal. Then write your translation of your GDScript into C and register your scripts' instantiate() functions. Voilà, the game is ready for cluige!


### ASCII art and SVG

Cluige was designed from the start to use ASCII art and SVG (vector graphics) as visual assets.

When you first make your game in Godot, use:
- Godot sub-Node **`Label` for ASCII art** (or a bit of Unicode art, but be aware of font limitations in console/terminal)
- Godot sub-Node **`Sprite2D` for SVG vector art**: Godot can already use .svg files as textures. For now, the only SVG feature supported in cluige is paths (unfilled). See, in our other GitHub project "test_cluige", test_cluige/godot_example/godot_only/memo_inkscape.svg example and recommendations (or comments in cluige/Nodes/SpriteSVG.h) to give your SVG the same scale in Godot and in cluige.

For now, only the default terminal background and foreground colors are used by cluige. Colored games are technically possible in console/terminal, but this feature in cluige is planned for a quite distant future.


### Past, present, future

See `roadmap.md` file for past, present, and future dev phases.


### Compatibility

Cluige and our other ASCII/console projects are _meant_ to be compatible with as many systems as possible. The work is still in progress; testing takes time, so unstable versions have no compatibility guaranteed unless specified.

Build and run tested on:
- Windows 10, 11
- Linux (Debian 12)


## BUILD

Only **usual C** here (C11 compliant so far, probably even C99 as long as we don't use threads). Just compile it your favourite way.

Only **one dependency: [the official _PDCurses_](https://github.com/wmcbrine/PDCurses)**

A **Code::Blocks project file is included** (cluige.cbp), for those who like it, providing straightforward compilation as a dynamic lib. Another Code::Blocks project file is included to build PDCurses as a "makefile project" (PDcurses.cbp). This is _the preferred environment of team cluige_. It assumes the following directory structure:
> - any_root_you_like
>   - **cluige**
>     - cluige.c
>     - cluige.cbp
>     - ...
>     - Nodes/
>     - ...
>   - **PDCurses**
>     - curses.h
>     - ...
>     - wincon/
>     - x11/

For information, here is an extract of a Code::Blocks build log on Linux in debug mode (see inside `cluige.cbp` for up-to-date options; see also our "test_cluige" project for info about building your game):
```
(inside ../PDCurses/x11) ./configure -disable-widec -enable-force-utf8 -enable-debug
(inside ../PDCurses/x11) make -f Makefile all
( => ) gcc ...
(inside codeblocks working dir, i.e. the one containing cluige.cbp, cluige.c ...)
gcc -Wredundant-decls -Wfloat-equal -Wmissing-include-dirs -Wall -std=c11 -fPIC -lm -O0 -ggdb3 -DCLUIGE_DEBUG -DCLUIGE_X11 -DXCURSES -I../PDCurses -c cluige.c -o obj/Debug/cluige.o
gcc ... Deque.c ...
...
gcc -shared -L../PDCurses/x11 -L"/usr/lib/x86_64-linux-gnu" obj/Debug/cluige.o obj/Debug/Deque.o ...  -o bin/Debug/libcluige.so -Wl,-rpath,'\\$ORIGIN'  -lXCurses -lm -lXaw -lXmu -lXt -lX11 -lXpm -lSM -lICE -lXext -lc
Output file is bin/Debug/libcluige.so with size 556.34 KB
```

And here is an equivalent extract on Windows mingw-w64:
```
(inside ../PDCurses/wincon/myDebug) mingw32-make.exe -f ..\Makefile DEBUG=Y DLL=Y PDCURSES_SRCDIR=../..
( => ) gcc -c -g -Wall -DPDCDEBUG -I../.. -DPDC_DLL_BUILD ../../pdcurses/addch.c
( => ) gcc -c ...
( => ) windres -i ../../common/pdcurses.rc pdcurses.o
( => ) gcc -Wl,--out-implib,pdcurses.a -shared -o pdcurses.dll addch.o addchstr.o addstr.o attr.o ...*all the .o* ...
(inside codeblocks working dir, i.e. the one containing cluige.cbp, cluige.c ...)
gcc.exe -Wredundant-decls -Wfloat-equal -Wmissing-include-dirs -Wall -std=c11 -fPIC -O0 -ggdb3 -DCLUIGE_DEBUG -DPDC_DLL_BUILD -I..\PDCurses -c cluige.c -o obj\Debug\cluige.o
gcc ... Deque.c ...
...
gcc.exe -shared -Wl,--output-def=bin\Debug\libcluige.def -Wl,--out-implib=bin\Debug\libcluige.a -Wl,--dll  obj\Debug\cluige.o obj\Debug\Deque.o obj\Debug\FileLineReader.o ...*all the .o* ... obj\Debug\Vector2.o  -o bin\Debug\cluige.dll  ..\PDCurses\wincon\myDebug\pdcurses.a
Output file is bin\Debug\cluige.dll with size 1.68 MB
```

Tested with:
- Code::Blocks with mingw-w64 on Windows 10, 11
- Code::Blocks with gcc on Linux (Debian 12)

## USE

Game, plugin, and mod developers: until we write proper docs, you can find info:
- in the Godot docs for general understanding, especially when types or functions have the same name
- in .h comments of cluige "classes"
- in examples in our "test_cluige" project and game templates (see 'roadmap.md')

#### Object-Oriented Programming
Cluige is written in C language. But we arranged this C code to be as close as possible to modern OOP languages. This is done with structs that can contain fields and function pointers. Usually, two structs are used for one "class": one for the instances (data and virtual methods), and another for what modern OOP languages call "static" fields (we also put any non-virtual methods there).

A general struct is used as a namespace for all cluige features. Some typical examples:
```
Vector2 v = iCluige.get_screen_size();
Clock* c = iCluige.iClock.new_Clock(); // does malloc()
```

In C, methods don't exist, you cannot define functions inside structs. But you can put function pointers there. Yet, they won't have an automatic `this` pointer. You must pass it as a parameter. So it looks like a repetition like `v->f(v)` but we cannot have simpler in C. Other typical cluige code examples:
```
my_script->delete_Script(my_script);   // does deep free()

Deque d;                                             //stack allocated
iCluige.iDeque.deque_alloc(&d, VT_FLOAT, capacity);  //kind of constructor
iCluige.iDeque.push_back(&d, 49.3);                  //typical method call (non-virtual)

TscnParser* tp = iCluige.checked_malloc(sizeof(*tp));         //explicit malloc
iCluige.iTscnParser.tscn_parser_alloc(tp, "Main.tscn", 2000); //kind of constructor
tp->parse_scene(tp);                                          //typical method call (virtual)
```

#### Collections
To avoid dependencies on other libs, cluige provides its own collections. Implemented:
- `Deque`: all-purpose "flat" collection. As a double-ended queue, it has all features of the usual array-list, stack, and queue.
- `SortedDictionary`: to use as an equivalent of the usual hashmap. Instead of hashing approach, it uses an internal Deque, automatically sorted, and uses binary search to find elements.

They use internally an array of elements of type `Variant`, itself a `union` of the most useful types (Godot uses the same trick in its C++) including `void*` for unspecified pointers. Examples to get the value of a Variant:
```
// Variant v  ...
float x = v.f;
c = (char*) iCluige.iDeque.at(&d, 42).ptr;  // at() returns a Variant
```

Deque and SortedDictionary provide variadic functions (functions with "..." as parameter) to easily add elements. The internal array automatically grows if needed. Examples:
```
//use of Deque::push_back(Deque* this_Deque, ...)
iCluige.iDeque.push_back(&d, &my_node);
```

At "constructor time" you specify the type of the elements (from an enum in Variant.h):
```
iCluige.iDeque.deque_alloc(&my_things, VT_FLOAT, capacity);
iCluige.iSortedDictionary.sorted_dictionary_alloc(&my_dico, VT_POINTER, VT_INT64, capacity); // let's say: dico<char*, int64_t>
```

Just after "constructor time" you can mark a Deque as "sorted". A default compare() function is automatically assigned but you can also give your own. Then use insert_or_replace_sorted() to insert new elements automatically at their sorted place. As long as the deque is sorted you can use bsearch() on it:
```
iCluige.iDeque.deque_alloc(&my_things, VT_POINTER, capacity);
my_things._sorted = true;
my_things._compare_func = my_compare_func; // somewhere you defined: int my_compare_func(Variant, Variant)
bool replace = false;
Checked_Variant cv = iCluige.iDeque.insert_or_replace_sorted(&my_things, replace, &my_elem);
if(cv.valid) ...
int found_index = iCluige.iDeque.bsearch(&my_things, &wanted_elem);
```

A common case with SortedDictionary is to set a string comparison function for key comparison. Such a function is provided in iDeque:
```
SortedDictionary* d = iCluige.checked_malloc(sizeof(*d));  // let's say: dico<char*, char*>
iCluige.iSortedDictionary.sorted_dictionary_alloc(d, VT_POINTER, VT_POINTER, capacity);
iCluige.iSortedDictionary.set_compare_keys_func(d, iCluige.iDeque.default_compare_string_func);
iCluige.iSortedDictionary.insert(d, bob->name, bob->description);
```

No other collection is planned for now (see 'roadmap.md'). Use Deque even if you only need an array-list equivalent, for example. In cluige philosophy, huge dictionaries are not expected, as they would not be low-tech. So we consider the binary-search complexity of SortedDictionary on par with real-life imperfect hash search.

#### Scripts
User scripts are "sub-classes" of a `Script` "class":
- their "virtual methods" are not "abstract", you can leave their default implementation (even when they are NULL)
- a non-virtual `instantiate(Dictionary<string param_name, string param_value>*)` function must be provided and registered in cluige (via its function pointer).

Cluige doesn't need anything else per script beyond this "subclassing" and "one-function-pointer-registering".

Examples will be provided as cluige game templates (see 'roadmap.md'). Other examples can be found in our other GitHub project "test_cluige". With a couple of user script examples, everything should be clear and straightforward.

#### Coding conventions
See 'codingConventions.md'.

#### Test-driven development
Team cluige uses a test-driven development approach: any new dev is tested in our other GitHub project "test_cluige". These two projects, "cluige" and "test_cluige", are maintained up to date together.

Old tests are all kept so they are always re-run for non-regression testing. Sometimes a cluige evolution forces us to fix old tests accordingly; we always do so each time it is needed.

