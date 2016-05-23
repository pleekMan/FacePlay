#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxAssimpModelLoader.h"

enum GameMode
{
	MESH_3D,
	GESTURE,
	FACE_TEXTURE	
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofVideoGrabber camera;
		ofxFaceTracker faceTracker;
		ExpressionClassifier faceClassifier;

		ofxAssimpModelLoader model3d;
		ofTexture model3dTex;
		ofLight light;

		ofFbo sceneSurface;

		GameMode gameMode;
		void draw3dModel();
		void drawGesture();
		void drawFaceTexture();
		void calculateMeshBoundingBox(vector<ofVec3f>& _points);

		ofImage numberGrid;
		ofRectangle meshBoundingBox;
		void testTextureBinding();
};
