#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);

	camera.initGrabber(640, 480);

	faceTracker.setup();
	faceTracker.setRescale(1);

	model3d.loadModel("3d/unicorn/unicorn.3ds", false);
	model3d.enableTextures();
	model3d.enableNormals();
	//ofImage textureImage;
	//textureImage.loadImage("3d/unicorn/redAndBlue.png");
	//model3dTex = textureImage.getTexture();
	//model3d.getTextureForMesh("RedAndBlue.png");

}

//--------------------------------------------------------------
void ofApp::update(){
	camera.update();
	if (camera.isFrameNew()) {
		if (faceTracker.update(toCv(camera))) {
			//faceClassifier.classify(faceTracker);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255);
	camera.draw(0, 0);
	faceTracker.draw();

	ofVec3f orientation = faceTracker.getOrientation() * 90;
	ofVec2f position = faceTracker.getPosition(); // SCREEN COORDINATES
	//faceTracker.getMesh().

	ofPushMatrix();

	ofTranslate(position);
	
	ofDrawCircle(0,0, 20, 20);

	for (int i = 0; i < faceTracker.getObjectPoints().size(); i++)
	{
		ofPoint pointPos = ofPoint(faceTracker.getObjectPoints()[i].x, faceTracker.getObjectPoints()[i].y) * ofPoint(4);
		ofDrawBitmapString(ofToString(i), pointPos);
		//ofDrawBitmapString()
	}

	ofRotateX(-orientation.x);
	ofRotateY(-orientation.y);
	ofRotateZ(orientation.z);

	ofPushMatrix();
	ofRotateX(-90);
	ofScale(0.5, 0.5, 0.5);
	model3d.drawFaces();
	ofPopMatrix();

	ofSetColor(255, 0, 0);
	ofBoxPrimitive box;
	box.setPosition(ofVec3f());
	box.set(100);
	box.drawWireframe();
	//ofDrawRectangle(ofPoint(0, 0), 100, 100);

	ofPopMatrix();



	ofDrawBitmapStringHighlight( "X: " + ofToString(orientation.x) + "\nY: " + ofToString(orientation.y) + "\nZ: " + ofToString(orientation.z), 20,20);



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
