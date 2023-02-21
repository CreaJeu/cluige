//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Node2D.h"

#include <string.h>
#include <assert.h>

////////////////////////////////// _Node2D /////////

static void n2d_deleteNode2D(Node* thisNode)
{
    struct _Node2D* thisNode2D = (struct _Node2D*)(thisNode->_subClass);
    void (*deleteNode)(Node*) = thisNode2D->deleteNode;
    assert(thisNode2D->_subClass == NULL);
    free(thisNode2D);
    thisNode->_subClass = NULL;
    deleteNode(thisNode);
}

static void n2d_preProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    thisNode2D->_localPositionChanged = false;
}

static void n2d_postProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    Node* n = thisNode;
    bool mustUpdateGlobalPos = false;
    while((n->parent != NULL) && !mustUpdateGlobalPos)
    {
        void* nSubclass = n->_subClass;
        if(nSubclass != NULL)
        {
            char* found = strstr(n->_className, "NodeNode2D");
            if(found == n->_className) //n is a Node2D
            {
                Node2D* n2d = (Node2D*)(n->_subClass);
                mustUpdateGlobalPos = n2d->_localPositionChanged;
            }
        }
        n = n->parent;
    }
    if(mustUpdateGlobalPos)
    {
        n = thisNode;
        thisNode2D->_tmpGlobalPosition = (Vector2){0, 0};
        while(n->parent != NULL)
        {
            void* nSubclass = n->_subClass;
            if(nSubclass != NULL)
            {
                char* found = strstr(n->_className, "NodeNode2D");
                if(found == n->_className) //n is a Node2D
                {
                    Node2D* n2d = (Node2D*)(n->_subClass);
                    iCluige.iVector2.add(
                        &(thisNode2D->_tmpGlobalPosition),
                        &(n2d->position),
                        &(thisNode2D->_tmpGlobalPosition)
                        );
                }
            }
            n = n->parent;
        }
    }
}

static void n2d_enterTreeNode2D(Node* thisNode)
{
    struct _Node2D* thisNode2D = (struct _Node2D*)(thisNode->_subClass);
    thisNode2D->_localPositionChanged = true;
    n2d_postProcessNode(thisNode);//compute _tmpGlobalPosition
    thisNode2D->enterTreeNode(thisNode);//calls script.enterTree()
}


////////////////////////////////// iiNode2D /////////

static struct _Node2D* n2d_newNode2D()
{
    Node* newNode = iCluige.iNode.newNode();
    struct _Node2D* newNode2D = iCluige.checkedMalloc(sizeof(Node2D));

    newNode2D->visible = true;
    newNode2D->position = (Vector2){0., 0.};
    newNode2D->_tmpGlobalPosition = (Vector2){0., 0.};
    newNode2D->_localPositionChanged = false;

    newNode2D->_thisNode = newNode;
    newNode2D->_subClass = NULL;
    newNode2D->deleteNode = newNode->deleteNode;
    newNode2D->deleteNode2D = n2d_deleteNode2D;
    newNode2D->enterTreeNode = newNode->enterTree;
    newNode->enterTree = n2d_enterTreeNode2D;

    newNode->_subClass = newNode2D;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(&sb, strlen("NodeNode2D"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2D");
    newNode->_subClass = newNode2D;

    newNode->deleteNode = n2d_deleteNode2D;
    newNode->preProcessNode = n2d_preProcessNode;
    newNode->postProcessNode = n2d_postProcessNode;

    return newNode2D;
}

static void n2d_show(Node2D* thisNode2D)
{
    thisNode2D->visible = true;
}

static void n2d_hide(Node2D* thisNode2D)
{
    thisNode2D->visible = false;
}

static void n2d_moveLocal(Node2D* thisNode2D, Vector2 depl)
{
    iCluige.iVector2.add(
        &(thisNode2D->position),
        &(depl),
        &(thisNode2D->position)
        );
    thisNode2D->_localPositionChanged = true;
}

static void n2d_setLocalPosition(Node2D* thisNode2D, Vector2 newPos)
{
    thisNode2D->position = newPos;
    thisNode2D->_localPositionChanged = true;
}

/////////////////////////////////// Node //////////

void iiNode2DInit()
{
    iCluige.iNode2D.newNode2D = n2d_newNode2D;
    iCluige.iNode2D.show = n2d_show;
    iCluige.iNode2D.hide = n2d_hide;
    iCluige.iNode2D.moveLocal = n2d_moveLocal;
    iCluige.iNode2D.setLocalPosition = n2d_setLocalPosition;
}

