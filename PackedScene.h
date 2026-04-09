#ifndef PACKED_SCENE_H_INCLUDED
#define PACKED_SCENE_H_INCLUDED

#include "SortedDictionary.h"
#include "Deque.h"

typedef struct _PackedScene PackedScene;

struct _PackedScene
{
	const char* name;
	const char* type;
	const char* instance_path;
	const char* parent;
	const char* uid;
	const char* tscn_format;
	//SortedDictionary<String, string>
	//   param_name -> param_value
	//warning : in this dico, 'class' names are from godot :
	//	Label instead of SpriteText, Sprite2D instead of SpriteSVG
	SortedDictionary dico_node;

	//SortedDictionary dico_svg_paths;//not used! see TscnParser

	//later : connected signals

	//Deque<PackedScene*>
	Deque children;
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiPackedScene
{
	//Dico<path, packed_scene>
	//~static public
	SortedDictionary dico_path_to_packed;

	void (*packed_scene_alloc)(PackedScene* this_PackedScene);
	PackedScene* (*new_PackedScene)();

//	void (*pre_delete_PackedScene)(PackedScene* this_PackedScene);// maybe one day

	PackedScene* (*get_packed_node)(PackedScene* root, const char* path);

	//returns NULL if not found
	PackedScene* (*packed_scene_from_uid)(const char* uid);

	Node* (*instantiate)(const PackedScene* this_PackedScene);

	void (*prepare_ext_instantiate)(PackedScene* this_PackedScene, const PackedScene* ext_other, const PackedScene* local_other);

	//returned char* is malloced, up to the user to delete it
	char* (*debug)(const PackedScene* this_PackedScene);
	char* (*debug_recrusive)(const PackedScene* root, Deque* strings);
};
//iPackedScene : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiPackedScene_init();


#endif // PACKED_SCENE_H_INCLUDED

