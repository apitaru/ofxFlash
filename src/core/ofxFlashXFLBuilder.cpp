/*
 *  ofxFlashXFLBuilder.cpp
 *  emptyExample
 *
 *  Created by lukasz karluk on 5/11/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxFlashXFLBuilder.h"

ofxFlashXFLBuilder :: ofxFlashXFLBuilder()
{
    bVerbose    = false;

    xflRoot     = "";
	xflFile		= "";
	container	= NULL;
	domType		= DOM_DOCUMENT_TYPE;
	totalFrames	= 1;
}

ofxFlashXFLBuilder :: ~ofxFlashXFLBuilder()
{
	//
}

///////////////////////////////////////////
//	BUILD.
///////////////////////////////////////////

void ofxFlashXFLBuilder :: build ( const string& root, const string& file, ofxFlashDisplayObjectContainer* container )
{
    xflRoot = root;
	xflFile	= file;
    string xflFilePath = xflRoot + xflFile;
    
	this->container = container;
	
    bool success = loadFile( xflFilePath );
    
    if( bVerbose )
    {
        if( success )
            cout << "[ ofxFlashXFLBuilder :: build ] - loading movieclip xml - SUCCESS :: " << xflFilePath << endl;
        else
            cout << "[ ofxFlashXFLBuilder :: build ] - loading movieclip xml - FAILED  :: " << xflFilePath << endl;
    }
    
	if( success )
	{
		TiXmlElement* child = ( storedHandle.FirstChild() ).ToElement();
		domType = child->Value();
		
		pushTag( domType, 0 );
		
		if( domType == DOM_DOCUMENT_TYPE )
		{
			pushTag( "timelines", 0 );
		}
		else if( domType == DOM_SYMBOL_ITEM_TYPE )
		{
			pushTag( "timeline", 0 );
		}
		
		countTotalFrames();
		

		
		buildTimelines();
		//cout << "AP Total Frames: " << totalFrames << endl;
		
		popTag();
		popTag();
	}
	// cout << "XXXXXXXXXXXXDONE" << endl;
}

void ofxFlashXFLBuilder :: countTotalFrames ()
{
	pushTag( "DOMTimeline", 0 );
	pushTag( "layers", 0 );
	
	int numOfLayers;
	numOfLayers = getNumTags( "DOMLayer" );
	
	for( int i=0; i<numOfLayers; i++ )
	{
		string layerType;
		layerType = getAttribute( "DOMLayer", "layerType", "", i );
		
		if( layerType == "guide" )		// skip guide layers.
			continue;
		
		pushTag( "DOMLayer", i );
		pushTag( "frames", 0 );
		
		int numOfFrames;
		numOfFrames = getNumTags( "DOMFrame" );
		
		for( int j=0; j<numOfFrames; j++ )
		{
			int frameIndex		= getAttribute( "DOMFrame", "index",	0, j );
			int frameDuration	= getAttribute( "DOMFrame", "duration",	1, j );
			int frameEnd		= frameIndex + frameDuration;
			
			if( frameEnd > totalFrames )
			{
				totalFrames = frameEnd;
			}
		}
		
		popTag();
		popTag();
	}
	
	popTag();
	popTag();
}

void ofxFlashXFLBuilder :: buildTimelines ()
{
	int numOfTimelines;
	numOfTimelines = getNumTags( "DOMTimeline" );
	
	for( int i=0; i<numOfTimelines; i++ )
	{
		DOMTimeline dom;
		dom.name			= getAttribute( "DOMTimeline", "name",			"", i );
		dom.currentFrame	= getAttribute( "DOMTimeline", "currentFrame",	1,  i );
		domTimeline			= dom;
		
		pushTag( "DOMTimeline", i );
		pushTag( "layers", 0 );
		
		buildLayers();
		
		popTag();
		popTag();
		
		return;		// SUPPORT ONLY ONE TIMELINE.
	}
	
	popTag();
}

void ofxFlashXFLBuilder :: buildLayers ()
{
	int numOfLayers;
	numOfLayers = getNumTags( "DOMLayer" );
	lastKeyFrameDisplayObject = NULL;
	
	for( int i=numOfLayers-1; i>=0; i-- )	// work backwards through layers. so when adding to stage, objects sit in right order.
	{
		DOMLayer dom;
		dom.name		= getAttribute( "DOMLayer", "name",			"",		i );
		dom.color		= getAttribute( "DOMLayer", "color",		0,		i );
		dom.locked		= getAttribute( "DOMLayer", "locked",		false,  i );
		dom.current		= getAttribute( "DOMLayer", "current",		false,  i );
		dom.isSelected	= getAttribute( "DOMLayer", "isSelected",	false,  i );
		dom.autoNamed	= getAttribute( "DOMLayer", "autoNamed",	false,  i );
		dom.layerType	= getAttribute( "DOMLayer", "layerType",	"",		i );
		domLayer.index = i;
		domLayer		= dom;
		
		if( domLayer.layerType == "guide" )		// skip guide layers.
			continue;
		
		pushTag( "DOMLayer", i );
		pushTag( "frames", 0 );
		// cout << "XXXXXXXXXXXXXXX" << endl;
		activeLayerIndex = 		((ofxFlashMovieClip *)container)->createNewLayer();
		
		ofxFlashMovieClip* mc;
		mc = (ofxFlashMovieClip*)container;
		mc->setTotalFrames( activeLayerIndex, totalFrames );
		
		buildFrames();
		
		popTag();
		popTag();
	}
	
}

void ofxFlashXFLBuilder :: buildFrames ()
{
	int numOfFrames;
	numOfFrames = getNumTags( "DOMFrame" );
	
	ofxFlashMovieClip* containerMc;
	containerMc = (ofxFlashMovieClip*)container;
	
	for( int i=0; i<numOfFrames; i++ )
	{
		DOMFrame * dom = new DOMFrame();
		dom->index			= getAttribute( "DOMFrame", "index",			0,		i );
		dom->duration		= getAttribute( "DOMFrame", "duration",			1,		i );
		dom->tweenType		= getAttribute( "DOMFrame", "tweenType",		"",		i );
		dom->motionTweenSnap	= getAttribute( "DOMFrame", "motionTweenSnap",	false,	i );
		dom->keyMode			= getAttribute( "DOMFrame", "keyMode",			0,		i );
		domFrame			=  *dom;
		
		pushTag( "DOMFrame", i );
		pushTag( "elements", 0 );
	//	cout << "AP buildFrames > frame index: " << dom.index << endl;
	//	cout << "AP buildFrames > frame duration: " << dom.duration << endl;
	//	cout << "AP buildFrames > frame keyMode: " << dom.keyMode << endl;		
		buildElements();
				// cout << "XXXXXXXXXXXXXX1" << endl;
			addTimelineDataToFrames();	
				// cout << "XXXXXXXXXXXXXXX2" << endl;
		popTag();
		popTag();
		
	//	containerMc->addDOMFrameToTimeline(dom);


	}
}

void ofxFlashXFLBuilder :: buildElements ()
{
	int numOfElements = 0;
	TiXmlElement* child = ( storedHandle.FirstChildElement() ).ToElement();
	for( numOfElements = 0; child; child = child->NextSiblingElement(), ++numOfElements ) {}
	
	for( int i=0; i<numOfElements; i++ )
	{
		TiXmlElement* child = ( storedHandle.ChildElement( i ) ).ToElement();
		string elementTag = child->Value();
		
		if( elementTag == "DOMGroup" )
		{
			pushTagAt( i );
			pushTag( "members", 0 );
			
			buildElements();
			
			popTag();
			popTag();
			
		}
		else if( elementTag == "DOMBitmapInstance" )
		{
			DOMBitmapInstance dom;
			dom.libraryItemName = child->Attribute( "libraryItemName" );
			if(child->Attribute( "name" )) dom.name = child->Attribute( "name" ); // Support for CS5.5, which doesn't export name tags if empty
			dom.referenceID		= "";
			domBitmapInstance	= dom;
			
			pushTagAt( i );
			
			buildBitmap();
			
			popTag();
		}
		else if( elementTag == "DOMSymbolInstance" )
		{
			DOMSymbolInstance dom;
			dom.libraryItemName	= child->Attribute( "libraryItemName" );
			if(child->Attribute( "name" )) dom.name = child->Attribute( "name" ); // Support for CS5.5, which doesn't export name tags if empty
			dom.centerPoint3DX	= 0.0;
			dom.centerPoint3DY	= 0.0; 
			domSymbolInstance	= dom;
			
			pushTagAt( i );
			
			buildMovieClip();
			
			popTag();
		}
		else if( elementTag == "DOMRectangleObject" )
		{
			DOMRectangleObject dom;
			child->QueryFloatAttribute( "x",			&dom.x );
			child->QueryFloatAttribute( "y",			&dom.y );
			child->QueryFloatAttribute( "objectWidth",	&dom.objectWidth );
			child->QueryFloatAttribute( "objectHeight",	&dom.objectHeight );
			domRectangleObject = dom;
			
			pushTagAt( i );
			
			buildRectangleShape();
			
			popTag();
		}
		else if( elementTag == "DOMOvalObject" )
		{
			DOMOvalObject dom;
			child->QueryFloatAttribute( "x",			&dom.x );
			child->QueryFloatAttribute( "y",			&dom.y );
			child->QueryFloatAttribute( "objectWidth",	&dom.objectWidth );
			child->QueryFloatAttribute( "objectHeight",	&dom.objectHeight );
			child->QueryFloatAttribute( "endAngle",		&dom.endAngle );
			domOvalObject = dom;
			
			pushTagAt( i );
			
			buildOvalShape();
			
			popTag();
		}
		else if( elementTag == "DOMShape" )
		{
			continue;	// NOT SUPPORTED AT THE MOMENT.
		}
		else if( elementTag == "DOMStaticText" )
		{
			continue;	// NOT SUPPORTED AT THE MOMENT.
		}
		else if( elementTag == "DOMDynamicText" )
		{
			continue;	// NOT SUPPORTED AT THE MOMENT.
		}
		else if( elementTag == "DOMInputText" )
		{
			continue;	// NOT SUPPORTED AT THE MOMENT.
		}
	}
}

void ofxFlashXFLBuilder :: buildBitmap ()
{
	ofBaseDraws* bitmapImage;
	bitmapImage = ofxFlashLibrary :: getInstance()->getAsset( domBitmapInstance.libraryItemName );
	
    //
    // if no name is given for bitmap, use the file name.
    // file name is taken from libraryItemName.
    //
    string name;
    name = domBitmapInstance.name;
    if( name.size() == 0 )
        name = ofSplitString( domBitmapInstance.libraryItemName, "/" ).back();
    
	ofxFlashBitmap* bm;
	bm = new ofxFlashBitmap( bitmapImage );
	bm->name( name );
	bm->libraryItemName( domBitmapInstance.libraryItemName );

	setupMatrixForDisplayObject( bm );
	
	addDisplayObjectToFrames( bm );
}

void ofxFlashXFLBuilder :: buildMovieClip ()
{
    bool bAddLibraryToPath = false;
    bAddLibraryToPath = bAddLibraryToPath || domType == DOM_DOCUMENT_TYPE;
    bAddLibraryToPath = bAddLibraryToPath || domType == DOM_SYMBOL_ITEM_TYPE;
    
	string libraryItemPath = "";
	libraryItemPath += bAddLibraryToPath ? "LIBRARY/" : "";
	libraryItemPath += domSymbolInstance.libraryItemName;
	libraryItemPath += ".xml";

	ofxFlashMovieClip* mc;
	mc = new ofxFlashMovieClip();
	mc->name( domSymbolInstance.name );
	mc->libraryItemName( domSymbolInstance.libraryItemName );
	
	setupMatrixForDisplayObject( mc );
	setupColorForDisplayObject( mc );
	
	addDisplayObjectToFrames( mc );
	
	// Optimization for MC's that repeate on timeline (and are ignore in Flash IDE)
	// If you find'em, assume their children are similar to the previus mc. Avoid the whole building process.
	if(lastKeyFrameDisplayObject != NULL  &&
	   mc->libraryItemName() == lastKeyFrameDisplayObject->libraryItemName() && 
	   mc->name() == lastKeyFrameDisplayObject->name())
	{
		mc->timeline = lastKeyFrameDisplayObject->timeline;
		cout << "OPTIMIZE IT: " << mc->libraryItemName()  << endl;
	} else{
		cout << "Build IT: " <<  mc->libraryItemName()  << endl;
		ofxFlashXFLBuilder* builder;
		builder = new ofxFlashXFLBuilder();
		builder->setVerbose( bVerbose );
		builder->build( xflRoot, libraryItemPath, mc );
		delete builder;
		builder = NULL;
		lastKeyFrameDisplayObject = mc;			
	}
	

	
}

void ofxFlashXFLBuilder :: buildRectangleShape ()
{
	ofxFlashShape* shape;
	shape = new ofxFlashShape();
	
	//-- position & transform.
	
	float cx = domRectangleObject.x + domRectangleObject.objectWidth  * 0.5;		// center point.
	float cy = domRectangleObject.y + domRectangleObject.objectHeight * 0.5;		// center point.
	
	float transformationPointX = cx;												// default transformation point is center.
	float transformationPointY = cy;												// default transformation point is center.
	
	if( tagExists( "transformationPoint", 0 ) )
	{
		pushTag( "transformationPoint", 0 );
		
		transformationPointX = getAttribute( "Point", "x", cx, 0 );
		transformationPointY = getAttribute( "Point", "y", cy, 0 );
		
		popTag();
	}
	
	setupMatrixForDisplayObject( shape );
	
	float shiftX = transformationPointX - cx;
	float shiftY = transformationPointY - cy;
	
	ofxFlashMatrix matrix;
	matrix = shape->matrix();				// get matrix.
	
	float tx = matrix.getTx() + shiftX;
	float ty = matrix.getTy() + shiftY;
	
	matrix.setTx( tx );						// adjust matrix.
	matrix.setTy( ty );
	
	shape->matrix( matrix );				// set matrix.
	
	ofRectangle shapeRect;
	shapeRect.x			= domRectangleObject.x + shiftX;
	shapeRect.y			= domRectangleObject.y + shiftY;
	shapeRect.width		= domRectangleObject.objectWidth;
	shapeRect.height	= domRectangleObject.objectHeight;
	
	shape->setRectangle( shapeRect.x, shapeRect.y, shapeRect.width, shapeRect.height );
	
	setupFillForShape( shape );
	setupStrokeForShape( shape );
	
	addDisplayObjectToFrames( shape );
}

void ofxFlashXFLBuilder :: buildOvalShape ()
{
	ofxFlashShape* shape;
	shape = new ofxFlashShape();
	
	float cx = domOvalObject.x + domOvalObject.objectWidth  * 0.5;		// center point.
	float cy = domOvalObject.y + domOvalObject.objectHeight * 0.5;		// center point.
	
	float transformationPointX = cx;									// default transformation point is center.
	float transformationPointY = cy;									// default transformation point is center.
	
	if( tagExists( "transformationPoint", 0 ) )
	{
		pushTag( "transformationPoint", 0 );
		
		transformationPointX = getAttribute( "Point", "x", cx, 0 );
		transformationPointY = getAttribute( "Point", "y", cy, 0 );
		
		popTag();
	}
	
	setupMatrixForDisplayObject( shape );
	
	float shiftX = transformationPointX - cx;
	float shiftY = transformationPointY - cy;
	
	ofxFlashMatrix matrix;
	matrix = shape->matrix();				// get matrix.
	
	float tx = matrix.getTx() + shiftX;
	float ty = matrix.getTy() + shiftY;
	
	matrix.setTx( tx );						// adjust matrix.
	matrix.setTy( ty );
	
	shape->matrix( matrix );				// set matrix.
	
	ofRectangle shapeRect;
	shapeRect.x			= domOvalObject.x + shiftX;
	shapeRect.y			= domOvalObject.y + shiftY;
	shapeRect.width		= domOvalObject.objectWidth;
	shapeRect.height	= domOvalObject.objectHeight;
	
	shape->setOval( shapeRect.x, shapeRect.y, shapeRect.width, shapeRect.height );
	
	setupFillForShape( shape );
	setupStrokeForShape( shape );
	
	addDisplayObjectToFrames( shape );
}

///////////////////////////////////////////
//	COMMON BUILDER FUNCTIONS.
///////////////////////////////////////////

void ofxFlashXFLBuilder :: addDisplayObjectToFrames ( ofxFlashDisplayObject* displayObject )
{
    ofxFlashMovieClip* containerMc;
    containerMc = (ofxFlashMovieClip*)container;
    
	int i = domFrame.index;
	int t = domFrame.index + domFrame.duration;
	/*
	if(displayObject->libraryItemName() == lastKeyFrameDisplayObject->libraryItemName() && 
	   displayObject->name() == lastKeyFrameDisplayObject->name())
	{
		
		
	} else {
	 */
		//	for( i; i<t; i++ )
		//	{
        containerMc->gotoAndStop( i + 1 );
		containerMc->addChild( displayObject, activeLayerIndex );
		//	}		
