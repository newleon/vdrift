#ifndef _WIDGET_IMAGE_H
#define _WIDGET_IMAGE_H

#include "widget.h"
#include "mathvector.h"
#include "scenegraph.h"
#include "vertexarray.h"

#include <string>
#include <cassert>

class TEXTURE_GL;

class WIDGET_IMAGE : public WIDGET
{
private:
	MATHVECTOR <float, 2> corner1;
	MATHVECTOR <float, 2> corner2;
	VERTEXARRAY varray;
	DRAWABLE * draw;
	
public:
	WIDGET_IMAGE() : draw(NULL) {}
	virtual WIDGET * clone() const {return new WIDGET_IMAGE(*this);};
	
	void SetupDrawable(SCENENODE * scene, TEXTURE_GL * teximage, float x, float y, float w, float h, int order=0, bool button_mode=false, float screenhwratio=1.0)
	{
		MATHVECTOR <float, 2> dim;
		dim.Set(w,h);
		MATHVECTOR <float, 2> center;
		center.Set(x,y);
		corner1 = center - dim*0.5;
		corner2 = center + dim*0.5;
		
		assert(scene);
		draw = &scene->AddDrawable();
		draw->SetDiffuseMap(teximage);
		draw->SetVertArray(&varray);
		draw->SetLit(false);
		draw->Set2D(true);
		draw->SetCull(false, false);
		draw->SetColor(1,1,1,1);
		draw->SetDrawOrder(order+100);
		draw->SetPartialTransparency(true);
		
		if (button_mode)
			varray.SetTo2DButton(x, y, w, h, h/(screenhwratio*3.0));
			//varray.SetTo2DButton(x, y, w, h, h*0.25);
		else
			varray.SetToBillboard(corner1[0], corner1[1], corner2[0], corner2[1]);
	}
	
	virtual void SetAlpha(float newalpha)
	{
		if (draw)
			draw->SetColor(1,1,1,newalpha);
	}
	
	virtual void SetVisible(bool newvis)
	{
		if (draw)
			draw->SetDrawEnable(newvis);
	}
	
	const MATHVECTOR <float, 2> & GetCorner1() const {return corner1;}
	const MATHVECTOR <float, 2> & GetCorner2() const {return corner2;}
};

#endif