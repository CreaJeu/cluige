
#include "cluige.h"
#include "Deque.h"
#include "SortedDictionary.h"

#include <stdarg.h>



////////////////////////////////// iiSortedDictionary /////////

//constructor/destructor (don't forget to use them)

static void sd_sorted_dictionary_alloc(SortedDictionary* this_SortedDictionary, VariantType keys_type, VariantType values_type, int capacity)
{
    iCluige.iDeque.deque_alloc(&(this_SortedDictionary->_pairs), VT_POINTER, capacity);
    this_SortedDictionary->_pairs._sorted = true;
    this_SortedDictionary->_pairs._compare_func = iCluige.iDeque.default_compare_pair_key_func;
    this_SortedDictionary->_keys_type = keys_type;
    this_SortedDictionary->_values_type = values_type;
//    this_SortedDictionary->compare_keys_func = sd__deque_compare_func;
}

//just a forward declaration, see below
static void sd_clear(SortedDictionary* this_SortedDictionary);

//if elements are pointers, they must be deleted outside of this function,
//according to the user needs
static void sd_pre_delete_SortedDictionary(SortedDictionary* this_SortedDictionary)
{
    sd_clear(this_SortedDictionary);
    iCluige.iDeque.pre_delete_Deque(&(this_SortedDictionary->_pairs));
}

static void sd_free_all_keys_pointers(SortedDictionary* this_SortedDictionary)
{
    Deque* pairs = &(this_SortedDictionary->_pairs);
    int nb_points = iCluige.iDeque.size(pairs);
    for(int j=0; j<nb_points; j++)
    {
        Pair* pair_j = (Pair*)(iCluige.iDeque.at(pairs, j).ptr);
        void* key_j = pair_j->first.ptr;
        free(key_j);
    }
}

static void sd_free_all_values_pointers(SortedDictionary* this_SortedDictionary)
{
    Deque* pairs = &(this_SortedDictionary->_pairs);
    int nb_points = iCluige.iDeque.size(pairs);
    for(int j=0; j<nb_points; j++)
    {
        Pair* pair_j = (Pair*)(iCluige.iDeque.at(pairs, j).ptr);
        void* value_j = pair_j->second.ptr;
        free(value_j);
    }
}

static void sd_set_compare_keys_func(SortedDictionary* this_SortedDictionary, int (*compare_key_func)(const Deque*, Variant, Variant))
{
    this_SortedDictionary->_pairs._compare_sub_func = compare_key_func;
}


//read

static Checked_Variant sd_get(const SortedDictionary* this_SortedDictionary, ...)
{
    Checked_Variant res;
    const Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_SortedDictionary);
    Variant key = iCluige.iVariant.from_s_args(
                _pairs->_elems_type, &s_args_elem_value);
    va_end(s_args_elem_value.args);

    Pair new_pair = { key, iCluige.iVariant.NULL_VARIANT };
    int i = iCluige.iDeque.bsearch(_pairs, new_pair);
    if(i == -1)
    {
        res.valid = false;
        return res;
    }
    else
    {
        Pair* found = (Pair*)((iCluige.iDeque.at(_pairs, i)).ptr);
        res.v = found->second;
        res.valid = true;
        return res;
    }
}

static Variant sd_at(const SortedDictionary* this_SortedDictionary, int i)
{
    const Deque* _pairs = &(this_SortedDictionary->_pairs);
    return iCluige.iDeque.at(_pairs, i);
}

static int sd_size(const SortedDictionary* this_SortedDictionary)
{
    return iCluige.iDeque.size(&(this_SortedDictionary->_pairs));
}

static bool sd_is_empty(const SortedDictionary* this_SortedDictionary)
{
    return iCluige.iDeque.empty(&(this_SortedDictionary->_pairs));
}


//insertion

static void sd_insert_first(SortedDictionary* this_SortedDictionary, ...)
{
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_key_value;
    va_start(s_args_key_value.args, this_SortedDictionary);

    Pair new_pair_local = iCluige.iVariant.pair_from_args(
            this_SortedDictionary->_keys_type, this_SortedDictionary->_values_type,
            &s_args_key_value);
    va_end(s_args_key_value.args);

    Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
    *new_pair = new_pair_local;
    iCluige.iDeque.push_front(_pairs, new_pair);
}

static void sd_insert_last(SortedDictionary* this_SortedDictionary, ...)
{
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_key_value;
    va_start(s_args_key_value.args, this_SortedDictionary);

    Pair new_pair_local = iCluige.iVariant.pair_from_args(
            this_SortedDictionary->_keys_type, this_SortedDictionary->_values_type,
            &s_args_key_value);
    va_end(s_args_key_value.args);

    Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
    *new_pair = new_pair_local;
    iCluige.iDeque.push_back(_pairs, new_pair);
}

