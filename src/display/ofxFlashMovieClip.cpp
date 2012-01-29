/*
 *  ofxFlashMovieClip.cpp
 *  emptyExample
 *
 *  Created by lukasz karluk on 1/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxFlashMovieClip.h"

ofxFlashMovieClip :: ofxFlashMovieClip( )
{
	typeID = OFX_FLASH_TYPE_MOVIECLIP;
	createNewLayer();
	layer = timeline.layers[0];
	setTotalFrames(0,1);
	frameIndex = 0;
	resetTimeline = false;
	firstRun = true;
	lastFrameIndex = -1;
	play();
}

ofxFlashMovieClip :: ~ofxFlashMovieClip()
{

    for( int k = 0; k < timeline.layers.size(); k++)
	{
		removeFrameChildren(k);
		for( int i=0; i< timeline.layers[k]->frames.size(); i++ )
			delete timeline.layers[k]->frames[ i ];
		
		timeline.layers[k]->frames.clear();
		
	}
	//frame = NULL;
}

///////////////////////////////////////////////
//	SETUP - INTERNAL.
///////////////////////////////////////////////
/*
void ofxFlashMovieClip :: addDOMFrameToTimeline(DOMFrame *domFrame)
{
	timelineDOMFrames.push_back(domFrame);
}
*/

void ofxFlashMovieClip :: setTotalFrames ( int layerIndex, int total )
{
	int t1	= total;
	int t2	= timeline.layers[layerIndex]->frames.size();
	int t	= MAX( t1, t2 );
	
	for( int i=0; i<t; i++ )
	{
		if( i > t2 - 1 )			// number of frames has increased, add more.
		{
			timeline.layers[layerIndex]->frames.push_back( new ofxFlashDisplayObjectContainer() );
		}
		else if( i > t1 - 1 )		// number of frames has decreased, remove some.
		{
			ofxFlashDisplayObjectContainer* frameToRemove;
			frameToRemove = timeline.layers[layerIndex]->frames[ i ];
			
			timeline.layers[layerIndex]->frames.erase( timeline.layers[layerIndex]->frames.begin() + i );
			
			--i;
			--t;
			
			delete frameToRemove;
			frameToRemove = NULL;
		}
	}
	
	frameIndex	= 0;
	//this->frame	= timeline.layers[layerIndex]->frames[ frameIndex ];
}

///////////////////////////////////////////////
//	INTERNAL.
///////////////////////////////////////////////


int ofxFlashMovieClip :: createNewLayer (){
	layerStruct * layer = new layerStruct();
	timeline.layers.push_back(layer);
	ofxFlashDisplayObjectContainer * frame = new ofxFlashDisplayObjectContainer();		// create a single frame.
	layer->frames.push_back( frame );							// add first frame to frames.
	lastTreatedObj.push_back(frame);
	return timeline.layers.size() -1;
	
}

void ofxFlashMovieClip :: updateOnFrame ()
{
	
	//	// cout << "n: <" <<libraryItemName() << "> " ;
	//	// cout <<  "c: <" << children.size() << ">  |  "; 
	
	/*
	 resettimeline helps us add new mc's dynamically. but this system is broken: 
	 - adding a new mc should not affect other layers
	 - what happens if we add something witht he same name? unload the first. avoid dups. 
	 - in general this func also moves the timeline forward, and that's wrong. we should have the core of thie func elsewhere so it can be called without worrieng about moving the timeline frameindex
	 
	 
	 */
	if(frameIndex != lastFrameIndex || firstRun || resetTimeline)
	{
		
		//// cout << frameIndex << " " << lastFrameIndex << endl;
		//	cout <<  timeline.layers.size() << endl;
		for(int l = 1; l < timeline.layers.size() ; l++)
			//for(int l = timeline.layers.size() ; l >0; l--)
		{
			readTimelineIntoThis(l);
		}				
		readTimelineIntoThis(0);
		resetTimeline = false;
		
		
	}
	lastFrameIndex = frameIndex;	
	
	if( bPlay )
	{
		nextFrame();
	}
	firstRun = false; // make this per layer!!!!!
	
}


void ofxFlashMovieClip :: readTimelineIntoThis(int layerIndex) 
{
	int l = layerIndex;
	if(timeline.layers[l]->frames.size() > frameIndex)
	{
		ofxFlashDisplayObjectContainer * frame  = timeline.layers[l]->frames[frameIndex];	
		
		//	// cout << frame->_offsetFromKeyframe  << " " << isThisFrameContinual(lastTreatedObj[l], frame) << " | " ;
		if(frame->_offsetFromKeyframe == 0)
		{
			bool b = isThisFrameContinual(lastTreatedObj[l], frame);
			if (!b || firstRun == true || resetTimeline == true)
			{
				// cout << "firstRun: " << name() << endl;
				removeFrameChildren(l);
				//cout << l << endl;
				addFrameChildren(l);
				
			} else if (b == true && l != 0) {
				
				ofxFlashDisplayObject * targetChild = frame->children[0];
				ofxFlashDisplayObject * targetOnStage = ((ofxFlashMovieClip*)(this)->getChildByName(targetChild->name()));
				targetOnStage->matrix( targetChild->matrix() ); // NO GOOD! WE're erasing the original matrix! OOOOOF! Create a vec inside objects and build a tweening system. have the xflbuilder create a tween in the first object instead of copying it over multiple time. then play the tween here (will also save the 'is this a continual frame' issue)
				if( timeline.layers[l]->frames[frameIndex]->_offsetFromKeyframe == 0) targetOnStage->matrix( targetChild->originalMatrix() ); 
				/*
				 OK OK, here's what we're doing next: 
				 xflbuilder learns about continual frames and tells the container who's the keyframe. 
				 we might even store a vec of the matrices in the array on that container, which will also be helpful for tweening.
				 
				 and oh - indexing is also a mess. adding things manually is a hack (see the h file for addChild). 
				 
				 
				 */
				targetOnStage->_layerIndex = l;
				if (l != 0) setChildIndex(targetOnStage, l - 1);
				cout << "frame: " << frameIndex << " name: " << targetOnStage->name() << " " << targetOnStage->_layerIndex << endl;
			}
		}
		lastTreatedObj[l] = timeline.layers[l]->frames[frameIndex];
	}
}

