
#pragma once

enum frameTypeEnum {
	FRAME_TYPE_KEY, FRAME_TYPE_EMPTY_KEY, FRAME_TYPE_EMPTY, FRAME_TYPE_KEY_IGNORE, FRAME_TYPE_KEY_TWEEN_START, FRAME_TYPE_KEY_TWEEN_MID, FRAME_TYPE_KEY_TWEEN_END
};

struct DOMTimeline
{
	string	name;
	int		currentFrame;
};

struct DOMLayer
{
	string	name;
	int		color;
	bool	locked;
	bool	current;
	bool	isSelected;
	bool	autoNamed;
	string	layerType;
	int index;
};

struct DOMFrame
{
	int		index;
	int		duration;
	string	tweenType;
	bool	motionTweenSnap;
	int		keyMode;
	//frameTypeEnum frameType;
	//DOMFrame * LinkedKeyFrame;
};

struct DOMSymbolInstance
{
	string	libraryItemName;
	string	name;
	float	centerPoint3DX;
	float	centerPoint3DY;
};

struct DOMBitmapInstance
{
	string	libraryItemName;
	string	name;
	string	referenceID;
};

struct DOMRectangleObject
{
	float	x;
	float	y;
	float	objectWidth;
	float	objectHeight;
};

struct DOMOvalObject
{
	float	x;
	float	y;
	float	objectWidth;
	float	objectHeight;
	float	endAngle;
};
