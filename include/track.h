#ifndef _TRACK_H
#define _TRACK_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <list>
#include <memory>
#include <vector>

#include "model_joe03.h"
#include "texture.h"
#include "joepack.h"
#include "scenegraph.h"
#include "tracksurface.h"
#include "mathvector.h"
#include "quaternion.h"
#include "bezier.h"
#include "aabb.h"
#include "aabb_space_partitioning.h"
#include "k1999.h"
#include "optional.h"

#include "track_object.h"
#include "roadstrip.h"

class OBJECTLOADER;

class TRACK
{
public:
	TRACK(std::ostream & info, std::ostream & error);
	~TRACK();
	
	void Clear();
	
	///returns true if successful.  loads the entire track with this one function call.
	bool Load(
		const std::string & trackpath,
		const std::string & effects_texturepath,
		SCENENODE & sceneroot,
		bool reverse,
		int anisotropy,
		const std::string & texsize);
	
	///returns true if successful.  only begins loading the track; the track won't be loaded until more calls to ContinueDeferredLoad().  use Loaded() to see if loading is complete yet.
	bool DeferredLoad(
		const std::string & trackpath,
		const std::string & effects_texturepath,
		SCENENODE & sceneroot,
		bool reverse,
		int anisotropy,
		const std::string & texsize,
		bool shadows,
		bool doagressivecombining);
	
	bool ContinueDeferredLoad();
	
	int DeferredLoadTotalObjects();

	std::pair <MATHVECTOR <float, 3>, QUATERNION <float> > GetStart(unsigned int index);
	
	int GetNumStartPositions()
	{
		return start_positions.size();
	}
	
	bool Loaded() const
	{
		return loaded;
	}
	
	bool CastRay(
		const MATHVECTOR <float, 3> & origin,
		const MATHVECTOR <float, 3> & direction,
		float seglen, MATHVECTOR <float, 3> & outtri,
		const BEZIER * & colpatch,
		MATHVECTOR <float, 3> & normal) const;
	
	const std::list <ROADSTRIP> & GetRoadList() const
	{
		return roads;
	}
	
	unsigned int GetSectors() const
	{
		return lapsequence.size();
	}
	
	const BEZIER * GetLapSequence(unsigned int sector)
	{
		assert (sector < lapsequence.size());
		return lapsequence[sector];
	}
	
	void SetRacingLineVisibility(bool newvis)
	{
		if(racingline_node)
			racingline_node->SetChildVisibility(newvis);
	}
	
	void Unload()
	{
		racingline_node = NULL;
		Clear();
	}
	
	bool IsReversed() const
	{
		return direction == DIRECTION_REVERSE;
	}

	const TRACKSURFACE * GetRoadSurface() const
	{
		return &surface;
	}
	
	const std::list<TRACK_OBJECT> & GetTrackObjects()
	{
		return objects;
	}

private:
	std::ostream & info_output;
	std::ostream & error_output;

	std::string texture_size;
	std::map <std::string, MODEL_JOE03> model_library;
	std::map <std::string, TEXTURE_GL> texture_library;
	std::list <TRACK_OBJECT> objects;
	bool vertical_tracking_skyboxes;
	std::vector <std::pair <MATHVECTOR <float, 3>, QUATERNION <float> > > start_positions;

	// does the track use Surface types (surfaces.txt)
	bool usesurfaces;
	
	// using list to avoid reallocation
	std::list <TRACKSURFACE> tracksurfaces;
	
	// default road surface (asphalt)
	TRACKSURFACE surface;
	
	enum
	{
		DIRECTION_FORWARD,
		DIRECTION_REVERSE
	} direction;
	
	//road information
	std::list <ROADSTRIP> roads;
	
	//the sequence of beziers that a car needs to hit to do a lap
	std::vector <const BEZIER *> lapsequence;
	
	//racing line data
	SCENENODE * racingline_node;
	TEXTURE_GL racingline_texture;
	
	bool CreateRacingLines(
		SCENENODE * parentnode, 
		const std::string & texturepath,
		const std::string & texsize);
	
	bool LoadParameters(const std::string & trackpath);
	
	bool LoadSurfaces(const std::string & trackpath);
	
	bool LoadObjects(
		const std::string & trackpath,
		SCENENODE & sceneroot,
		int anisotropy);
	
	std::auto_ptr <OBJECTLOADER> objload;
	
	///returns false on error
	bool BeginObjectLoad(
		const std::string & trackpath,
		SCENENODE & sceneroot,
		int anisotropy,
		bool dynamicshadowsenabled,
		bool doagressivecombining);
	
	///returns a pair of bools: the first bool is true if there was an error, the second bool is true if an object was loaded
	std::pair <bool, bool> ContinueObjectLoad();
	
	bool LoadRoads(const std::string & trackpath, bool reverse);
	
	bool LoadLapSequence(const std::string & trackpath, bool reverse);
	
	void ClearRoads() {roads.clear();}
	
	void Reverse();
	
	///read from the file stream and put it in "output".
	/// return true if the get was successful, else false
	template <typename T>
	bool GetParam(std::ifstream & f, T & output)
	{
		if (!f.good())
			return false;
		
		std::string instr;
		f >> instr;
		if (instr.empty())
			return false;
		
		while (!instr.empty() && instr[0] == '#' && f.good())
		{
			f.ignore(1024, '\n');
			f >> instr;
		}
		
		if (!f.good() && !instr.empty() && instr[0] == '#')
			return false;
		
		std::stringstream sstr(instr);
		sstr >> output;
		return true;
	}
	
	bool loaded;
	bool cull;
};

#endif