bool ofxFlashMovieClip :: isThisFrameContinual(ofxFlashDisplayObjectContainer * lastFrame, ofxFlashDisplayObjectContainer * thisFrame) 
{
	if (lastFrame->children.size() == thisFrame->children.size() && thisFrame->children.size() == 1)
	{
		//// cout << lastFrame->children.size()  << " " << endl; 
		ofxFlashDisplayObject *lastchild = lastFrame->children[0];
		ofxFlashDisplayObject *thischild = thisFrame->children[0];		
		
		// !! add condition for non-MC, just don't check lib name if they don't have one
		if(thischild->typeID == OFX_FLASH_TYPE_MOVIECLIP && lastchild->libraryItemName() == thischild->libraryItemName() 
		   && lastchild->name() == thischild->name()  )
		{
			// cout << lastchild->libraryItemName() << endl;
			return true;
		}
		 
	}
	
	return false;
}

///////////////////////////////////////////////
//	FRAME CHILDREN.
///////////////////////////////////////////////

void ofxFlashMovieClip :: addFrameChildren (int layerIndex)
{
	
	// cout << "ADDFRAMECHILDREN " <<  name() << endl;
	ofxFlashDisplayObjectContainer * frame  = timeline.layers[layerIndex]->frames[frameIndex];
	for( int i=0; i< frame->children.size(); i++ )
	{
		
		
        ofxFlashDisplayObject *child;
        child = frame->children[ i ];
		// the child must be added manually, instead of using the addChild() method.
		// doing it this way ensures the child is not removed from the frames.
		// a movieclip can technically be added to only one parent, 
		// and as soon it is added to another it is removed from the previous parent.
		// with movieclip frames this is an exception and therefore movieclips have to be added like the below.
		
		children.push_back( child );
		child->stage	= this->stage;
		child->parent	= this;
		child->level( this->level() + 1 );
		child->_layerIndex = layerIndex;
		if (layerIndex != 0) setChildIndex(child, layerIndex - 1);
	//	cout << child->name() << " " << child->_layerIndex << " "  << layerIndex << endl;
		child->updateOnFrame();
		if(child->typeID == OFX_FLASH_TYPE_MOVIECLIP) 
		{
			((ofxFlashMovieClip *)child)->gotoAndPlay(1);
			//	// cout <<"CHILD ADDED: " << ((ofxFlashMovieClip *)child)->name() <<  " " << 			child->libraryItemName() <<" in " << this->name() << endl;		
		}
		// cout << "CHILD ADDED: " << ((ofxFlashMovieClip *)child)->name() <<  " " << 			child->libraryItemName() <<" in " << this->name() << endl;	
	}
}

void ofxFlashMovieClip :: removeFrameChildren (int layerIndex)
{
	ofxFlashDisplayObjectContainer :: removeAllChildren(layerIndex);
}

///////////////////////////////////////////////
//	MOVIE CLIP METHODS.
///////////////////////////////////////////////

void ofxFlashMovieClip :: gotoAndPlay ( int frameNum )
{
	int index = frameNum - 1;
	
	if( frameIndex != index )
	{
		frameIndex	= frameNum - 1;
		frameIndex	= ofClamp( frameIndex, 0, totalFrames() - 1 );
		//frame		= frames[ frameIndex ];
	}
	
	play();
}

void ofxFlashMovieClip :: gotoAndStop ( int frameNum )
{
	int index = frameNum - 1;
	
	if( frameIndex != index )
	{
		frameIndex	= index;
		frameIndex	= ofClamp( frameIndex, 0, totalFrames() - 1 );
		//frame		= frames[ frameIndex ];
	}
	
	stop();
}

void ofxFlashMovieClip :: nextFrame ()
{
	if( ++frameIndex > totalFrames() - 1 )
	{
		frameIndex = 0;
	}
	
	//frame = frames[ frameIndex ];
}

void ofxFlashMovieClip :: prevFrame ()
{
	if( --frameIndex < 0 )
	{
		frameIndex = totalFrames() - 1;
	}
	
//	frame = frames[ frameIndex ];
}

void ofxFlashMovieClip :: play ()
{
	bPlay = true;
}

void ofxFlashMovieClip :: stop ()
{
	bPlay = false;
}

///////////////////////////////////////////////
//	GETTERS.
///////////////////////////////////////////////

int ofxFlashMovieClip :: totalFrames ()
{
	int maxFrames = 0;
	for (int i = 0; i < timeline.layers.size(); i++) {
		int f = timeline.layers[i]->frames.size();
		if (f > maxFrames) maxFrames = f;
	}
	return maxFrames;
}

int ofxFlashMovieClip :: currentFrame ()
{
	return frameIndex + 1;
}