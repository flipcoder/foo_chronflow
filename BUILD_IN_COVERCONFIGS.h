static const char* buildInCoverConfigs[] = {\
"Albumart (build-in)", "function coverPosition(coverId){\r\n	var x, y, z;\r\n	x = coverId*0.0001;\r\n	y = 0;\r\n	z = -Math.abs(coverId)*1.5;\r\n	return new Array(x, y, z);\r\n}\r\nfunction coverRotation(coverId){\r\n	var angle = coverId * 180;\r\n	return new Array(angle,0,1,0);\r\n}\r\nfunction coverAlign(coverId){ return new Array(0, 0) }\r\nfunction coverSizeLimits(coverId){ return new Array(1, 1) }\r\n\r\nfunction drawCovers(){ return new Array(-1, 1) }\r\nfunction aspectBehaviour(){ return new Array(1, 0) }\r\n\r\n// the FoV is 45° -> this calculated distance shows us exactly one cover\r\nfunction eyePos(){ return new Array(0, 0, 0.5/Math.tan(Math.PI/8)) }\r\nfunction lookAt(){ return new Array(0, 0, 0) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\nfunction showMirrorPlane(){ return false }",  \
"Default (v0.2) (build-in)", "function coverPosition(coverId){\r\n	var x, y, z;\r\n	y = 0;\r\n	if (Math.abs(coverId) <= 1){ // The centered cover\r\n		z = 1 + 3 * (1 - Math.abs(coverId));\r\n		x = coverId;\r\n	} else { // The covers on the side\r\n		z = 1 - (Math.abs(coverId)-1) * 0.15;\r\n		x = 1 + 0.5 * (Math.abs(coverId)-1);\r\n		if (coverId < 0)\r\n			x *= -1;\r\n	}\r\n	return new Array(x, y, z);\r\n}\r\n\r\nfunction coverRotation(coverId){\r\n	var angle;\r\n	if (Math.abs(coverId) < 1){ // The centered cover\r\n		angle = coverId * -70;\r\n	} else { // The covers on the side\r\n		if (coverId > 0)\r\n			angle = -70;\r\n		else\r\n			angle = 70;\r\n	}\r\n	return new Array(angle, 0, 1, 0);\r\n}\r\n\r\nfunction coverSizeLimits(coverId){\r\n	if (Math.abs(coverId) < 1){ // The centered cover\r\n		var w, h;\r\n		w = 1;\r\n		h = 1.2 + Math.abs(coverId) * 0.8;\r\n		return new Array(w, h);\r\n	} else { // The covers on the side\r\n		return new Array(1, 2);\r\n	}\r\n}\r\n\r\nfunction coverAlign(coverId){ return new Array(0, -1) }\r\n\r\nfunction drawCovers(){ return new Array(-20, 20) }\r\nfunction aspectBehaviour(){ return new Array(0,1) }\r\n\r\nfunction eyePos(){ return new Array(0, 0.5, 6) }\r\nfunction lookAt(){ return new Array(0, 0.5, 0) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\nfunction showMirrorPlane(){ return true }\r\nfunction mirrorPoint(){ return new Array(0, 0, 0) }\r\nfunction mirrorNormal(){ return new Array(0, 1, 0) }",  \
"Default (v0.3) (build-in)", "// Author: mil3s\r\n\r\n// General Notes:\r\n// Most of these functions return an array with 3 elements\r\n// These are the x,y,z coordinates in 3d-space\r\n// x is left to right\r\n// y is bottom to top\r\n// z is front to back\r\n\r\nvar coverSpacing = 0.05;\r\n\r\n/************************* COVER DISPLAY *****************/\r\n// These functions define the Display of the single Covers\r\n// The given parameter coverId is a floating point number.\r\n// It is 0 for the center cover, 1 for the one right\r\n// beneath it, -1 for the one on the left side and so on.\r\n// During movement the values float between the integer\r\n// values.\r\nfunction coverPosition(coverId){\r\n	var x, y, z;\r\n	y = 0;\r\n	if (Math.abs(coverId) <= 1){ // The centered cover\r\n		z = 4 + 0.5 * (1 - Math.abs(coverId));\r\n		x = coverId * 0.875;\r\n	} else { // The covers on the side\r\n		z = 4 - (Math.abs(coverId)-1) * 0.01;\r\n		x = 0.875 + coverSpacing * (Math.abs(coverId)-1);\r\n		if (coverId < 0)\r\n			x *= -1;\r\n	}\r\n	return new Array(x, y, z);\r\n}\r\n\r\n// return array is (angle, x, y, z) - this rotates\r\n// the cover *angle* degrees around the vector (x,y,z)\r\n// With (0,0,0,0) the cover is parallel to the x-y-Plane\r\nfunction coverRotation(coverId){\r\n	var angle;\r\n	if (Math.abs(coverId) < 1){ // The centered cover\r\n		angle = coverId * -60;\r\n	} else { // The covers on the side\r\n		if (coverId > 0)\r\n			angle = -60;\r\n		else\r\n			angle = 60;\r\n	}\r\n	return new Array(angle, 0, 1, 0);\r\n}\r\n\r\n// Defines the the size boundaries for the cover.\r\n// Aspect ratio is preserved.\r\n// Return Array is (widht, height)\r\nfunction coverSizeLimits(coverId){\r\n	return new Array(1, 1);\r\n}\r\n\r\n\r\n// Sets which point of the cover coverPosition() defines\r\n// (-1,-1) means bottom left, (0,0) means center,\r\n// (1,1) means top right, (0, -1) means bottom center etc.\r\n// The cover is also rotated around this point.\r\nfunction coverAlign(coverId){\r\n	return new Array(0, -1);\r\n}\r\n\r\n\r\n// Defines the range of covers to draw.\r\n// Return array is (leftmostCover, rightmostCover)\r\n// This interval shouldn\'t be larger than 80\r\n// The center cover is 0.\r\nfunction drawCovers(){\r\n	return new Array(-30, 30);\r\n}\r\n\r\n\r\n// In which direction should the fov be expanded/shrinked\r\n// when the panel is resized?\r\n// If this returns (0,1), the height is fixed.\r\n// If this returns (1,0), the width is fixed.\r\n// You can also return stuff like (0.5,0.5) or (7, 3)\r\n// The values determine how important it is for this\r\n// dimension to stay fixed.\r\nfunction aspectBehaviour(){\r\n	return new Array(0, 1);\r\n}\r\n\r\n/************************** CAMMERA SETUP ****************/\r\n// Position of the viewport\r\nfunction eyePos(){\r\n	return new Array(0, 1.15, 6.1);\r\n}\r\n// Defines the point for the eye to look at\r\nfunction lookAt(){\r\n	return new Array(0, -1.15, 0);\r\n}\r\n// Used to rotate the view.\r\n// The returned Vector points upwards in the viewport.\r\n// This vector must not be parallel to the line of sight from the\r\n// eyePos point to the lookAt point.\r\nfunction upVector(){\r\n	return new Array(0, 1, 0);\r\n}\r\n\r\n/************************** MIRROR SETUP *****************/\r\nfunction showMirrorPlane(){\r\n	return true; // return false to hide the mirror\r\n}\r\n// Any Point on the Mirror Plane\r\nfunction mirrorPoint (){\r\n	return new Array(0, 0, 0);\r\n}\r\n// Normal of the Mirror Plane\r\nfunction mirrorNormal (){\r\n	return new Array(0, 1, 0);\r\n}",  \
"Default (build-in)", "// Author: mil3s\r\n\r\n// General Notes:\r\n// Most of these functions return an array with 3 elements\r\n// These are the x,y,z coordinates in 3d-space\r\n// x is left to right\r\n// y is bottom to top\r\n// z is front to back\r\n\r\nvar coverSpacing = 0.07;\r\n\r\n/************************* COVER DISPLAY *****************/\r\n// These functions define the Display of the single Covers\r\n// The given parameter coverId is a floating point number.\r\n// It is 0 for the center cover, 1 for the one right\r\n// beneath it, -1 for the one on the left side and so on.\r\n// During movement the values float between the integer\r\n// values.\r\nfunction coverPosition(coverId){\r\n	var x, y, z;\r\n	y = 0;\r\n	if (Math.abs(coverId) <= 1){ // The centered cover\r\n		var z_logit =  -0.04 + 0.55 / (1+Math.pow(Math.E,Math.abs(6.5*coverId )-4));\r\n		var z_root = z = 0.5 * Math.pow(1 - Math.abs(coverId),1.1)\r\n		var alpha = 0.3;\r\n		z = 4 + alpha * z_logit + (1-alpha) * z_root;\r\n		x = coverId * 0.875;\r\n	} else { // The covers on the side\r\n		z = 4 - (Math.abs(coverId)-1) * 0.01\r\n			  - Math.pow((Math.abs(coverId)-1)*0.025,2);\r\n		x = 0.875 + coverSpacing * (Math.abs(coverId)-1);\r\n		if (coverId < 0)\r\n			x *= -1;\r\n	}\r\n	return new Array(x, y, z);\r\n}\r\n\r\n\r\n// return array is (angle, x, y, z) - this rotates\r\n// the cover *angle* degrees around the vector (x,y,z)\r\n// With (0,0,0,0) the cover is parallel to the x-y-Plane\r\nfunction coverRotation(coverId){\r\n	var angle;\r\n	if (Math.abs(coverId) < 1){ // The centered cover\r\n		angle = coverId * -60;\r\n	} else { // The covers on the side\r\n		if (coverId > 0)\r\n			angle = -60;\r\n		else\r\n			angle = 60;\r\n	}\r\n	return new Array(angle, 0, 1, 0);\r\n}\r\n\r\n// Defines the the size boundaries for the cover.\r\n// Aspect ratio is preserved.\r\n// Return Array is (widht, height)\r\nfunction coverSizeLimits(coverId){\r\n	return new Array(1, 1);\r\n}\r\n\r\n\r\n// Sets which point of the cover coverPosition() defines\r\n// (-1,-1) means bottom left, (0,0) means center,\r\n// (1,1) means top right, (0, -1) means bottom center etc.\r\n// The cover is also rotated around this point.\r\nfunction coverAlign(coverId){\r\n	return new Array(0, -1);\r\n}\r\n\r\n\r\n// Defines the range of covers to draw.\r\n// Return array is (leftmostCover, rightmostCover)\r\n// This interval shouldn\'t be larger than 80\r\n// The center cover is 0.\r\nfunction drawCovers(){\r\n	return new Array(-40, 40);\r\n}\r\n\r\n\r\n// In which direction should the fov be expanded/shrinked\r\n// when the panel is resized?\r\n// If this returns (0,1), the height is fixed.\r\n// If this returns (1,0), the width is fixed.\r\n// You can also return stuff like (0.5,0.5) or (7, 3)\r\n// The values determine how important it is for this\r\n// dimension to stay fixed.\r\nfunction aspectBehaviour(){\r\n	return new Array(0, 1);\r\n}\r\n\r\n/************************** CAMMERA SETUP ****************/\r\n// Position of the viewport\r\nfunction eyePos(){\r\n	return new Array(0, 1.15, 6.1);\r\n}\r\n// Defines the point for the eye to look at\r\nfunction lookAt(){\r\n	return new Array(0, -1.15, 0);\r\n}\r\n// Used to rotate the view.\r\n// The returned Vector points upwards in the viewport.\r\n// This vector must not be parallel to the line of sight from the\r\n// eyePos point to the lookAt point.\r\nfunction upVector(){\r\n	return new Array(0, 1, 0);\r\n}\r\n\r\n/************************** MIRROR SETUP *****************/\r\nfunction showMirrorPlane(){\r\n	return true; // return false to hide the mirror\r\n}\r\n// Any Point on the Mirror Plane\r\nfunction mirrorPoint (){\r\n	return new Array(0, 0, 0);\r\n}\r\n// Normal of the Mirror Plane\r\nfunction mirrorNormal (){\r\n	return new Array(0, 1, 0);\r\n}",  \
"Shelf (build-in)", "// Author: Martin Gloderer\r\n\r\nfunction coverPosition(coverId){\r\n	// Ensure smooth exit at the borders of drawCovers()\r\n	if (coverId < drawCovers()[0]+1)\r\n		return coverPosition(drawCovers()[0]+1);\r\n	if (coverId > drawCovers()[1]-1) {\r\n		array = coverPosition(drawCovers()[1]-1);\r\n		array[2] -= 0.0001;\r\n		return array;\r\n	}\r\n\r\n	var x, y, z;\r\n	var cAbs = Math.abs(coverId);\r\n\r\n	if (cAbs >= 1) {\r\n		x = Math.ceil(cAbs/4) + 1.8;\r\n		y = (cAbs-1) % 8;\r\n		y = Math.min(y, 7-y);\r\n		if (y > 3 || y < 0) { //transistion between columns\r\n			x -= 1 - cAbs % 1;\r\n		}\r\n		y = Math.min(Math.max(y,0),3);\r\n		z = -Math.abs(x - 2.8)/5;\r\n	} else { //cAbs < 1\r\n		x = sigmoidInterpolation(cAbs)*Math.sqrt(cAbs) * 2.8;\r\n		y = 0;\r\n		z = sigmoidInterpolation(1-cAbs) * 0.4;\r\n	}\r\n	if (coverId < 0) {\r\n		x *= -1;\r\n	}\r\n	// Shrink scene to get a stronger mirror effect\r\n	x /= 4;  y /= 4;  z /= 2.5;\r\n	return new Array(x, y, z);\r\n}\r\n\r\nfunction coverSizeLimits(coverId){\r\n	var cAbs = Math.abs(coverId);\r\n	var w, h;\r\n	if (cAbs < 1){ // The centered cover\r\n		w = (sigmoidInterpolation(1-cAbs)*(1-cAbs) * 0.75)+0.25;\r\n		h = (sigmoidInterpolation(1-cAbs)*(1-cAbs) * 0.75)+0.25;\r\n	} else {\r\n		// Shrink scene to get a stronger mirror effect\r\n		w = 0.25;\r\n		h = 0.25;\r\n	}\r\n	return new Array(h, w);\r\n}\r\n\r\nfunction sigmoidInterpolation(t) {\r\n	t = (t-0.5)*8;\r\n	t = 1/(1 + Math.pow(Math.E, -t));\r\n	return t;\r\n}\r\n\r\nfunction coverRotation(coverId){ return new Array(0, 0, 0, 0) }\r\nfunction coverAlign(coverId){ return new Array(0, -1) }\r\n\r\n\r\nfunction drawCovers(){ return new Array(-25, 25) }\r\nfunction aspectBehaviour(){ return new Array(0,1) }\r\n\r\n\r\nfunction eyePos(){ return new Array(0, 0.42, 1.6) }\r\nfunction lookAt(){ return new Array(0, 0.42, 0) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\n\r\nfunction showMirrorPlane(){ return true }\r\nfunction mirrorPoint (){ return new Array(0, 0, 0) }\r\nfunction mirrorNormal (){ return new Array(0, 1, 0) }",  \
"Simple (build-in)", "// This config is very fast, since it draws only 5 covers\r\n\r\nfunction coverPosition(coverId){\r\n	var x, y, z;\r\n	x = coverId * 1.1;\r\n	y = 0;\r\n	z = -1.5 - Math.abs(coverId);\r\n	return new Array(x, y, z);\r\n}\r\n\r\nfunction coverRotation(coverId){\r\n	var angle;\r\n	// this makes the covers disappera smoothly\r\n	if (Math.abs(coverId) >= 2){\r\n		var f = Math.abs(coverId) - 2 // this is in [0-1]\r\n		// at this angle the covers are not visible\r\n		var hideAngle = rad2deg(Math.atan(coverPosition(3)[2] / coverPosition(3)[0]));\r\n		angle = hideAngle * f;\r\n		if (coverId > 0)\r\n			 angle *= -1;\r\n	} else {\r\n		angle = 0;\r\n	}\r\n	return new Array(angle,0,1,0);\r\n}\r\nfunction rad2deg(angle){\r\n	return angle/Math.PI * 180;\r\n}\r\n\r\n\r\nfunction coverSizeLimits(coverId){ return new Array(1, 1) }\r\nfunction coverAlign(coverId){ return new Array(0, 0) }\r\n\r\nfunction drawCovers(){ return new Array(-3, 3) }\r\nfunction aspectBehaviour(){ return new Array(0, 1) }\r\n\r\n\r\nfunction eyePos(){ return new Array(0, 0, 0) }\r\nfunction lookAt(){ return new Array(0, 0, -1.5) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\nfunction showMirrorPlane(){ return false }",  \
"Spiral (build-in)", "var outerRadius = 1.3;\r\nvar verticalSpacing = 1.7;\r\n\r\n\r\n// Some geometry (this is a regular polygon)\r\nvar sidelength = 1;\r\nvar n = Math.PI / Math.asin(sidelength / (2*outerRadius))\r\nvar a = Math.PI / n;\r\nvar factor = 2*Math.PI / n;\r\nvar innerRadius = outerRadius * Math.cos(a);\r\n\r\nfunction coverPosition(coverId){\r\n	var x, y, z;\r\n	x = Math.sin(coverId * factor) * innerRadius;\r\n	y = coverId * verticalSpacing / n;\r\n	z = Math.cos(coverId * factor) * innerRadius;\r\n	return new Array(x, y, z);\r\n}\r\n\r\nfunction coverRotation(coverId){\r\n	// We use radian but have to return degrees\r\n	var angle = rad2deg(coverId*factor);\r\n	return new Array(angle, 0, 1, 0);\r\n}\r\nfunction rad2deg(angle){\r\n	return angle/Math.PI * 180;\r\n}\r\n\r\nfunction coverSizeLimits(coverId){ return new Array(sidelength, verticalSpacing*0.85) }\r\nfunction coverAlign(coverId){ return new Array(0, -1) }\r\n\r\nfunction drawCovers(){ return new Array(-15, 18) }\r\nfunction aspectBehaviour(){ return new Array(1, 0) }\r\n\r\n\r\nfunction eyePos(){ return new Array(0.5, 0.9, outerRadius*4) }\r\nfunction lookAt(){ return new Array(0.5, 0.9, 0) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\n\r\nfunction showMirrorPlane(){ return true }\r\nfunction mirrorPoint(){ return new Array(outerRadius, 0, 0) }\r\nfunction mirrorNormal(){ return new Array(1, 0, 0) }",  \
"Vinyl Collection (build-in)", "// Original Concept: Martin Gloderer\r\n\r\nvar viewFromLeftSide = false;\r\nvar coverSpacing = 0.03;\r\n\r\n\r\nfunction coverPosition(coverId){\r\n	var z = -coverId * coverSpacing;\r\n	return new Array(0, 0, z);\r\n}\r\n\r\nfunction coverRotation(coverId){\r\n	var angle;\r\n	if (coverId < -0.7){\r\n		angle = 32;\r\n	} else if (coverId >= -0.7 && coverId <= -0.3) {\r\n		angle = -32 * (1 + (coverId + 0.3) * 5);\r\n	} else { // coverId > -0.3\r\n		angle = -32;\r\n	}\r\n	angle += 22;\r\n	return new Array(angle,1,0,0);\r\n}\r\n\r\nfunction coverAlign(coverId){	return new Array(0, -1) }\r\nfunction coverSizeLimits(coverId){ return new Array(1, 1) }\r\n\r\nfunction drawCovers(){ return new Array(-7, 20) }\r\nfunction aspectBehaviour(){ return new Array(0,1) }\r\n\r\nfunction eyePos(){\r\n	var x = 0.4;\r\n	if (viewFromLeftSide) x *= -1;\r\n	return new Array(x, 1.1, 1.3);\r\n}\r\nfunction lookAt(){ return new Array(0, 0.5, 0) }\r\nfunction upVector(){ return new Array(0, 1, 0) }\r\n\r\nfunction showMirrorPlane(){ return false; }",  \
""};
static const int buildInCoverConfigCount = 8;