//variadic for easier use than Variant
//inserts or automatically replaces if key already present
//returns a copy of replaced value (what was its value before replacement),
//for example if you need to do some free/delete
//result.valid = false if no elem was replaced
static Checked_Variant sd_insert(SortedDictionary* this_SortedDictionary, ...)
{
    Checked_Variant res;
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_key_value;
    va_start(s_args_key_value.args, this_SortedDictionary);

    Pair new_pair_local = iCluige.iVariant.pair_from_args(
            this_SortedDictionary->_keys_type, this_SortedDictionary->_values_type,
            &s_args_key_value);
    va_end(s_args_key_value.args);
    int this_size = iCluige.iDeque.size(_pairs);
    if(this_size == 0)
    {
        Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
        *new_pair = new_pair_local;
        iCluige.iDeque.push_back(_pairs, new_pair);
        res.valid = false;
        return res;
    }
    Variant new_variant_pair_local;
    new_variant_pair_local.ptr = &new_pair_local;

    struct _BSearchData bsd;
    bsd._i_min = 0;
    bsd._i_max = this_size - 1;
    iCluige.iDeque.bsearch_rec(_pairs, new_variant_pair_local, &bsd);
    if(bsd._found)
    {
        Pair* found = (Pair*)((iCluige.iDeque.at(_pairs, bsd._found_deque_index)).ptr);
        Variant replaced_value = found->second;//copy
        found->second = new_pair_local.second;
        res.v = replaced_value;
        res.valid = true;
        return res;
    }
    else
    {
        Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
        *new_pair = new_pair_local;
        iCluige.iDeque.insert(_pairs, bsd._found_insert_deque_index, new_pair);
        res.valid = false;
        return res;
    }
}


//deletion

//no auto free/delete
static Variant sd_erase(SortedDictionary* this_SortedDictionary, ...)
{
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    int this_size = iCluige.iDeque.size(_pairs);
    if(this_size == 0)
    {
        return iCluige.iVariant.NULL_VARIANT;
    }
    struct _Structed_va_list s_args_key;
    va_start(s_args_key.args, this_SortedDictionary);
    Variant key_var = iCluige.iVariant.from_s_args(
        this_SortedDictionary->_keys_type, &s_args_key);
    va_end(s_args_key.args);
    Pair new_pair_local = { key_var, iCluige.iVariant.NULL_VARIANT };
    Variant variant_pair_local;
    variant_pair_local.ptr = &new_pair_local;

    struct _BSearchData bsd;
    bsd._i_min = 0;
    bsd._i_max = this_size - 1;
    iCluige.iDeque.bsearch_rec(_pairs, variant_pair_local, &bsd);
    if(bsd._found)
    {
        Pair* found = (Pair*)((iCluige.iDeque.at(_pairs, bsd._found_deque_index)).ptr);
//        int64_t dbg_k = found->first.i64;
//        char* dbg_val = (char*)(found->second.ptr);
        Variant replaced_value = found->second;//copy
        iCluige.iDeque.remove(_pairs, bsd._found_deque_index);
        free(found);//TODO memory pool for pairs (maybe thread-safe ready?)
        return replaced_value;
    }
    else
    {
        return iCluige.iVariant.NULL_VARIANT;
    }
}

//no auto free/delete
static void sd_clear(SortedDictionary* this_SortedDictionary)
{
    //pairs are internal types, automatically malloced by SortedDictionary
    //so, must be freed
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    int size = iCluige.iDeque.size(_pairs);
    for(int i=0; i < size; i++)
    {
        Variant v = iCluige.iDeque.at(_pairs, i);//copy
        free(v.ptr);//free pair
    }
    iCluige.iDeque.clear(_pairs);
}

//only for Dico<char*, char*>
//returned char* is malloced, up to the user to delete it
static char* sd_debug_str_str(SortedDictionary* this_SortedDictionary)
{
	const SortedDictionary* dic = this_SortedDictionary;
	int len = 16 + iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;//" - nb elems : ####\n"
	CLUIGE_ASSERT(dic->_keys_type == VT_POINTER, "SortedDictionary::debug_str_str() : keys type must be VT_POINTER");
	CLUIGE_ASSERT(dic->_values_type == VT_POINTER, "SortedDictionary::debug_str_str() : (key,value) types must be (VT_POINTER,VT_POINTER)");
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
	for(int i=0; i<n; i++)
	{
		Variant v_pair_i = iCluige.iSortedDictionary.at(dic, i);
		Pair* pair_i = (Pair*)(v_pair_i.ptr);
		const char* key = (const char*)(pair_i->first.ptr);
		const char* val = (const char*)(pair_i->second.ptr);
		iCluige.iStringBuilder.append(&sb, "%s = %s\n", key, val);
	}
	iCluige.iStringBuilder.append(&sb, " - nb elems : %d", n);
	return sb.built_string;
}

////////////////////////////////// iSortedDictionary /////////

void iiSortedDictionary_init()
{
    iCluige.iSortedDictionary.sorted_dictionary_alloc = sd_sorted_dictionary_alloc;
    iCluige.iSortedDictionary.pre_delete_SortedDictionary = sd_pre_delete_SortedDictionary;
    iCluige.iSortedDictionary.free_all_keys_pointers = sd_free_all_keys_pointers;
    iCluige.iSortedDictionary.free_all_values_pointers = sd_free_all_values_pointers;
    iCluige.iSortedDictionary.set_compare_keys_func = sd_set_compare_keys_func;
    iCluige.iSortedDictionary.get = sd_get;
    iCluige.iSortedDictionary.at = sd_at;
    iCluige.iSortedDictionary.size = sd_size;
    iCluige.iSortedDictionary.is_empty = sd_is_empty;
    iCluige.iSortedDictionary.insert = sd_insert;
    iCluige.iSortedDictionary.insert_first = sd_insert_first;
    iCluige.iSortedDictionary.insert_last = sd_insert_last;
    iCluige.iSortedDictionary.erase = sd_erase;
    iCluige.iSortedDictionary.clear = sd_clear;
    iCluige.iSortedDictionary.debug_str_str = sd_debug_str_str;
}

