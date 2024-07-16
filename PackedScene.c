
#include <assert.h>

#include "cluige.h"
#include "PackedScene.h"


////////////////////////////////// _PackedScene /////////


////////////////////////////////// iiPackedScene /////////


static void pksc_packed_scene_alloc(PackedScene* this_PackedScene)
{
    SortedDictionary* dico_node = &(this_PackedScene->dico_node);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(dico_node, VT_POINTER, VT_POINTER, 7);
    iCluige.iSortedDictionary.set_compare_keys_func(dico_node, iCluige.iDeque.default_compare_string_func);

    //later : script
    //later : signals

    Deque* children = &(this_PackedScene->children);
    iCluige.iDeque.deque_alloc(children, VT_POINTER, 2);
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
	assert(child_name_len > 0);
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
		cmp = strncmp(ps->name, path, child_name_len);
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
	assert(00=="TODO");
	return NULL;
}

static char* pksc_debug(const PackedScene* this_PackedScene)
{
	int len = 15 + iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;//" - children : ####"
	len += 8 + strlen(this_PackedScene->name);//"name = AAA\n"
	len += 8 + strlen(this_PackedScene->name);//"type = BBB\n"
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
		Variant v_pair_i = iCluige.iSortedDictionary.at(dic, i);
		Pair* pair_i = (Pair*)(v_pair_i.ptr);
		const char* key = (const char*)(pair_i->first.ptr);
		const char* val = (const char*)(pair_i->second.ptr);
		len += 4 + strlen(key) + strlen(val);//"key = val\n"
	}
	StringBuilder sb;
	iCluige.iStringBuilder.string_alloc(&sb, len);

	iCluige.iStringBuilder.append(&sb, "name = %s\n", this_PackedScene->name);
	iCluige.iStringBuilder.append(&sb, "type = %s\n", this_PackedScene->type);
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
		Variant v_pair_i = iCluige.iSortedDictionary.at(dic, i);
		Pair* pair_i = (Pair*)(v_pair_i.ptr);
		const char* key = (const char*)(pair_i->first.ptr);
		const char* val = (const char*)(pair_i->second.ptr);
		iCluige.iStringBuilder.append(&sb, "%s = %s\n", key, val);
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

