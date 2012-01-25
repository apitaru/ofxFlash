/*
 *  ofxFlashXFLBuilder.h
 *  emptyExample
 *
 *  Created by lukasz karluk on 5/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxFlashLibrary.h"
#include "ofxFlashMovieClip.h"
#include "ofxFlashBitmap.h"
#include "ofxFlashShape.h"
#include "ofxXmlSettings.h"



//====================================================
#include "ofxFlashDomStructs.h";
#define DOM_DOCUMENT_TYPE		"DOMDocument"
#define DOM_SYMBOL_ITEM_TYPE	"DOMSymbolItem"
//====================================================

class ofxFlashXFLBuilder : public ofxXmlSettings
{

public:
	
	 ofxFlashXFLBuilder();
	~ofxFlashXFLBuilder();
	
    void setVerbose( bool value ) { bVerbose = value; }
    
	void build ( const string& xflRoot, const string& xflFile, ofxFlashDisplayObjectContainer* container );
	
private:
	
    bool bVerbose;
    
	ofxFlashDisplayObjectContainer* container;
    string	xflRoot;
	string	xflFile;
	string	domType;
	int		totalFrames;
	int activeLayerIndex;
	
	DOMTimeline			domTimeline;
	DOMLayer			domLayer;
	DOMFrame			domFrame;
	DOMSymbolInstance	domSymbolInstance;
	DOMBitmapInstance	domBitmapInstance;
	DOMRectangleObject	domRectangleObject;
	DOMOvalObject		domOvalObject;
	
	void countTotalFrames		();
	void buildTimelines			();
	void buildLayers			();
	void buildFrames			();
	void buildElements			();
	void buildBitmap			();
	void buildMovieClip			();
	void buildRectangleShape	();
	void buildOvalShape			();
	
	void addDisplayObjectToFrames		( ofxFlashDisplayObject* displayObject );
	void setupMatrixForDisplayObject	( ofxFlashDisplayObject* displayObject );
	void setupColorForDisplayObject		( ofxFlashDisplayObject* displayObject );
	void setupFillForShape				( ofxFlashShape* shape );
	void setupStrokeForShape			( ofxFlashShape* shape );
	
	void addTimelineDataToFrames ();
	
	void pushTagAt			( int i );
	
	string cleanHexString	( string value );
	int  stringToHex		( string value );
	
};