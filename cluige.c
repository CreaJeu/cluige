
#include <stdio.h>
#include <curses.h>
#include "cluige.h"
#include "Nodes/Clock.h"

////////////////////////////////// iiCluige /////////

static void* clg_checked_malloc(size_t s)//TODO : move to Utils, add a checked_realloc()
{
    void* res = malloc(s);
    if(res == NULL)
    {
        printf("\n\n\n    fatal memory alloc error !\n\n\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

static Vector2 clg_get_screen_size()
{
//    int w, h;
    //TODO : make it work !
//    if(is_termresized())
//    {
//        resize_term(0, 0);
//    }
//    getmaxyx(stdscr, h, w);
//    return (Vector2){ w, h };
    return (Vector2){ 0, 0 };
}


/////////////////////////////////// Cluige //////////

//redeclaration because of extern (like static members in c++)
struct iiCluige iCluige;

void cluige_init()
{
    iCluige.checked_malloc = clg_checked_malloc;
    iCluige.get_screen_size = clg_get_screen_size;
    iCluige.wanted_frame_seconds = .0666;//15 fps by default
    iCluige.EPSILON = 0.00001;
    iCluige.quit_asked = false;

    iiVariant_init();
    iiDeque_init();
    iiSortedDictionary_init();
    iiFileLineReader_init();
    iiStringBuilder_init();
    iiPackedScene_init();
    iiNode_init();
    iCluige._private_root_2D = iCluige.iNode.new_Node();
    iCluige.iNode.set_name(iCluige._private_root_2D, "private_root_2D");

    iiScript_init();
    iiVector2_init();
    iiNode2D_init();
    iiSpriteText_init();
    iiLineDrawerThin_init();
    iiPath2D_init();
    iiSpriteSVG_init();

    iiClock_init();
    iCluige.clock = iCluige.iClock.new_Clock();
    iCluige.iNode.add_child(iCluige._private_root_2D, iCluige.clock->_this_Node);

    iiInput_init();
    iCluige.input = iCluige.iInput.new_Input();
    iCluige.iNode.add_child(iCluige._private_root_2D, iCluige.input->_this_Node);

    iiCamera2D_init();
    Camera2D* default_camera = iCluige.iCamera2D.new_Camera2D();
    iCluige.iNode.set_name(default_camera->_this_Node2D->_this_Node,"default_camera");
    default_camera->anchor_mode = ANCHOR_MODE_FIXED_TOP_LEFT;
    iCluige.iNode.add_child(iCluige._private_root_2D, default_camera->_this_Node2D->_this_Node);
    iCluige.iCamera2D.default_camera = default_camera;
//    iCluige.iCamera2D.current_camera = default_camera;
	iCluige.iCamera2D.make_current(default_camera);
    //iCluige.iNode.print_tree_pretty(iCluige.private_root_2D);

    iCluige.public_root_2D = iCluige.iNode.new_Node();
    iCluige.iNode.set_name(iCluige.public_root_2D, "public_root_2D");
    iCluige.iNode.add_child(iCluige._private_root_2D, iCluige.public_root_2D);

    iiTscnParser_init();

    //...
    //...

    //...
    //...
	iCluige.iSortedDictionary.sorted_dictionary_alloc(
		&(iCluige._prioritized_nodes_to_process),
		VT_INT64, VT_POINTER, 7);
}

enum ProcessPass
{
//    STARTING_LOOP_PASS,
    PROCESS_PASS,
    BAKE_PASS,
    ERASE_PASS,
    DRAW_PASS
};

static void _do_process_prioritized()
{
	SortedDictionary* dico = &(iCluige._prioritized_nodes_to_process);
	int nb_deques = iCluige.iSortedDictionary.size(dico);
	for(int dico_i=0; dico_i < nb_deques; dico_i++)
	{
		Pair pair_p = iCluige.iSortedDictionary.at(dico, dico_i);
		Deque* nodes_with_prio_p = (Deque*)(pair_p.second.ptr);
		int nb_nodes = iCluige.iDeque.size(nodes_with_prio_p);
		for(int n_i=0; n_i < nb_nodes; n_i++)
		{
			Node* n = (Node*)(iCluige.iDeque.at(nodes_with_prio_p, n_i).ptr);
			//if(n->process != NULL) already checked in process_tree()
			n->process(n);
		}
		iCluige.iDeque.clear(nodes_with_prio_p);
	}
}

static void process_tree(Node* root, enum ProcessPass pass)
{
    //recursion mode : DFS
    switch(pass)
    {
//    case STARTING_LOOP_PASS:
//        if(root->on_loop_starting != NULL)
//        {
//            root->on_loop_starting(root);
//        }
//        break;
    case ERASE_PASS:
        if(root->erase != NULL)
        {
            root->erase(root);
        }
        break;
    case PROCESS_PASS:
        if(root->process != NULL)
        {
			//root->process(root);
			int64_t p = root->process_priority;
			SortedDictionary* dico = &(iCluige._prioritized_nodes_to_process);
			Checked_Variant found = iCluige.iSortedDictionary.get(dico, p);
			Deque* nodes_with_prio_p;
			if(found.valid)
			{
				nodes_with_prio_p = (Deque*)(found.v.ptr);
			}
			else
			{
				//never deleted but usually used for the whole game execution
				nodes_with_prio_p = iCluige.checked_malloc(sizeof(Deque));
				iCluige.iDeque.deque_alloc(nodes_with_prio_p, VT_POINTER, 15);
				iCluige.iSortedDictionary.insert(dico, p, nodes_with_prio_p);
			}
			iCluige.iDeque.push_back(nodes_with_prio_p, root);
        }
        break;
    case BAKE_PASS:
        if(root->bake != NULL)
        {
            root->bake(root);
        }
        break;
    case DRAW_PASS:
        if(root->draw != NULL)
        {
            root->draw(root);
        }
        break;
    }

    if(root->children != NULL)
    {
        process_tree(root->children, pass);
    }

    if(root->next_sibling != NULL)
    {
        process_tree(root->next_sibling, pass);
    }
}

void cluige_run()
{
    //curses  //TODO separate from core cluige via 'display server'
    initscr();
    nodelay(stdscr, true);
    cbreak();
    //nl();//?
    noecho();
    curs_set(0);
    timeout(0);// for getch() : 0=blocking 0=return ERR/...
    keypad(stdscr, true);

//    process_tree(iCluige._private_root_2D, STARTING_LOOP_PASS);
	//in case remove_child() was called before loop
	iCluige.iDeque.clear(&(iCluige.iNode._just_removed_nodes));

    //game loop
    while(!(iCluige.quit_asked))
    {
    	//BAKE
        process_tree(iCluige._private_root_2D, BAKE_PASS);
        int nb_just_removed = iCluige.iDeque.size(&(iCluige.iNode._just_removed_nodes));
        for(int i=0; i<nb_just_removed; i++)
		{
			Node* n = (Node*)iCluige.iDeque.at(&(iCluige.iNode._just_removed_nodes), i).ptr;
			if(n->bake != NULL)
			{
				n->bake(n);
			}
		}

		//ERASE
        process_tree(iCluige._private_root_2D, ERASE_PASS);
        for(int i=0; i<nb_just_removed; i++)
		{
			Node* n = (Node*)iCluige.iDeque.at(&(iCluige.iNode._just_removed_nodes), i).ptr;
			if(n->erase != NULL)
			{
				n->erase(n);
			}
		}

		//queue_free_early
        iCluige.iNode._do_all_queue_free_early_step();

        //DRAW
        process_tree(iCluige._private_root_2D, DRAW_PASS);

        //curses refresh
        refresh();

        //queue_free_late
        iCluige.iNode._do_all_queue_free_late_step();
		iCluige.iDeque.clear(&(iCluige.iNode._just_removed_nodes));

        //curses sleep
        int sleep_frame_milliseconds = 1;
        if(iCluige.clock->elapsed_seconds < iCluige.wanted_frame_seconds)
        {
            float sleep_frame_seconds = iCluige.wanted_frame_seconds - iCluige.clock->elapsed_seconds;
            sleep_frame_milliseconds = (int)(sleep_frame_seconds * 1000);
        }
        napms(sleep_frame_milliseconds);//sleep to avoid 100% CPU

        //PROCESS
        process_tree(iCluige._private_root_2D, PROCESS_PASS);//just computes priorities
        _do_process_prioritized();//actually processes
        //while-test about quit_asked is executed just after process
        // to avoid unnecessary erase, draw, free...
    }
}

int cluige_finish()
{
    //iCluige.iNode.delete_Node(iCluige.private_root_2D);
    iCluige._private_root_2D->delete_Node(iCluige._private_root_2D);
    //close files
    //free tmp locks
    //...


	curs_set(1);
	endwin();
    return EXIT_SUCCESS;
}

