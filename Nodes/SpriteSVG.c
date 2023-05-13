//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
#include "SpriteSVG.h"

//#include <string.h>
#include <assert.h>
#include <curses.h>
//#include <math.h>

////////////////////////////////// _SpriteSVG /////////

static void ssvg_deleteSpriteSVG(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteSVG* thisSpriteSVG = (SpriteSVG*)(thisNode2D->_subClass);
    void (*deleteNode2D)(Node*) = thisSpriteSVG->deleteNode2D;
    //delete my attributes
    //Deque<Deque<Vector2*>* > paths
    Deque* paths = &(thisSpriteSVG->paths);
    int nbPaths = iCluige.iDeque.size(paths);
    for(int i=0; i<nbPaths; i++)
    {
        Deque* pathI = iCluige.iDeque.at(paths, i).ptr;
        int nbPoints = iCluige.iDeque.size(pathI);
        for(int j=0; j<nbPoints; j++)
        {
            Vector2* pointJ = iCluige.iDeque.at(pathI, j).ptr;
            free(pointJ);
        }
        iCluige.iDeque.deleteDeque(pathI);
    }
    iCluige.iDeque.deleteDeque(paths);

    assert(thisSpriteSVG->_subClass == NULL);
    free(thisSpriteSVG);
    thisNode2D->_subClass = NULL;
    deleteNode2D(thisNode);
}

//utility function
static void ssvg_drawLine(const float start_x, const float start_y, const float end_x, const float end_y, bool draw)
{
    Vector2 start_pos = {start_x, start_y};
    Vector2 end_pos = {end_x, end_y};

	LineDrawerThin d;
    iCluige.iLineDrawerThin.start(&d, &start_pos, &end_pos);
	while(!iCluige.iLineDrawerThin.hasFinished(&d))
	{
	    if(draw)
        {
            const char* glyph = iCluige.iLineDrawerThin.glyph(&d);
            //mvaddch(d.current_position.y, d.current_position.x, shade(d.curr_dist));
            mvaddstr(d.current_position.y, d.current_position.x, glyph);
        }
        else
        {
            //erase
            mvaddch(d.current_position.y, d.current_position.x, ' ');
        }

		iCluige.iLineDrawerThin.next(&d);
	}
}

static void ssvg_preProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteSVG* thisSpriteSVG = (SpriteSVG*)(thisNode2D->_subClass);
    //call super()
    thisSpriteSVG->preProcessNode2D(thisNode);

    if(!(thisNode2D->visible))
    {
        return;
    }

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.substract(
            &(thisNode2D->_tmpGlobalPosition),
            &(thisSpriteSVG->offset),
            &orig);
    float sX = thisSpriteSVG->scale.x;
    float sY = thisSpriteSVG->scale.y;
    Deque* paths = &(thisSpriteSVG->paths);
    int nbPaths = iCluige.iDeque.size(paths);
    for(int i=0; i<nbPaths; i++)
    {
        Deque* pathI = iCluige.iDeque.at(paths, i).ptr;
        int nbPoints = iCluige.iDeque.size(pathI);
        for(int j=0; j<(nbPoints-1); j++)
        {
            Vector2* p1 = iCluige.iDeque.at(pathI, j).ptr;
            Vector2* p2 = iCluige.iDeque.at(pathI, j+1).ptr;
            ssvg_drawLine(
                orig.x + ((p1->x) * sX), orig.y + ((p1->y) * sY),
                orig.x + ((p2->x) * sX), orig.y + ((p2->y) * sY),
                false);
        }
    }
}

