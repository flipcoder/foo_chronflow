#include "stdafx.h"
#include "config.h"

#include "Renderer.h"

#include "AsynchTexLoader.h"
#include "AppInstance.h"
#include "DisplayPosition.h"
#include "DbAlbumCollection.h"
#include "Helpers.h"
#include "ImgTexture.h"
#include "ScriptedCoverPositions.h"
#include "glStructs.h"


// Extensions
PFNGLFOGCOORDFPROC glFogCoordf = NULL;
PFNGLBLENDCOLORPROC glBlendColor = NULL;


#define SELECTION_CENTER INT_MAX //Selection is an unsigned int, so this is center
#define SELECTION_COVERS 1
#define SELECTION_MIRROR 2

Renderer::Renderer(AppInstance* instance, DisplayPosition* displayPos)
: appInstance(instance),
  displayPos(displayPos),
  textDisplay(this)
{
}

Renderer::~Renderer(void)
{
}

void Renderer::initGlState()
{
	// move this to an extra function - otherwice it's called twice due to mulitsampling
	loadExtensions();

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_FASTEST);
	glEnable(GL_TEXTURE_2D);

	GLint maxTexSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);
	ImgTexture::setMaxGlTextureSize(maxTexSize);
	
	if (isExtensionSupported("GL_EXT_fog_coord")){
		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 5);
		GLfloat	fogColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};     // Fog Color - should be BG color
		glFogfv(GL_FOG_COLOR, fogColor);					// Set The Fog Color
		glHint(GL_FOG_HINT, GL_NICEST);						// Per-Pixel Fog Calculation
		glFogi(GL_FOG_COORD_SRC, GL_FOG_COORD);		// Set Fog Based On Vertice Coordinates
	}

	glfwSwapInterval(0);
	vSyncEnabled = false;
}

void Renderer::ensureVSync(bool enableVSync){
	if (vSyncEnabled != enableVSync){
		vSyncEnabled = enableVSync;
		glfwSwapInterval(enableVSync ? 1 : 0);
	}
}

void Renderer::loadExtensions(){
	if(isExtensionSupported("GL_EXT_fog_coord"))
		glFogCoordf = (PFNGLFOGCOORDFPROC) wglGetProcAddress("glFogCoordf");
	else
		glFogCoordf = 0;

	if(isExtensionSupported("GL_EXT_blend_color"))
		glBlendColor = (PFNGLBLENDCOLORPROC)wglGetProcAddress("glBlendColor");
	else
		glBlendColor = 0;
}

bool Renderer::isExtensionSupported(const char *extName){
	char *p;
	char *end;
	int extNameLen;

	extNameLen = strlen(extName);

	p = (char *)glGetString(GL_EXTENSIONS);
	if (!p) {
		return false;
	}

	end = p + strlen(p);

	while (p < end) {
		int n = strcspn(p, " ");
		if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
			return true;
		}
		p += (n + 1);
	}
	return false;
}

void Renderer::resizeGlScene(int width, int height){
	if (height == 0)
		height = 1;
	winWidth = width;
	winHeight = height;

	glViewport(0, 0, width, height); // Reset The Current Viewport
	setProjectionMatrix();
}

void Renderer::getFrustrumSize(double &right, double &top, double &zNear, double &zFar) {
	zNear = 0.1;
	zFar  = 500;
	// Calculate The Aspect Ratio Of The Window
	static const double fov = 45;
	static const double squareLength = tan(deg2rad(fov)/2) * zNear;
	fovAspectBehaviour weight = coverPos.getAspectBehaviour();
	double aspect = (double)winHeight/winWidth;
	right = squareLength / pow(aspect, (double)weight.y);
	top   = squareLength * pow(aspect, (double)weight.x);
}

