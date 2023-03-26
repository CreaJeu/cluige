//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include <stdio.h> //for printTreePretty()
#include <assert.h>
#include <string.h>
#include "cluige.h"
//#include "Node.h" //already in cluige.h


////////////////////////////////// _Node /////////

// no : would encourage not freeable nodes on the stack
//static void nde_initZero(Node* node)
//{
//    ...
//}

static void nde_deleteNode(Node* node)
{
    if(node->children != NULL)
    {
        node->children->deleteNode(node->children);
        node->children = NULL;
    }
    if(node->nextSibling != NULL)
    {
        node->nextSibling->deleteNode(node->nextSibling);
        node->nextSibling = NULL;
    }
     //beware in C, hardcoded strings like char* s = "bob" are allocated on stack
    if(node->name != NULL)
    {
        free(node->name);
    }
    free(node->_className);
    assert(node->_subClass == NULL);

    if(node->script != NULL)
    {
        node->script->deleteScript(node->script);
    }

    free(node);
}

static void nde_processNode(Node* thisNode)
{
    if((thisNode->script != NULL) && (thisNode->script->process != NULL))
    {
        thisNode->script->process(thisNode->script, iCluige.clock->elapsedSeconds);
    }
}

static void nde_enterTree(Node* thisNode)
{
    //TODO enterTree in Script?
//    if((thisNode->script != NULL) && (thisNode->script->enterTree != NULL))
//    {
//        thisNode->script->enterTree(thisNode->script);
//    }
}


////////////////////////////////// iiNode /////////

static Node* nde_newNode()
{
    Node* node = iCluige.checkedMalloc(sizeof(Node));
    //nde_initZero(node);
    node->parent = NULL;
    node->nextSibling = NULL;
    node->children = NULL;
    node->script = NULL;
    node->name = NULL;
    node->active = true;

    StringBuilder sb;
    node->_className = iCluige.iStringBuilder.stringAlloc(&sb, 4);
    iCluige.iStringBuilder.append(&sb, "Node");

    node->deleteNode = nde_deleteNode;
    node->enterTree = nde_enterTree;
    node->onLoopStarting = NULL;
    node->preProcessNode = NULL;
    node->processNode = nde_processNode;
    node->postProcessNode = NULL;
    node->_subClass = NULL;
    return node;
}

static int nde_getIndex(const Node* node)
{
    int res = 0;
    if(node->parent != NULL)
    {
        Node* siblingI = node->parent->children;
        while(siblingI != node)
        {
            siblingI = siblingI->nextSibling;
            res++;
        }
    }
    return res;
}

static int nde_getDepth(const Node* node)
{
    int res = 0;
    Node* parent = node->parent;
    while(parent != NULL)
    {
        parent = parent->parent;
        res++;
    }
    return res;
}

static void nde_setName(Node* n, const char* newName)
{
    //allocate new name to prevent pointing to the stack
    int size = strlen(newName);
    char* nextName = iCluige.checkedMalloc((size + 1) * sizeof(char));
    strcpy(nextName, newName);
    //free old name
    if(n->name != NULL)
    {
        free(n->name);
    }
    n->name = nextName;
}

//make unique name
static void nde_autoName(Node* node)
{
    int b2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;
    int p2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER;
    size_t nameSizeMax = b2d + p2d + 1;
    if(node->parent != NULL)
    {
        nameSizeMax += 1 + strlen(node->parent->name);
    }

    StringBuilder sb;
    node->name = iCluige.iStringBuilder.stringAlloc(&sb, nameSizeMax);
    iCluige.iStringBuilder.append(&sb, "%p-%d", node, nde_getIndex(node));

    if(node->parent != NULL)
    {
        iCluige.iStringBuilder.append(&sb, "<%s", node->parent->name);
    }
}

//asserts that wanted child doesn't have already a parent
static void nde_addChild(Node* parent, Node* child)
{
    assert(child->parent == NULL);
    assert(child != parent);
    if(parent->children == NULL)
    {
        parent->children = child;
    }
    else
    {
        Node* lastChild = parent->children;
        while(lastChild->nextSibling != NULL)
        {
            lastChild = lastChild->nextSibling;
        }
        lastChild->nextSibling = child;
    }
    child->parent = parent;

    if(child->name == NULL)
    {
        nde_autoName(child);
    }

    //if script, call ready (not yet enterTree())
    if(!child->_alreadyEnteredTree)
    {
        child->_alreadyEnteredTree = true;
        if((child->script != NULL) && (child->script->ready != NULL))
        {
            child->script->ready(child->script);
        }
    }
    if(child->enterTree != NULL)
    {
        child->enterTree(child);
    }
}

static void nde_printTreePretty(const Node* node)
{
    static int depth = 0;

    for(int i=0; i<depth; i++)
    {
        printf("   ");
    }
    printf("\\_%s (%s)\n", node->name, node->_className);

    if(node->children != NULL)
    {
        depth++;
        nde_printTreePretty(node->children);
        depth--;
    }
    if(node->nextSibling != NULL)
    {
        nde_printTreePretty(node->nextSibling);
    }
}

/////////////////////////////////// Node //////////

void iiNodeInit()
{
    //iCluige.iNode.initZero = nde_initZero;
    iCluige.iNode.newNode = nde_newNode;
    //iCluige.iNode.deleteNode = nde_deleteNode;
    iCluige.iNode.getIndex = nde_getIndex;
    iCluige.iNode.getDepth = nde_getDepth;
    iCluige.iNode.setName = nde_setName;
    iCluige.iNode.addChild = nde_addChild;
    iCluige.iNode.printTreePretty = nde_printTreePretty;
}