//	}

    
    containerMc->gotoAndPlay( 1 );
	//	cout << "adding MC to frames: " << displayObject->name() << " " << i + 1 <<  endl;
	
	//lastKeyFrameDisplayObject = displayObject;

}

void ofxFlashXFLBuilder :: addTimelineDataToFrames ( )
{
    ofxFlashMovieClip* containerMc;
    containerMc = (ofxFlashMovieClip*)container;
    
	int i = domFrame.index;
	int t = domFrame.index + domFrame.duration;
	for( i; i<t; i++ )
	{
		containerMc->gotoAndStop( i + 1 );
		containerMc->addTimelineDataToChild( i - domFrame.index ,  activeLayerIndex); // offsetFromKeyFrame, layerIndex
		// 9 10 11 
	}
	containerMc->gotoAndPlay(1);
}


void ofxFlashXFLBuilder :: setupMatrixForDisplayObject ( ofxFlashDisplayObject* displayObject )
{
	if( tagExists( "matrix", 0 ) )
	{
		pushTag( "matrix", 0 );
		
		float a		= getAttribute( "Matrix", "a",  1.0, 0 );
		float b		= getAttribute( "Matrix", "b",  0.0, 0 );
		float c		= getAttribute( "Matrix", "c",  0.0, 0 );
		float d		= getAttribute( "Matrix", "d",  1.0, 0 );
		float tx	= getAttribute( "Matrix", "tx", 0.0, 0 );
		float ty	= getAttribute( "Matrix", "ty", 0.0, 0 );
		
		ofxFlashMatrix matrix;
		matrix.set( a, b, c, d, tx, ty );
		
		displayObject->matrix( matrix );
		displayObject->originalMatrix(matrix);
		popTag();
	}
}