void Renderer::setProjectionMatrix(bool pickMatrix, int x, int y){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (pickMatrix){
		GLint viewport[4];
		glGetIntegerv (GL_VIEWPORT, viewport);
		gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 
					  1.0, 1.0, viewport);
	}
	double right, top, zNear, zFar;
	getFrustrumSize(right, top, zNear, zFar);
	glFrustum(-right, +right, -top, +top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::glPushOrthoMatrix(){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, winWidth, 0, winHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Renderer::glPopOrthoMatrix(){
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}

bool Renderer::offsetOnPoint(int x, int y, int& out) 
{
	GLuint buf[256];
	glSelectBuffer(256, buf);
	glRenderMode(GL_SELECT);
	setProjectionMatrix(true, x, y);
	glInitNames();
	drawScene(true);
	GLint hits = glRenderMode(GL_RENDER);
	setProjectionMatrix();
	GLuint *p = buf;
	GLuint minZ = INFINITE;
	GLuint selectedName = 0;
	for (int i=0; i < hits; i++){
		GLuint names = *p; p++;
		GLuint z = *p; p++; p++;
		if ((names > 1) && (z < minZ) && (*p == SELECTION_COVERS)){
			minZ = z;
			selectedName = *(p+1);
		}
		p += names;
	}
	if (minZ != INFINITE){
		out = (selectedName - SELECTION_CENTER);
		return true;
	} else {
		return false;
	}
}

void Renderer::drawMirrorPass(){
	glVectord mirrorNormal = coverPos.getMirrorNormal();
	glVectord mirrorCenter = coverPos.getMirrorCenter();

	double mirrorDist; // distance from origin
	mirrorDist = mirrorCenter * mirrorNormal;
	glVectord mirrorPos = mirrorDist * mirrorNormal;

	glVectord scaleAxis(1, 0, 0);
	glVectord rotAxis = scaleAxis.cross(mirrorNormal);
	double rotAngle = rad2deg(scaleAxis.intersectAng(mirrorNormal));
	rotAngle = -2*rotAngle;

	if (glFogCoordf){
		GLfloat	fogColor[4] = {GetRValue(cfgPanelBg)/255.0f, GetGValue(cfgPanelBg)/255.0f, GetBValue(cfgPanelBg)/255.0f, 1.0f};
		glFogfv(GL_FOG_COLOR, fogColor);
		glEnable(GL_FOG);
	}

	glPushMatrix();
		glTranslated(2 * mirrorPos.x, 2 * mirrorPos.y, 2 * mirrorPos.z);
		glScalef(-1.0f, 1.0f, 1.0f);
		glRotated(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);

		drawCovers();
	glPopMatrix();
	
	if (glFogCoordf)
		glDisable(GL_FOG);
}

void Renderer::drawMirrorOverlay(){
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glColor4f(GetRValue(cfgPanelBg)/255.0f, GetGValue(cfgPanelBg)/255.0f, GetBValue(cfgPanelBg)/255.0f, 0.60f);
	
	glShadeModel(GL_FLAT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			glBegin(GL_QUADS);
				glVertex3i (-1, -1, -1);
				glVertex3i (1, -1, -1);
				glVertex3i (1, 1, -1); 
				glVertex3i (-1, 1, -1);
			glEnd();
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
}

pfc::array_t<double> Renderer::getMirrorClipPlane(){
	glVectord mirrorNormal = coverPos.getMirrorNormal();
	glVectord mirrorCenter = coverPos.getMirrorCenter();
	glVectord eye2mCent = (mirrorCenter - coverPos.getCameraPos());

	// Angle at which the mirror normal stands to the eyePos
	double mirrorNormalAngle = rad2deg(eye2mCent.intersectAng(mirrorNormal));
	pfc::array_t<double> clipEq;
	clipEq.set_size(4);
	if (mirrorNormalAngle > 90){
		clipEq[0] = -mirrorNormal.x;
		clipEq[1] = -mirrorNormal.y;
		clipEq[2] = -mirrorNormal.z;
		clipEq[3] = mirrorNormal * mirrorCenter;
	} else {
		clipEq[0] = mirrorNormal.x;
		clipEq[1] = mirrorNormal.y;
		clipEq[2] = mirrorNormal.z;
		clipEq[3] = -(mirrorNormal * mirrorCenter);
	}
	return clipEq;
}

void Renderer::drawScene(bool selectionPass)
{
	glLoadIdentity();
	gluLookAt(
		coverPos.getCameraPos().x, coverPos.getCameraPos().y, coverPos.getCameraPos().z,
		coverPos.getLookAt().x, coverPos.getLookAt().y, coverPos.getLookAt().z,
		coverPos.getUpVector().x, coverPos.getUpVector().y, coverPos.getUpVector().z);
	
	pfc::array_t<double> clipEq;

	if (coverPos.isMirrorPlaneEnabled()){
		clipEq = getMirrorClipPlane();
		if (!selectionPass){
			glClipPlane(GL_CLIP_PLANE0, clipEq.get_ptr());
			glEnable(GL_CLIP_PLANE0);
			glPushName(SELECTION_MIRROR);
				drawMirrorPass();
			glPopName();
			glDisable(GL_CLIP_PLANE0);
			drawMirrorOverlay();
		}

		// invert the clip equation
		for (int i=0; i < 4; i++){
			clipEq[i] *= -1;
		}
		
		glClipPlane(GL_CLIP_PLANE0, clipEq.get_ptr());
		glEnable(GL_CLIP_PLANE0);
	}
	
	glPushName(SELECTION_COVERS);
		drawCovers(true);
	glPopName();
	
	if (coverPos.isMirrorPlaneEnabled()){
		glDisable(GL_CLIP_PLANE0);
	}
}

void Renderer::drawGui(){
	if (cfgShowAlbumTitle || appInstance->albumCollection->getCount() == 0){
		pfc::string8 albumTitle;
		if (appInstance->albumCollection->getCount()){
			appInstance->albumCollection->getTitle(appInstance->albumCollection->getTargetPos(), albumTitle);
		} else if (*(appInstance->reloadWorker.synchronize())){
			albumTitle = "Generating Cover Display ...";
		} else {
			albumTitle = "No Covers to Display";
		}
		textDisplay.displayText(albumTitle, int(winWidth*cfgTitlePosH), int(winHeight*(1-cfgTitlePosV)), TextDisplay::center, TextDisplay::middle);
	}

	if (cfgShowFps){
		double fps, msPerFrame, longestFrame, minFPS;
		fpsCounter.getFPS(fps, msPerFrame, longestFrame, minFPS);
		pfc::string8 dispStringA;
		pfc::string8 dispStringB;
		dispStringA << "FPS:  " << pfc::format_float(fps, 4, 1);
		dispStringB << " min: " << pfc::format_float(minFPS, 4, 1);
		dispStringA << "  cpu-ms/f: " << pfc::format_float(msPerFrame, 5, 2);
		dispStringB << "   max:     " << pfc::format_float(longestFrame, 5, 2);
		textDisplay.displayBitmapText(dispStringA, winWidth - 250, winHeight - 20);
		textDisplay.displayBitmapText(dispStringB, winWidth - 250, winHeight - 35);
	}
}

void Renderer::drawBg(){
	glClearColor(GetRValue(cfgPanelBg)/255.0f, GetGValue(cfgPanelBg)/255.0f, GetBValue(cfgPanelBg)/255.0f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawFrame()
{
	TRACK_CALL_TEXT("Renderer::drawFrame");
	drawBg();
	drawScene(false);
	drawGui();
}

void Renderer::swapBuffers(){
	TRACK_CALL_TEXT("Renderer::swapBuffers");
	glfwSwapBuffers(appInstance->glfwWindow);
}

void Renderer::drawCovers(bool showTarget){
#ifdef COVER_ALPHA
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.1f);
#endif
	
	if (cfgHighlightWidth == 0)
		showTarget = false;

	if (appInstance->albumCollection->getCount() == 0)
		return;

	float centerOffset = displayPos->getCenteredOffset();
	CollectionPos centerCover = displayPos->getCenteredPos();
	CollectionPos firstCover = displayPos->getOffsetPos(coverPos.getFirstCover() + 1);
	CollectionPos lastCover = displayPos->getOffsetPos(coverPos.getLastCover());
	lastCover++; // getOffsetPos does not return the end() element
	CollectionPos targetCover = appInstance->albumCollection->getTargetPos();

	int offset = appInstance->albumCollection->rank(firstCover) - appInstance->albumCollection->rank(centerCover);
	
	for (CollectionPos p = firstCover; p != lastCover; ++p, ++offset){
		float co = -centerOffset + offset;

		shared_ptr<ImgTexture> tex = texLoader->getLoadedImgTexture(p);
		tex->glBind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// calculate darkening
		float g = 1-(min(1.0f,(abs(co)-2)/5))*0.5f;
		if (abs(co) < 2)
			g = 1;
		/*float g = 1 - (abs(co)-2)*0.2f;
		g = 1 - abs(co)*0.1f;
		g = 1 - abs(zRot)/80;
		g= 1;
		if (g < 0) g = 0;*/
		glColor3f( g, g, g);
		glVectord origin(0, 0.5, 0);

		glQuad coverQuad = coverPos.getCoverQuad(co, tex->getAspect());
		
		glPushName(SELECTION_CENTER + offset);

		glBegin(GL_QUADS);
		if (glFogCoordf) glFogCoordf((GLfloat)coverPos.distanceToMirror(coverQuad.topLeft));
			glTexCoord2f(0.0f, 1.0f); // top left
			glVertex3fv((GLfloat*)&(coverQuad.topLeft.x));

			if (glFogCoordf) glFogCoordf((GLfloat)coverPos.distanceToMirror(coverQuad.topRight));
			glTexCoord2f(1.0f, 1.0f); // top right
			glVertex3fv((GLfloat*)&(coverQuad.topRight.x));

			if (glFogCoordf) glFogCoordf((GLfloat)coverPos.distanceToMirror(coverQuad.bottomRight));
			glTexCoord2f(1.0f, 0.0f); // bottom right
			glVertex3fv((GLfloat*)&(coverQuad.bottomRight.x));

			if (glFogCoordf) glFogCoordf((GLfloat)coverPos.distanceToMirror(coverQuad.bottomLeft));
			glTexCoord2f(0.0f, 0.0f); // bottom left
			glVertex3fv((GLfloat*)&(coverQuad.bottomLeft.x));
		glEnd();
		glPopName();

		if (showTarget){
			if (p == targetCover){
				bool clipPlane = false;
				if (glIsEnabled(GL_CLIP_PLANE0)){
					glDisable(GL_CLIP_PLANE0);
					clipPlane = true;
				}

				showTarget = false;
				
				glColor3f(GetRValue(cfgTitleColor) / 255.0f, GetGValue(cfgTitleColor) / 255.0f, GetBValue(cfgTitleColor) / 255.0f);

				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				glDisable(GL_TEXTURE_2D);

				glLineWidth((GLfloat)cfgHighlightWidth);
				glPolygonOffset(-1.0f, -1.0f);
				glEnable(GL_POLYGON_OFFSET_LINE);

				glEnable(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, (void*) &coverQuad);
				glDrawArrays(GL_QUADS, 0, 4);

				glDisable(GL_POLYGON_OFFSET_LINE);

				glEnable(GL_TEXTURE_2D);


				if (clipPlane)
					glEnable(GL_CLIP_PLANE0);
			}
		}
	}

#ifdef COVER_ALPHA
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
#endif
}