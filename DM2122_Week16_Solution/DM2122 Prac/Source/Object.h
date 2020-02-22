#ifndef OBJECT_H
#define OBJECT_H

class Object
{
private:
	float translationX;
	float translationY;
	float translationZ;
	float lastTranslationX;
	float lastTranslationY;
	float lastTranslationZ;
	float rotationAmount;
	float rotateX;
	float rotateY;
	float rotateZ;
	float scaleX;
	float scaleY;
	float scaleZ;
	float offsetX;
	float offsetY;
	float offsetZ;
public:
	Object();
	~Object();

	void init(float translateX, float translateY, float translateZ, float scaleX, float scaleY, float scaleZ, float rotationAmt, float rotateXaxis, float rotateYaxis, float rotateZaxis); // Use this to init your object values.

	void setTranslationXYZ(float x, float y, float z);
	void setTranslationX(float x);
	void setTranslationY(float y);
	void setTranslationZ(float z);

	void setLastTranslationXYZ(float x, float y, float z); // Don't use these unless you know what they are for, it will mess up the movement.
	void setLastTranslationX(float x); // *
	void setLastTranslationY(float y); // *
	void setLastTranslationZ(float z); // * 

	float getTranslationX();
	float getTranslationY();
	float getTranslationZ();

	float getLastTranslationX(); // * 
	float getLastTranslationY(); // *
	float getLastTranslationZ(); // *

	void setRotationAmount(float r);
	void setRotationXYZ(float x, float y, float z);
	void setRotateX(float x);
	void setRotateY(float y);
	void setRotateZ(float z);

	float getRotationAmount();
	float getRotateX();
	float getRotateY();
	float getRotateZ();

	void setScaleXYZ(float x, float y, float z); // Note: Don't use this unless you're doing a GUI, basically don't use this if it has anything to do with collision detection.
	void setScaleX(float x); // !
	void setScaleY(float y); // !
	void setScaleZ(float z); // !

	float getScaleX(); // !
	float getScaleY(); // !
	float getScaleZ(); // !
	
	void setOffsetXYZ(float x, float y, float z); // Don't use these unless you know what they are for, it will mess up the collision detection.
	void setOffsetX(float x); // *
	void setOffsetY(float y); // *
	void setOffsetZ(float z); // *

	float getOffsetX(); // *
	float getOffsetY(); // *
	float getOffsetZ(); // *
};

#endif