static void ssvg_postProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteSVG* thisSpriteSVG = (SpriteSVG*)(thisNode2D->_subClass);
    //call super()
    thisSpriteSVG->postProcessNode2D(thisNode);

    if(!(thisNode2D->visible))
    {
        return;
    }

    //draw new one
    Vector2 orig;
    iCluige.iVector2.substract(
            &(thisNode2D->_tmpGlobalPosition),
            &(thisSpriteSVG->offset),
            &orig);
    float sX = thisSpriteSVG->scale.x;
    float sY = thisSpriteSVG->scale.y;
    Deque* paths = &(thisSpriteSVG->paths);
    int nbPaths = iCluige.iDeque.size(paths);
    for(int i=0; i<nbPaths; i++)
    {
        Deque* pathI = iCluige.iDeque.at(paths, i).ptr;
        int nbPoints = iCluige.iDeque.size(pathI);
        for(int j=0; j<(nbPoints-1); j++)
        {
            Vector2* p1 = iCluige.iDeque.at(pathI, j).ptr;
            Vector2* p2 = iCluige.iDeque.at(pathI, j+1).ptr;
            ssvg_drawLine(
                orig.x + ((p1->x) * sX), orig.y + ((p1->y) * sY),
                orig.x + ((p2->x) * sX), orig.y + ((p2->y) * sY),
                true);
        }
    }
}

////////////////////////////////// iiSpriteSVG /////////

static SpriteSVG* ssvg_newSpriteSVG()
{
	Node2D* newNode2D = iCluige.iNode2D.newNode2D();
	Node* newNode = newNode2D->_thisNode;
    SpriteSVG* newSpriteSVG = iCluige.checkedMalloc(sizeof(SpriteSVG));

    newSpriteSVG->offset = (Vector2) { 0., 0. };
    newSpriteSVG->scale = (Vector2) { 1., 1. };
    iCluige.iDeque.dequeAlloc(&(newSpriteSVG->paths), VT_POINTER, 3);

	newSpriteSVG->_thisNode2D = newNode2D;
	newSpriteSVG->_subClass = NULL;
	newSpriteSVG->deleteNode2D = newNode->deleteNode;
	newSpriteSVG->preProcessNode2D = newNode->preProcessNode;
	newSpriteSVG->postProcessNode2D = newNode->postProcessNode;

    newNode2D->_subClass = newSpriteSVG;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(
            &sb, strlen("NodeNode2DSpriteSVG"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteSVG");
    newNode2D->_subClass = newSpriteSVG;

    newNode->deleteNode = ssvg_deleteSpriteSVG;
    newNode->preProcessNode = ssvg_preProcessNode;
    newNode->postProcessNode = ssvg_postProcessNode;

    return newSpriteSVG;
}

static void ssvg_add_path_from_array(SpriteSVG* thisSpriteSVG, Vector2* points, int nbPoints)
{
    Deque* newPath = iCluige.checkedMalloc(sizeof(Deque));
    iCluige.iDeque.dequeAlloc(newPath, VT_POINTER, nbPoints);
    for(int i=0; i<nbPoints; i++)
    {
        Vector2* pointI = iCluige.checkedMalloc(sizeof(Vector2));
        *pointI = points[i];
        iCluige.iDeque.push_back(newPath, pointI);
    }
    iCluige.iDeque.push_back(&(thisSpriteSVG->paths), newPath);
}

static void ssvg_add_path_from_array_relative(SpriteSVG* thisSpriteSVG, Vector2* points, int nbPoints)
{
    Deque* newPath = iCluige.checkedMalloc(sizeof(Deque));
    iCluige.iDeque.dequeAlloc(newPath, VT_POINTER, nbPoints);
    Vector2 prev = (Vector2){0,0};
    for(int i=0; i<nbPoints; i++)
    {
        Vector2* pointI = iCluige.checkedMalloc(sizeof(Vector2));
        iCluige.iVector2.add(&prev, &(points[i]), pointI);
        prev = *pointI;
        iCluige.iDeque.push_back(newPath, pointI);
    }
    iCluige.iDeque.push_back(&(thisSpriteSVG->paths), newPath);
}

/////////////////////////////////// Node //////////

void iiSpriteSVGInit()
{
    iCluige.iSpriteSVG.newSpriteSVG = ssvg_newSpriteSVG;
    iCluige.iSpriteSVG.add_path_from_array = ssvg_add_path_from_array;
    iCluige.iSpriteSVG.add_path_from_array_relative = ssvg_add_path_from_array_relative;
}

