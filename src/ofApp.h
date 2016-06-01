#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "Clone.h"

enum GameMode
{
	MESH_3D,
	GESTURE,
	FACE_TEXTURE,
	CLONE_FACE
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
		
		// COMMON
		ofVideoGrabber camera;
		ofxFaceTrackerThreaded faceTracker;
		ofFbo sceneSurface;
		ofxPanel objectDrawConfig;

		// MESH_3D
		ofxAssimpModelLoader model3d;
		ofTexture model3dTex;
		ofLight light;
		ofParameter<float> objectScaleMultiplier;
		ofImage backTribal;

		// GESTURES
		ExpressionClassifier faceClassifier;

		GameMode gameMode;
		void draw3dModel();
		void drawGesture();
		void drawFaceTexture();
		void drawCloneFace();
		void calculateMeshBoundingBox(vector<ofVec3f>& _points);

		// FACE TEXTURE
		ofImage imageB;
		ofRectangle meshBoundingBox;
		void testTextureBinding();

		// CLONE FACE
		
		Clone clone;
		ofFbo sourceFbo;
		ofFbo maskFbo;
		ofxFaceTracker sourceTracker;
		vector<ofVec2f> sourcePoints;

		ofxPanel trackerConfig;
		ofParameter<int> trackerIterations;
		ofParameter<int> trackerClamp;
		ofParameter<float> trackerTolerance;
		ofParameter<int> trackerAttempts;
		//ofParameter<int> trackerReScale;
		
		
		void createGUI();
		


};
