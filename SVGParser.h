#ifndef SVGPARSER_H_INCLUDED
#define SVGPARSER_H_INCLUDED

#include "Deque.h"

//typedef struct _SVGParser SVGParser;

struct _SVGParser
{
    //bool file_finished;

    //Deque<float> [x y x y x y x y x y...]
    Deque coordinates_sequence;

    //TODO
    //bool _visible;
    //color, thickness...
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSVGParser
{
	void (*SVGParser_alloc)(struct _SVGParser* this_SVGParser);
	//void (*pre_delete_SVGParser)(struct _SVGParser* this_SVGParser);

	bool (*prepare_parsing)(struct _SVGParser* this_SVGParser, char* file_path);
    bool (*parse_path)(struct _SVGParser* this_SVGParser);
    void (*end_parsing)(struct _SVGParser* this_SVGParser);
};
//iSVGParser : in iiSpriteSVG

//to be called only by SpriteSVG_init() to set functions pointers
void iiSVGParser_init();


#endif // SVGPARSER_H_INCLUDED
