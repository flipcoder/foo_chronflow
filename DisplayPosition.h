#pragma once
#include "CriticalSection.h"
#include "DbAlbumCollection.h"

class AppInstance;
//class CriticalSection;

class DisplayPosition
{
	AppInstance* appInstance;
public:
	DisplayPosition(AppInstance* instance, CollectionPos startingPos);
	~DisplayPosition(void);
	void moveTargetBy(int n);
	void update(void);

	bool isMoving(void);
	CollectionPos getCenteredPos(void) const; //Position centered or left of center (eg. display between 2. and 3. cover -> CenteredPos = 2. cover
	float getCenteredOffset(void) const; // return in range [0;1)  (eg. display on pos 2.59 -- returns 0.59)
	CollectionPos getOffsetPos(int offset) const; // Relative to centeredPos

	void hardSetCenteredPos(CollectionPos pos);

	void onTargetChange();

private:
	float targetDist2moveDist(float targetDist);

	bool rendering;

	CollectionPos centeredPos;
	volatile float centeredOffset;
	float lastSpeed;
	double lastMovement;
};
