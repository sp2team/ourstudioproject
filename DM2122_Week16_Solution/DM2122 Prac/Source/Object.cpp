#include "Object.h"

Object::Object() // Initialises the object with default values.
{
	init(0, 0, 0, 1, 1, 1, 0, 0, 1, 0);
}

Object::~Object()
{
}

void Object::init(float translateX, float translateY, float translateZ, float scaleX, float scaleY, float scaleZ, float rotationAmt, float rotateXaxis, float rotateYaxis, float rotateZaxis) // Initialises the object with their translation, rotation and scale etc.
{
	setTranslationXYZ(translateX, translateY, translateZ);
	setLastTranslationXYZ(translateX, translateY, translateZ);
	setOffsetXYZ(translateX, translateY, translateZ);
	setScaleXYZ(scaleX, scaleY, scaleZ);
	setRotationAmount(rotationAmt);
	setRotationXYZ(rotateXaxis, rotateYaxis, rotateZaxis);
}

void Object::setTranslationXYZ(float x, float y, float z)
{
	translationX = x;
	translationY = y;
	translationZ = z;
}

void Object::setTranslationX(float x)
{
	translationX = x;
}

void Object::setTranslationY(float y)
{
	translationY = y;
}

void Object::setTranslationZ(float z)
{
	translationZ = z;
}

void Object::setLastTranslationXYZ(float x, float y, float z)
{
	lastTranslationX = x;
	lastTranslationY = y;
	lastTranslationZ = z;
}

void Object::setLastTranslationX(float x)
{
	lastTranslationX = x;
}

void Object::setLastTranslationY(float y)
{
	lastTranslationY = y;
}

void Object::setLastTranslationZ(float z)
{
	lastTranslationZ = z;
}

float Object::getTranslationX()
{
	return translationX;
}

float Object::getTranslationY()
{
	return translationY;
}

float Object::getTranslationZ()
{
	return translationZ;
}

float Object::getLastTranslationX()
{
	return lastTranslationX;
}

float Object::getLastTranslationY()
{
	return lastTranslationY;
}

float Object::getLastTranslationZ()
{
	return lastTranslationZ;
}

void Object::setRotationAmount(float r)
{
	rotationAmount = r;
}

void Object::setRotationXYZ(float x, float y, float z)
{
	rotateX = x;
	rotateY = y;
	rotateZ = z;
}

void Object::setRotateX(float x)
{
	rotateX = x;
}

void Object::setRotateY(float y)
{
	rotateY = y;
}

void Object::setRotateZ(float z)
{
	rotateZ = z;
}

float Object::getRotationAmount()
{
	return rotationAmount;
}

float Object::getRotateX()
{
	return rotateX;
}

float Object::getRotateY()
{
	return rotateY;
}

float Object::getRotateZ()
{
	return rotateZ;
}

void Object::setScaleXYZ(float x, float y, float z)
{
	scaleX = x;
	scaleY = y;
	scaleZ = z;
}

void Object::setScaleX(float x)
{
	scaleX = x;
}

void Object::setScaleY(float y)
{
	scaleY = y;
}

void Object::setScaleZ(float z)
{
	scaleZ = z;
}

float Object::getScaleX()
{
	return scaleX;
}

float Object::getScaleY()
{
	return scaleY;
}

float Object::getScaleZ()
{
	return scaleZ;
}

void Object::setOffsetXYZ(float x, float y, float z)
{
	offsetX = x;
	offsetY = y;
	offsetZ = z;
}

void Object::setOffsetX(float x)
{
	offsetX = x;
}

void Object::setOffsetY(float y)
{
	offsetY = y;
}

void Object::setOffsetZ(float z)
{
	offsetZ = z;
}

float Object::getOffsetX()
{
	return offsetX;
}

float Object::getOffsetY()
{
	return offsetY;
}

float Object::getOffsetZ()
{
	return offsetZ;
}
