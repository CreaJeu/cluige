# cluige
Cluige, for Command Line Interface Game Engine, is an ascii/unicode/console/terminal minimalist game engine. It provides only core features like scene tree, game clock, sprite or keyboard input. More advanced features are meant to be kept separate as third party plugins. Please contact us to share your plugins, we'll maintain a list of them somewhere. This project is just beginning, we hope it to grow with time.

 	
### Low-tech

Why text/ascii/unicode/console/terminal/clui? Because it runs on much much less powerful devices! It could, one day, even run on e-readers with passive display and almost no CPU. With all the current environmental and resources consumption challenges, it should be the first concern of every today's projects. An ascii display is exaggerated, but this exaggeration is a choice to demonstrate that real needs (for example the deep cerebral things that make us like games) can be satisfied by virtuous low-tech. Not a regression, low-tech is sustainable progress.

At first we plan to remain on 2D games. Maybe we'll work later on very basic 3D features, but not now. Non low-tech features, like heavy physics or lightning will not be added nor encouraged.


### Inspired by Godot

Cluige design is very similar to Godot game engine. Everything in your game is made of Nodes, sometimes with attached scripts, organized in trees. Of course, cluige has much less features, but the same design philosophy is followed. Many functions and types names are the same.

We don't plan to provide any editor. Later, we'd like to provide instead a Godot scene file importer, so your game designers can prepare scenes in Godot editor, save them into usual .tscn files and give those directly to cluige.


### Compatibility

Cluige and our other ascii/console projects are _meant_ to be compatible with as many systems as possible. The work is still in progress, testing takes time, so unstable versions have no compatibility guaranteed unless specified. Without any notice, the most probable compatibility is with windows 10.


### BUILD

Only usual C here (C11 compliant so far). Just compile it your favourite way. Only one dependency: our other project _textraster_.

A Code::Blocks project file is included (cluige.cbp), for those who like it, providing just straightforward compilation as static lib. It assumes _cluige/_ directory is side by side with _textraster/_ directory.


### USE

Game or plugin developers : until we write more doc about it, look how Scripts are added to Nodes. You'll give there a function pointer for cluige to call your code.

Our first game using cluige, idle_cluige, can be used as a first example. Because of the similarity, you can use Godot doc for general understanding, especially when types or functions have the same name.

