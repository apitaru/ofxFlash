/*
 *  ofxFlashMovieClip.h
 *  emptyExample
 *
 *  Created by lukasz karluk on 1/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofxFlashSprite.h"
#include "ofxFlashDomStructs.h"

/** 
 *  Forward Declarations to prevent Cyclic Dependency.
 *  http://www.eventhelix.com/RealtimeMantra/HeaderFileIncludePatterns.htm
**/
class ofxFlashXFLBuilder;
class ofxFlashLibrary;



struct layerStruct{
	vector<ofxFlashDisplayObjectContainer*>	frames;
};
struct timeLineStruct {
	vector <layerStruct *> layers;
};

class ofxFlashMovieClip : public ofxFlashSprite
{
	
public:
	
	 ofxFlashMovieClip();
	~ofxFlashMovieClip();
    
    friend class ofxFlashXFLBuilder;    // friends! http://www.cplusplus.com/doc/tutorial/inheritance/
    friend class ofxFlashLibrary;
	
	virtual void setup	() {};
	virtual void update	() {};
	virtual void draw	() {};
    
    //----------------------------------------- movie clip methods.
	void gotoAndPlay	( int frameNum );
	void gotoAndStop	( int frameNum );
	void nextFrame		();
	void prevFrame		();
	void play			();
	void stop			();
	
	int totalFrames		();
	int currentFrame	();
    
    //------------------------------------------------------------------- display object container override.
    ofxFlashDisplayObject* addChild ( ofxFlashDisplayObject* child, int layerIndex = 0) {
		resetTimeline = true;
        return timeline.layers[layerIndex]->frames[frameIndex]->addChild( child );
    }
	
    void addTimelineDataToChild ( int offsetFromKeyframe, int layerIndex ) {
		timeline.layers[layerIndex]->frames[frameIndex]->addTimelineData( offsetFromKeyframe);
    }
	
    
    ofxFlashDisplayObject* addChildAt ( ofxFlashDisplayObject* child, int index, int layerIndex = 0 ) {
        return timeline.layers[layerIndex]->frames[frameIndex]->addChildAt( child, index );
    }
    
    bool contains ( ofxFlashDisplayObject* child ) {
		for(int i = 0; i < timeline.layers.size() -1; i++)
		{
			if (  timeline.layers[i]->frames[frameIndex]->contains( child )) {
				return true;
			}
		}
		return false;
    }
    
    ofxFlashDisplayObject* getChildAt ( int index ) {
		return ofxFlashDisplayObjectContainer :: getChildAt( index );
    }
    
    ofxFlashDisplayObject* getChildByName ( string name ) {
        return ofxFlashDisplayObjectContainer :: getChildByName( name );
    }
    
    int	getChildIndex ( ofxFlashDisplayObject* child ) {
        return ofxFlashDisplayObjectContainer :: getChildIndex( child );
    }
    
    vector<ofxFlashDisplayObject*> getObjectsUnderPoint ( ofPoint point ) {
        return ofxFlashDisplayObjectContainer :: getObjectsUnderPoint( point );
    }
    
    ofxFlashDisplayObject* removeChild ( ofxFlashDisplayObject* child ) {
        return ofxFlashDisplayObjectContainer :: removeChild( child );
    }
    
    ofxFlashDisplayObject* removeChildAt ( int index ) {
        return ofxFlashDisplayObjectContainer :: removeChildAt( index );
    }
    
    void setChildIndex ( ofxFlashDisplayObject* child, int index ) {
       ofxFlashDisplayObjectContainer :: setChildIndex( child, index );
    }
    
    void swapChildren ( ofxFlashDisplayObject* child1, ofxFlashDisplayObject* child2 ) {
       ofxFlashDisplayObjectContainer :: swapChildren( child1, child2 );
    }
    
    void swapChildrenAt	( int index1, int index2 ) {
       ofxFlashDisplayObjectContainer :: swapChildrenAt( index1, index2 );
    }
	//void addDOMFrameToTimeline(DOMFrame * domFrame);
	bool isThisFrameContinual(ofxFlashDisplayObjectContainer * lastFrame, ofxFlashDisplayObjectContainer * thisFrame);
	
	int createNewLayer();
	virtual void updateOnFrame	();

	void readTimelineIntoThis(int layerIndex);
protected:
	
	
private:
    
	void setTotalFrames	( int layerIndex, int totalFrames );
	
	void addFrameChildren		(int layerIndex = 0);
	void removeFrameChildren	(int layerIndex = 0);
	
	//vector<DOMFrame *> timelineDOMFrames;
	
	timeLineStruct timeline;	
	layerStruct * layer;
	//ofxFlashDisplayObjectContainer*			frame;
	vector <ofxFlashDisplayObjectContainer *> lastTreatedObj;
	
	int		frameIndex;
	bool	bPlay;
	bool    resetTimeline;
	bool firstRun;
	int lastFrameIndex;
	
};