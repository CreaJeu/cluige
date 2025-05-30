
#include "cluige.h"
#include "PackedScene.h"


////////////////////////////////// _PackedScene /////////


////////////////////////////////// iiPackedScene /////////


static void pksc_packed_scene_alloc(PackedScene* this_PackedScene)
{
    SortedDictionary* dico_node = &(this_PackedScene->dico_node);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(dico_node, VT_POINTER, VT_POINTER, 7);
    iCluige.iSortedDictionary.set_compare_keys_func(dico_node, iCluige.iDeque.default_compare_string_func);

    SortedDictionary* dico_svg_paths = &(this_PackedScene->dico_svg_paths);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(dico_svg_paths, VT_POINTER, VT_POINTER, 7);
    iCluige.iSortedDictionary.set_compare_keys_func(dico_svg_paths, iCluige.iDeque.default_compare_string_func);

    //later : script
    //later : signals

    Deque* children = &(this_PackedScene->children);
    iCluige.iDeque.deque_alloc(children, VT_POINTER, 2);
    this_PackedScene->type = NULL;
    this_PackedScene->instance_res = NULL;
}

static PackedScene* pksc_new_PackedScene()
{
    PackedScene* new_PackedScene = iCluige.checked_malloc(sizeof(PackedScene));
    pksc_packed_scene_alloc(new_PackedScene);
    return new_PackedScene;
}

static void pksc_pre_delete_PackedScene(PackedScene* this_PackedScene)
{

    SortedDictionary* dico_node = &(this_PackedScene->dico_node);
    iCluige.iSortedDictionary.free_all_keys_pointers(dico_node);
    iCluige.iSortedDictionary.free_all_values_pointers(dico_node);
    iCluige.iSortedDictionary.pre_delete_SortedDictionary(dico_node);
    //later : script
    //later : signals
    Deque* children = &(this_PackedScene->children);
    iCluige.iDeque.free_all_elems_pointers(children);
    iCluige.iDeque.pre_delete_Deque(children);
}

static PackedScene* pksc_get_packed_node(PackedScene* root, const char* path)
{
	int child_name_len = strlen(path);
	CLUIGE_ASSERT(child_name_len > 0, "PackedScene::get_packed_node() : path is empty");
	const char* first_slash = strchr(path, '/');
	if(first_slash != NULL)
	{
		child_name_len = (first_slash - path) / sizeof(char);
	}
	Deque* children = &(root->children);
	int n = iCluige.iDeque.size(children);
	PackedScene* ps = NULL;
	int cmp = -1;
	for(int i=0; i<n; i++)
	{
		ps = (PackedScene*)(iCluige.iDeque.at(children, i).ptr);
		char sub_path[iCluige.iNode._MAX_NAME_LENGTH];
		strncpy(sub_path, path, child_name_len);
		sub_path[child_name_len] = '\0';
		cmp = strcmp(sub_path, ps->name);
		if(cmp == 0)
		{
			if(first_slash == NULL)
			{
				return ps;
			}
			else
			{
				return pksc_get_packed_node(ps, first_slash + 1);
			}
		}
	}
	return NULL;
}

static Node* pksc_instanciate(const PackedScene* this_PackedScene)
{
	//TODO type XOR instance_res : instance_res => instance from other scene
	const char* t = this_PackedScene->type;
	const SortedDictionary* fcties = &(iCluige.iNode.node_factories);
	Checked_Variant got = iCluige.iSortedDictionary.get(fcties, t);
	CLUIGE_ASSERT(got.valid, "PackedScene::instanciate() : type found in PackedScene in unknown by cluige");
	const NodeFactory* fcty = (const NodeFactory*)(got.v.ptr);
	Node* new_node = fcty->instanciate(&(this_PackedScene->dico_node));

	//children
	int nb_children = iCluige.iDeque.size(&(this_PackedScene->children));
	for(int c = 0; c < nb_children; c++)
	{
		const PackedScene* child_ps =
			(const PackedScene*)(iCluige.iDeque.at(&(this_PackedScene->children), c).ptr);
		Node* child_nde = pksc_instanciate(child_ps);
		iCluige.iNode.add_child(new_node, child_nde);
	}
	return new_node;
}

