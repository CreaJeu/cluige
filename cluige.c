
#include <stdio.h>
#include <curses.h>
#include "cluige.h"
#include "Nodes/Clock.h"

////////////////////////////////// iiCluige /////////

static void* clg_checked_malloc(size_t s)
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
    int w, h;
    //TODO : make it work !
    if(is_termresized())
    {
        resize_term(0, 0);
    }
    getmaxyx(stdscr, h, w);
    return (Vector2){ w, h };
}


/////////////////////////////////// Cluige //////////

//redeclaration because of extern (like static members in c++)
struct iiCluige iCluige;

void cluige_init()
{
    iCluige.checked_malloc = clg_checked_malloc;
    iCluige.get_screen_size = clg_get_screen_size;
    iCluige.wanted_frame_seconds = .0666;//15 fps by default
    iCluige.quit_asked = false;

    iiVariant_init();
    iiDeque_init();
    iiSortedDictionary_init();
    iiStringBuilder_init();
    iiNode_init();
    iCluige.private_root_2D = iCluige.iNode.new_Node();
    iCluige.iNode.set_name(iCluige.private_root_2D, "private_root_2D");

    iiScript_init();
    iiVector2_init();
    iiNode2D_init();
    iiSpriteText_init();
    iiLineDrawerThin_init();
    iiPath2D_init();
    iiSpriteSVG_init();

    iiClock_init();
    iCluige.clock = iCluige.iClock.new_Clock();
    iCluige.iNode.add_child(iCluige.private_root_2D, iCluige.clock->_this_Node);

    iiInput_init();
    iCluige.input = iCluige.iInput.new_Input();
    iCluige.iNode.add_child(iCluige.private_root_2D, iCluige.input->_this_Node);

    iCluige.public_root_2D = iCluige.iNode.new_Node();
    iCluige.iNode.set_name(iCluige.public_root_2D, "public_root_2D");
    iCluige.iNode.add_child(iCluige.private_root_2D, iCluige.public_root_2D);

    //...
    //...

    //...
    //...

    //curses  //TODO separate from core cluige via 'display server'
    initscr();
    nodelay(stdscr, true);
    cbreak();
    //nl();//?
    noecho();
    curs_set(0);
    timeout(0);// for getch() : 0=blocking 0=return ERR/...
    keypad(stdscr, true);
}

enum ProcessPass
{
    STARTING_LOOP_PASS,
    PRE_PROCESS_PASS,
    PROCESS_PASS,
    POST_PROCESS_PASS
};

static void process_tree(Node* root, enum ProcessPass pass)
{
    //recursion mode : DFS
    if(root->children != NULL)
    {
        process_tree(root->children, pass);
    }

    switch(pass)
    {
    case STARTING_LOOP_PASS:
        if(root->on_loop_starting != NULL)
        {
            root->on_loop_starting(root);
        }
        break;
    case PRE_PROCESS_PASS:
        if(root->pre_process_Node != NULL)
        {
            root->pre_process_Node(root);
        }
        break;
    case PROCESS_PASS:
        if(root->process_Node != NULL)
        {
            root->process_Node(root);
        }
        break;
    case POST_PROCESS_PASS:
        if(root->post_process_Node != NULL)
        {
            root->post_process_Node(root);
        }
        break;
    }

    if(root->next_sibling != NULL)
    {
        process_tree(root->next_sibling, pass);
    }
}

void cluige_run()
{
    process_tree(iCluige.private_root_2D, STARTING_LOOP_PASS);

    //game loop
    while(!(iCluige.quit_asked))
    {
        process_tree(iCluige.private_root_2D, PRE_PROCESS_PASS);
        process_tree(iCluige.private_root_2D, PROCESS_PASS);
        process_tree(iCluige.private_root_2D, POST_PROCESS_PASS);

        refresh();
        iCluige.iNode._do_all_queue_free();
        //curses
        int sleep_frame_milliseconds = 1;

        if(iCluige.clock->elapsed_seconds < iCluige.wanted_frame_seconds)
        {
            float sleep_frame_seconds = iCluige.wanted_frame_seconds - iCluige.clock->elapsed_seconds;
            sleep_frame_milliseconds = (int)(sleep_frame_seconds * 1000);
        }
        napms(sleep_frame_milliseconds);//sleep to avoid 100% CPU
    }
}

int cluige_finish()
{
    //iCluige.iNode.delete_Node(iCluige.private_root_2D);
    iCluige.private_root_2D->delete_Node(iCluige.private_root_2D);
    //close files
    //free tmp locks
    //...


	curs_set(1);
	endwin();
    return EXIT_SUCCESS;
}

