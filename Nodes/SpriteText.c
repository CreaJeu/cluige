//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "SpriteText.h"

#include <string.h>
#include <assert.h>

////////////////////////////////// _SpriteText /////////

static void sprtx_deleteSpriteText(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteText* thisSpriteText = (SpriteText*)(thisNode2D->_subClass);
    void (*deleteNode2D)(Node*) = thisSpriteText->deleteNode2D;
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
    //TODO call super()
    //TODO clear old one (unless immobile? => no,
    //because other masking things could have moved and
    //made this sprite visible again)
}

static void sprtx_postProcessNode(Node* thisNode)
{
    Node2D* thisNode2D = (Node2D*)(thisNode->_subClass);
    SpriteText* thisSpriteText = (SpriteText*)(thisNode2D->_subClass);
    //TODO call super()
    //TODO draw new one
}


////////////////////////////////// iiSpriteText /////////

static SpriteText* sprtx_newSpriteText()
{
	Node2D* newNode2D = iCluige.iNode2D.newNode2D();
	Node* newNode = newNode2D->_thisNode;
    SpriteText* newSpriteText = iCluige.checkedMalloc(sizeof(SpriteText));

    newSpriteText->text = NULL;//the ASCII art / unicode art
	newSpriteText->_thisNode2D = newNode2D;
	newSpriteText->_subClass = NULL;
	newSpriteText->deleteNode2D = newNode->deleteNode;
	newSpriteText->preProcessNode2D = newNode->preProcessNode;
	newSpriteText->postProcessNode2D = newNode->postProcessNode;

    newNode2D->_subClass = newSpriteText;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(
            &sb, strlen("SpriteText"));
    iCluige.iStringBuilder.append(&sb, "SpriteText");
    newNode2D->_subClass = newSpriteText;

    newNode->deleteNode = sprtx_deleteSpriteText;
    newNode->preProcessNode = sprtx_preProcessNode;
    newNode->postProcessNode = sprtx_postProcessNode;

    return newSpriteText;
}

static SpriteText* sprtx_setText(SpriteText* thisSpriteText, const char* newText)
{
    StringBuilder sb;
    int len = strlen(newText);
    if(thisSpriteText->text == NULL)
    {
        thisSpriteText->text = iCluige.iStringBuilder.stringAlloc(&sb, len);
    }
    else
    {
        thisSpriteText->text = realloc(thisSpriteText->text, (len + 1) * sizeof(char));
        iCluige.iStringBuilder.connectExistingString(&sb, thisSpriteText->text);
    }
    iCluige.iStringBuilder.append(&sb, newText);
}

/////////////////////////////////// Node //////////

void iiSpriteTextInit()
{
    iCluige.iSpriteText.newSpriteText = sprtx_newSpriteText;
    iCluige.iSpriteText.setText = sprtx_setText;
}

