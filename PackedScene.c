
#include <assert.h>

#include "cluige.h"
#include "PackedScene.h"


////////////////////////////////// _PackedScene /////////


////////////////////////////////// iiPackedScene /////////


static void pksc_packed_scene_alloc(PackedScene* this_PackedScene)
{
    SortedDictionary* dico_node = &(this_PackedScene->dico_node);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(dico_node, VT_POINTER, VT_POINTER, 7);

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

static Node* pksc_instanciate(const PackedScene* this_PackedScene)
{
    assert(00=="TODO");
    return NULL;
}

/////////////////////////////////// iPackedScene //////////

void iiPackedScene_init()
{
    iCluige.iPackedScene.packed_scene_alloc = pksc_packed_scene_alloc;
    iCluige.iPackedScene.pre_delete_PackedScene = pksc_pre_delete_PackedScene;
    iCluige.iPackedScene.new_PackedScene = pksc_new_PackedScene;
    iCluige.iPackedScene.instanciate = pksc_instanciate;
}