void ofxFlashXFLBuilder :: setupColorForDisplayObject ( ofxFlashDisplayObject* displayObject )
{
	if( tagExists( "color", 0 ) )
	{
		pushTag( "color", 0 );
		
		float alphaMultiplier = getAttribute( "Color", "alphaMultiplier",  1.0, 0 );
		
		displayObject->alpha( alphaMultiplier );
		
		popTag();
	}
}

void ofxFlashXFLBuilder :: setupFillForShape ( ofxFlashShape* shape )
{
	if( tagExists( "fill", 0 ) )
	{
		pushTag( "fill", 0 );
		
		string fillSolidColor;
		fillSolidColor = getAttribute( "SolidColor", "color", "#000000", 0 );
		fillSolidColor = cleanHexString( fillSolidColor );
		
		float fillAlpha;
		fillAlpha = getAttribute( "SolidColor", "alpha",  1.0, 0 );
		
		shape->setFill( true );
		shape->setFillColor( stringToHex( fillSolidColor ) );
		shape->setFillAlpha( fillAlpha );
		
		popTag();
	}
}

void ofxFlashXFLBuilder :: setupStrokeForShape ( ofxFlashShape* shape )
{
	if( tagExists( "stroke", 0 ) )
	{
		pushTag( "stroke", 0 );
		
		int solidStrokeWeight;
		solidStrokeWeight = getAttribute( "SolidStroke", "weight",  0, 0 );
		
		pushTag( "SolidStroke", 0 );
		pushTag( "fill", 0 );
		
		string fillSolidColor;
		fillSolidColor = getAttribute( "SolidColor", "color", "#000000", 0 );
		fillSolidColor = cleanHexString( fillSolidColor );
		
		float fillAlpha;
		fillAlpha = getAttribute( "SolidColor", "alpha",  1.0, 0 );
		
		shape->setStroke( true );
		shape->setStrokeWeight( solidStrokeWeight );
		shape->setStrokeColor( stringToHex( fillSolidColor ) );
		shape->setStrokeAlpha( fillAlpha );
		
		ofColor c;
		
		popTag();
		popTag();
		popTag();
	}
}

///////////////////////////////////////////
//	CUSTOM XML FUNCTIONS.
///////////////////////////////////////////

void ofxFlashXFLBuilder :: pushTagAt( int i )
{
	TiXmlHandle isRealHandle = storedHandle.ChildElement( i );
	if( isRealHandle.ToNode() )
	{
		storedHandle = isRealHandle;
		level++;
	}
}

///////////////////////////////////////////
//	STRING HEX TO INT CONVERSIONS.
///////////////////////////////////////////

string ofxFlashXFLBuilder :: cleanHexString ( string value )
{
    string clean = "0x";
    clean += value.substr( 1, value.length() - 1 );
    
	return clean;
}

int ofxFlashXFLBuilder :: stringToHex ( string value )
{
	unsigned int x;
	stringstream ss;
	ss << hex << value;
	ss >> x;
	
	return x;
}