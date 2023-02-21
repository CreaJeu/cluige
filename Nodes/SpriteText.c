//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "SpriteText.h"

#include <string.h>
#include <assert.h>
#include <curses.h>
#include <math.h>

////////////////////////////////// _SpriteText /////////

static void sprtx_deleteSpriteText(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteText* thisSpriteText = (SpriteText*)(thisNode2D->_subClass);
    void (*deleteNode2D)(Node*) = thisSpriteText->deleteNode2D;
//    for(int i=0; i<thisSpriteText->nbLines; i++)
//    {
//        free(thisSpriteText->text[i]);
//    }
    free(thisSpriteText->text);
    assert(thisSpriteText->_subClass == NULL);
    free(thisSpriteText);
    thisNode2D->_subClass = NULL;
    deleteNode2D(thisNode);
}

static void sprtx_preProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteText* thisSpriteText = (SpriteText*)(thisNode2D->_subClass);
    //call super()
    thisSpriteText->preProcessNode2D(thisNode);

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
            &(thisSpriteText->offset),
            &orig);
    int flatI = 0;
    for(int line = 0; line < thisSpriteText->nbLines; line++)
    {
        int col = 0;
        //char* lineString = (char*)(thisSpriteText->text[line]);
        //char currChar = lineString[col];
        char currChar = thisSpriteText->text[flatI];
        while(currChar != 0)
        {
            if(currChar != ' ')
            {
                mvaddch(lrintf(orig.y) + line, lrintf(orig.x) + col, ' ');
            }
            col++;
            //currChar = lineString[col];
            flatI++;
            currChar = thisSpriteText->text[flatI];
        }
        flatI++;
    }
}

static void sprtx_postProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteText* thisSpriteText = (SpriteText*)(thisNode2D->_subClass);
    //call super()
    thisSpriteText->postProcessNode2D(thisNode);

    if(!(thisNode2D->visible))
    {
        return;
    }

    //draw new one
    Vector2 orig;
    iCluige.iVector2.substract(
            &(thisNode2D->_tmpGlobalPosition),
            &(thisSpriteText->offset),
            &orig);
    char* lineString = thisSpriteText->text;
    for(int line = 0; line < thisSpriteText->nbLines; line++)
    {
        //mvaddstr(orig.y + line, orig.x, (char*)(thisSpriteText->text[line]));
        mvaddstr(lrintf(orig.y) + line, lrintf(orig.x), lineString);
        lineString += strlen(lineString) + 1;
    }
}


////////////////////////////////// iiSpriteText /////////

static SpriteText* sprtx_newSpriteText()
{
	Node2D* newNode2D = iCluige.iNode2D.newNode2D();
	Node* newNode = newNode2D->_thisNode;
    SpriteText* newSpriteText = iCluige.checkedMalloc(sizeof(SpriteText));

    newSpriteText->offset = (Vector2) { 0., 0. };
    newSpriteText->text = NULL;//the ASCII art / unicode art
    newSpriteText->nbLines = 0;
	newSpriteText->_thisNode2D = newNode2D;
	newSpriteText->_subClass = NULL;
	newSpriteText->deleteNode2D = newNode->deleteNode;
	newSpriteText->preProcessNode2D = newNode->preProcessNode;
	newSpriteText->postProcessNode2D = newNode->postProcessNode;

    newNode2D->_subClass = newSpriteText;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(
            &sb, strlen("NodeNode2DSpriteText"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteText");
    newNode2D->_subClass = newSpriteText;

    newNode->deleteNode = sprtx_deleteSpriteText;
    newNode->preProcessNode = sprtx_preProcessNode;
    newNode->postProcessNode = sprtx_postProcessNode;

    return newSpriteText;
}

static void sprtx_setText(SpriteText* thisSpriteText, const char* newText)
{
    if(thisSpriteText->text != NULL)
    {
//        for(int i=0; i<thisSpriteText->nbLines; i++)
//        {
//            free(thisSpriteText->text[i]);
//        }
        free(thisSpriteText->text);
    }
//    thisSpriteText->text =
//        iCluige.iStringBuilder.split(newText, "\n", &(thisSpriteText->nbLines));
    int totalLength = strlen(newText);
    thisSpriteText->nbLines = 0;
    StringBuilder sb;
    thisSpriteText->text = iCluige.iStringBuilder.stringAlloc(&sb, strlen(newText));
    iCluige.iStringBuilder.append(&sb, newText);
    for(int i=0; i<totalLength; i++)
    {
        if(newText[i] == '\n')
        {
            thisSpriteText->text[i] = '\0';
            thisSpriteText->nbLines++;
        }
    }
    if(totalLength > 0)
    {
        thisSpriteText->nbLines++;
    }
}

/////////////////////////////////// Node //////////

void iiSpriteTextInit()
{
    iCluige.iSpriteText.newSpriteText = sprtx_newSpriteText;
    iCluige.iSpriteText.setText = sprtx_setText;
}