static char* pksc_debug(const PackedScene* this_PackedScene)
{
	int len = 15 + iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;//" - children : ####"
	len += 8 + strlen(this_PackedScene->name);//"name = AAA\n"
	if(this_PackedScene->type != NULL)
	{
		len += 8 + strlen(this_PackedScene->type);//"type = BBB\n"
	}
	if(this_PackedScene->instance_res != NULL)
	{
		len += 20 + strlen(this_PackedScene->instance_res);//"instance resource = BBB\n"
	}
	int parent_len = 0;
	if(this_PackedScene->parent != NULL)
	{
		parent_len = strlen(this_PackedScene->parent);
	}
	len += 10 + parent_len;//"parent = CCC\n"
	const SortedDictionary* dic = &(this_PackedScene->dico_node);
	int n = iCluige.iSortedDictionary.size(dic);
	for(int i=0; i<n; i++)
	{
		Pair pair_i = iCluige.iSortedDictionary.at(dic, i);
		const char* key = (const char*)(pair_i.first.ptr);
		const char* val = (const char*)(pair_i.second.ptr);
		if(strcmp(key, "name") != 0)
		{
			len += 4 + strlen(key) + strlen(val);//"key = val\n"
		}
	}
	StringBuilder sb;
	iCluige.iStringBuilder.string_alloc(&sb, len);

	iCluige.iStringBuilder.append(&sb, "name = %s\n", this_PackedScene->name);
	if(this_PackedScene->type != NULL)
	{
		iCluige.iStringBuilder.append(&sb, "type = %s\n", this_PackedScene->type);
	}
	if(this_PackedScene->instance_res != NULL)
	{
		iCluige.iStringBuilder.append(&sb, "instance resource = %s\n", this_PackedScene->instance_res);
	}
	if(this_PackedScene->parent == NULL)
	{
		iCluige.iStringBuilder.append(&sb, "parent =\n");
	}
	else
	{
		iCluige.iStringBuilder.append(&sb, "parent = %s\n", this_PackedScene->parent);
	}
	for(int i=0; i<n; i++)
	{
		Pair pair_i = iCluige.iSortedDictionary.at(dic, i);
		const char* key = (const char*)(pair_i.first.ptr);
		const char* val = (const char*)(pair_i.second.ptr);
		if(strcmp(key, "name") != 0)
		{
			iCluige.iStringBuilder.append(&sb, "%s = %s\n", key, val);
		}
	}

	int nb_children = iCluige.iDeque.size(&(this_PackedScene->children));
	iCluige.iStringBuilder.append(&sb, " - children : %d", nb_children);
	return sb.built_string;
}

static char* pksc_debug_recrusive(const PackedScene* root, Deque* strings)
{
	Deque strings_candidate;
	if(root->parent == NULL)
	{
		iCluige.iDeque.deque_alloc(&strings_candidate, VT_POINTER, 8);
		strings = &strings_candidate;
	}

	iCluige.iDeque.push_back(strings, pksc_debug(root));
	const Deque* chil = &(root->children);
	int n = iCluige.iDeque.size(chil);
	for(int i=0; i<n; i++)
	{
		const PackedScene* child_i = (const PackedScene*)(iCluige.iDeque.at(chil, i).ptr);
		pksc_debug_recrusive(child_i, strings);
	}

	if(root->parent == NULL)
	{
		int len = 1;
		int sn = iCluige.iDeque.size(strings);
		for(int i=0; i<sn; i++)
		{
			char* string_i = (char*)(iCluige.iDeque.at(strings, i).ptr);
			len += 2 + strlen(string_i);// + \n\n
		}
		StringBuilder sb;
		char* res = iCluige.iStringBuilder.string_alloc(&sb, len);
		for(int i=0; i<sn; i++)
		{
			char* string_i = (char*)(iCluige.iDeque.at(strings, i).ptr);
			iCluige.iStringBuilder.append(&sb, "%s\n\n", string_i);
		}
		iCluige.iDeque.free_all_elems_pointers(&strings_candidate);
		iCluige.iDeque.pre_delete_Deque(&strings_candidate);
		return res;
	}
	else
	{
		return NULL;
	}
}


/////////////////////////////////// iPackedScene //////////

void iiPackedScene_init()
{
    iCluige.iPackedScene.packed_scene_alloc = pksc_packed_scene_alloc;
    iCluige.iPackedScene.pre_delete_PackedScene = pksc_pre_delete_PackedScene;
    iCluige.iPackedScene.new_PackedScene = pksc_new_PackedScene;
    iCluige.iPackedScene.get_packed_node = pksc_get_packed_node;
    iCluige.iPackedScene.instanciate = pksc_instanciate;
    iCluige.iPackedScene.debug = pksc_debug;
    iCluige.iPackedScene.debug_recrusive = pksc_debug_recrusive;
}

