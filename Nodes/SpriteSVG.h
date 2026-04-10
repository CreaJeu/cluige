#ifndef SPRITE_SVG_H_INCLUDED
#define SPRITE_SVG_H_INCLUDED

#include "../Vector2.h"
#include "../Deque.h"
#include "../SVGParser.h"

typedef struct _SpriteSVG SpriteSVG;

//relation between pixels, length units, sprite dimensions...

//summary :
//  in inkscape, the UI "Document properties / Display / Front page" only sets page limits
//  godot uses these page limits to set its texture size, using its own scale 1px svg = 1px godot
//  godot rasterizes paths so that their apparent size relative to inkscape page = apparent size relative to godot texture
//  for cluige-godot, set inkscape page limits in px to wanted size in cluige characters, and page scale to 1

//paths inside .svg file :
//  <path ... H 49.3 V 42 ...>
//   no unit is specified

//page/sprite dimension
//  inkscape :
//	Document properties / Display / Front page / Width, Height (in chosen unit)
//	(changing these Width-Height-Unit in UI doesn't modify numbers already inside paths, only page limits)
//	(those page limits impact the apparent scale in godot, see below)
//  svg file :
//	<svg width="30cm" height="30cm" ...>
//	or without nammed unit (meaning 'px' in inkscape) :
//	<svg width="30" height="30" ...>
//  godot :
//	page limits => godot texture dimensions (scaling : see below)
//	scales paths so that :
//	  relative size to godot texture = relative size to inkscape page limits
//	reads values and units from these .svg attributes just above (using its own scaling, see below)
//  cluige :
//	we want 1 px in godot texture = 1 cluige character

//scaling 'length unit' - pixel
//  inkscape :
//	Document properties / Display / Front page / scale ("cm per user unit")
//	the "page" limits are set relatively to this "scale" (changing this scale in UI doesn't modify numbers already inside paths, only page limits)
//	(these page limits impact the apparent scale in godot, see above)
//  svg file :
//	<svg ... viewBox="0 0 300 300" ...>
//	equivalent info (just said differently) than inkscape scale ("cm per user unit")
//  godot :
//	sets whole texture size from page limits
//	uses its own pixel/length convention for this texture size, ignoring viewBox/inkscape
//	1 mm .svg page height/width   =>  3.78 px godot texture height/width
//	1 cm .svg page height/width   => 37.8 px godot texture height/width
//	1 inch .svg page height/width => 96 px godot texture height/width
//	1 pt .svg page height/width   =>  1.3333 px godot texture height/width
//	1 pc .svg page height/width   => 16 px godot texture height/width
//	1 px .svg page height/width   =>  1 px godot texture height/width
//	scales paths so their relative size to svg page = relative size to godot texture (see above)

//conclusion :
//for godot-cluige, adviced inkscape workflow :
//  first thing in inkscape : open the UI "Document properties / Display" and set :
//	units : px
//	scale ("px per user unit") : 1
//	Height/Width : size in px = size in cluige characters of your reference game screen
//	set page background color like your game clear color
//  now the main view background of inkscape represents your reference game screen
//  draw your paths at the size you want them relative to this screen
//  when (nearly) finished (scale tested ok in the game), in the UI "Document properties / Display" :
//	click the button "Resize to content"
//	re-center your drawing within the page (or any alignment you need)
//	  beware : don't select "layers" for re-alignment, only paths
//  when finally saved, default godot import => wanted default size in game (godot and cluige)


struct _SpriteSVG
{

	//size attribute of the top <svg> tag in .svg
	//  <svg width="30cm" height="30cm" ...>
	//  or without nammed unit (meaning 'px' in inkscape) :
	//  <svg width="30" height="30" ...>
	//in inkscape =
	//  Document properties / Display / Front page / Width, Height
	//in godot, defines the dimensions of the sprite/texture in px
	//in cluige, defines the dimensions of the sprite in characters
	Vector2 size_svg;

	//godot default : true
	bool centered;
	Vector2 _centered_offest;

	//top left corner of svg will be drawn at
	//Node2D.position - offset
	//godot default : (0,0)
	Vector2 offset;

	Vector2 scale;//tmp before general Node2D scale

	Deque paths;//Deque<Path2D* > all paths of the svg

	//private
//	struct
//	{
//		bool state_changed;
//	} _state_changes;//see old_baked/new_baked in Node2D for example if needed

	//private
	Node2D* _this_Node2D;
	void* _sub_class;

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);
//	void (*_erase_super)(Node*);
	void (*_draw_super)(Node*);

	//virtual methods in .c :
	//void erase_Node(Node*);
	//void draw(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSpriteSVG
{
	// ~private static
	NodeFactory _SpriteSVG_factory;

	//too specific to be in iCluige
	struct iiSVGParser iSVGParser;

	//SpriteSVG* new_SpriteSVG()
	SpriteSVG* (*new_SpriteSVG)();
	SpriteSVG* (*new_SpriteSVG_from_Node2D)();

	//copies the points from the array
	void (*add_path_from_array)(SpriteSVG*, Vector2*, int nb_points);
	//copies the points from the array
	void (*add_path_from_array_relative)(SpriteSVG*, Vector2*, int nb_points);
	//copies the points from the deque [x y x y x y...]
	void (*add_path_from_parsed_deque)(SpriteSVG*, Deque*);

	void (*parse_file)(SpriteSVG*, char* file_path);
};
//iSpriteSVG : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiSpriteSVG_init();


#endif // SPRITE_SVG_H_INCLUDED
