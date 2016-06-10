#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetVerticalSync(true);
	ofDisableArbTex(); // ARB TEXTURE MODE IS IN SCREEN COORDINATES (ALSO, BY DEFAULT)!! LETS USE NORMALIZED COORDINATES

	// GLOBALS
	sceneSurface.allocate(ofGetWidth(), ofGetHeight());
	sceneSurface.begin();
	ofClear(0);
	sceneSurface.end();

	camera.initGrabber(ofGetWidth(), ofGetHeight());

	faceTracker.setup();
	faceClassifier.load("expressions");
	faceTracker.setRescale(0.1);

	gameMode = FACE_TEXTURE;

	// GLOBALS - END

	// 3D MODELS SCENE
	selectedModel = 0;
	ofxAssimpModelLoader woodMask;
	woodMask.loadModel("3d/wood mask/maskWood.dae", false);
	woodMask.enableTextures();
	woodMask.enableNormals();

	ofxAssimpModelLoader demonMask;
	demonMask.loadModel("3d/demon mask/maskDemon.dae", false);
	demonMask.enableTextures();
	demonMask.enableNormals();

	models3D.push_back(woodMask);
	models3D.push_back(demonMask);

	backTribal.loadImage("BeachScene.png");

	// FACE TEXTURE SCENE
	imageB.loadImage("tigerMask_hd.png");
	earL.loadImage("earLeft.png");
	earR.loadImage("earRight.png");
	nose.loadImage("nose.png");

	earRotation = 0;
	oscIncrement = 0.001;

	// CLONE FACE
	/*
	clone.setup(camera.getWidth(), camera.getHeight());

	maskFbo.allocate(camera.getWidth(), camera.getHeight());
	sourceFbo.allocate(camera.getWidth(), camera.getHeight());
	sourceTracker.setup();
	sourceTracker.setIterations(25);
	sourceTracker.setAttempts(4);

	if (sourceTracker.update(toCv(imageB))) {
		sourcePoints = sourceTracker.getImagePoints();
	}
	*/
	// CLONE FACE - END

	createGUI();
}

//--------------------------------------------------------------
void ofApp::update(){
	
	
	// CLONE FACE TRACKER CONFIG VALUES (WITHOUT USING LISTENERS)
	
	faceTracker.setIterations(trackerIterations);
	faceTracker.setClamp(trackerClamp);
	faceTracker.setTolerance(trackerTolerance);
	faceTracker.setAttempts(trackerAttempts);
	//faceTracker.setRescale(trackerReScale);
	

	// BEGIN SCENE SURFACE RENDERING  ------------
	sceneSurface.begin();
	ofBackground(0);

	ofSetColor(255);
	camera.update();

	if (camera.isFrameNew()) {
		if (faceTracker.update(toCv(camera))) {
			if(gameMode == FACE_TEXTURE)faceClassifier.classify(faceTracker);
		}
		
	}


	// DRAW CAM IMAGE
	ofSetColor(255);
	camera.draw(0, 0);

	// DRAW TRACKER FACiAL GIZMO
	ofPushMatrix();
	ofTranslate(0, 0, 20);
	//faceTracker.draw();
	ofPopMatrix();

	// DRAW GRAPHICS FOR SELECTED MODE
	if (gameMode == MESH_3D)
	{
		draw3dModel();
	}
	else if (gameMode == GESTURE)
	{
		drawGesture();
	}
	else if (gameMode == FACE_TEXTURE)
	{
		if (faceTracker.getFound())
		{
			ofSetColor(255);
			drawFaceTexture();
		}

		testTextureBinding();
	}
	else {	
		drawCloneFace();
	}

	sceneSurface.end();

	// END SCENE SURFACE RENDERING  ------------

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	
	sceneSurface.draw(0, 0);

	trackerConfig.draw();
	objectDrawConfig.draw();

	//backTribal.draw(0, 0);
	ofDrawBitmapString("FR: " + ofToString(ofGetFrameRate()), ofGetWidth() - 100, 20);

}

void ofApp::draw3dModel() {

	ofEnableDepthTest();
	ofEnableLighting();
	ofSetGlobalAmbientColor(ofColor(255));
	light.setAreaLight(200,200);
	light.enable();
	light.setDiffuseColor(ofColor(0,200,250));
	light.setPosition(ofGetMouseX(), ofGetMouseY(), 200);

	ofDrawBox(light.getPosition(), 20);

	ofSetColor(255);

	ofVec3f orientation = faceTracker.getOrientation() * 90;
	ofVec2f position = faceTracker.getPosition(); // SCREEN COORDINATES
	//faceTracker.getMesh().

	ofPushMatrix();

	ofTranslate(position + ofVec3f(0, 0, -20));

	ofDrawCircle(0, 0, 20, 20);

	// RENDER FACETRACKER MESH POINTS
	/*
	for (int i = 0; i < faceTracker.getObjectPoints().size(); i++)
	{
		ofPoint pointPos = ofPoint(faceTracker.getObjectPoints()[i].x, faceTracker.getObjectPoints()[i].y) * ofPoint(5);
		ofDrawBitmapString(ofToString(i), pointPos);
	}
	*/

	ofRotateX(-orientation.x);
	ofRotateY(-orientation.y);
	ofRotateZ(orientation.z);

	ofPushMatrix();
	//ofRotateX(-90);
	//ofRotateY(ofMap(ofGetMouseX(), 0, (float)ofGetWindowWidth(), 0, 360));
	ofScale(objectScaleMultiplier, objectScaleMultiplier, objectScaleMultiplier);
	models3D[selectedModel].drawFaces();
	//model3d.drawWireframe();
	ofPopMatrix();

	//ofSetColor(255, 0, 0);
	//ofBoxPrimitive box;
	//box.setPosition(ofVec3f());
	//box.set(100);
	//box.drawWireframe();
	//ofDrawRectangle(ofPoint(0, 0), 100, 100);

	ofPopMatrix();

	ofDrawBitmapStringHighlight("X: " + ofToString(orientation.x) + "\nY: " + ofToString(orientation.y) + "\nZ: " + ofToString(orientation.z), 20, 20);
	ofDrawBitmapString("FR:" + ofToString(ofGetFrameRate()), 20, 100);

	ofDisableLighting();
	ofDisableDepthTest();
	
}

void ofApp::drawGesture() {

	int w = 100, h = 12;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(5, 10);
	int n = faceClassifier.size();
	int primary = faceClassifier.getPrimaryExpression();
	for (int i = 0; i < n; i++) {
		ofSetColor(i == primary ? ofColor::red : ofColor::black);
		ofDrawRectangle(0, 0, w * faceClassifier.getProbability(i) + .5, h);
		ofSetColor(255);
		ofDrawBitmapString(faceClassifier.getDescription(i), 5, 9);
		ofTranslate(0, h + 5);
	}

	if (primary = 0)
	{
		ofSetColor(0,255,0);
		ofDrawRectangle(0, 0, 100, 100);
	}

	ofDrawBitmapString(ofToString(primary),20,20);

	ofPopMatrix();
	ofPopStyle();

	
}

void ofApp::drawFaceTexture() {

	// DEFORMING THE TEXTURE WITH THE MESH´S VERTEX
	/*
	ofMesh faceMesh = faceTracker.getMesh(faceTracker.getImagePoints());
	ofMesh overlayMesh = faceTracker.getMesh(faceTracker.getImagePoints());

	faceTracker.getObjectMesh().drawWireframe();

	//calculateMeshBoundingBox(faceTracker.getImagePoints());
	// GET PSEUDO BOUNDING BOX FROM FACE MESH OF faceTracker
	meshBoundingBox.setSize(faceTracker.getImagePoint(16).x - faceTracker.getImagePoint(0).x, faceTracker.getImagePoint(8).y - faceTracker.getImagePoint(24).y);
	meshBoundingBox.setPosition(faceMesh.getCentroid().x - (meshBoundingBox.getWidth() * 0.5), faceMesh.getCentroid().y - (meshBoundingBox.getHeight() * 0.5));

	ofNoFill();
	ofSetColor(255, 0, 0);
	//ofDrawRectangle(meshBoundingBox);
	//ofDrawCircle(faceMesh.getCentroid(), 10);

	vector<ofPoint> vertexTexCoords;
	for (unsigned int i = 0; i < faceMesh.getVertices().size(); i++)
	{
		float xCoord = ofNormalize(faceMesh.getVertex(i).x, meshBoundingBox.x, meshBoundingBox.x + meshBoundingBox.getWidth());
		float yCoord = ofNormalize(faceMesh.getVertex(i).y, meshBoundingBox.y, meshBoundingBox.y + meshBoundingBox.getHeight());
		ofPoint newPoint = ofPoint(xCoord, yCoord);

		overlayMesh.setTexCoord(i, ofVec2f(xCoord, yCoord));
		//overlayMesh.setTexCoord(i, ofVec2f(faceMesh.getVertex(i).x * numberGrid.getWidth(), faceMesh.getVertex(i).y * numberGrid.getHeight()));
		//overlayMesh.setTexCoord(i, ofVec2f(xCoord, yCoord));
	}
	*/


	/*
	ofPushMatrix();
	ofScale(ofPoint(objectScaleMultiplier));
	for (unsigned int i = 0; i < overlayMesh.getVertices().size(); i++)
	{
		overlayMesh.setVertex(i, overlayMesh.getVertex(i) - faceMesh.getCentroid());
	}
	//ofScale(ofPoint(objectScaleMultiplier));
	ofTranslate(faceMesh.getCentroid());
	*/

	// READING GESTURE
	int w = 100, h = 12;
	ofPushStyle();
	ofFill();
	ofPushMatrix();
	ofTranslate(5, 10);
	int n = faceClassifier.size();
	int primary = faceClassifier.getPrimaryExpression();
	ofDrawBitmapString(ofToString(primary) + faceClassifier.getDescription(primary), 300, 300);

	for (int i = 0; i < n; i++) {
		ofSetColor(i == primary ? ofColor::red : ofColor::black);
		ofDrawRectangle(0, 0, w * faceClassifier.getProbability(i) + .5, h);
		ofSetColor(255);
		ofDrawBitmapString(faceClassifier.getDescription(i), 5, 9);
		ofTranslate(0, h + 5);
	}

	if (primary == 0)
	{
		ofSetColor(0, 255, 0);
		ofDrawRectangle(0, 0, 100, 100);
	}

	ofDrawBitmapString(ofToString(primary), 20, 20);

	ofPopMatrix();
	ofPopStyle();

	if (faceClassifier.getPrimaryExpression() == 0) // 0 = bocaAbierta in expression (yml file)
	{
		oscIncrement = 0.5;
		ofDrawRectangle(400, 400, 50, 50);
	}
	else {
		oscIncrement = 0.001;

	}
	
	earRotation += oscIncrement;
	//earRotation += ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 0.5);
	float normRotation = sin(earRotation);



	drawFaceMeshPoints();

	// DRAW CAT MASK
	ofSetColor(255);

	ofVec3f orientation = faceTracker.getOrientation() * 90;

	ofPushMatrix();
	ofTranslate(faceTracker.getPosition() + ofVec3f(0, 0, -20));
	ofRotateX(-orientation.x);
	ofRotateY(-orientation.y);
	ofRotateZ(orientation.z);
		
		// LEFT EAR
		ofPushMatrix();
		//ofPoint earLPos = (ofPoint(faceTracker.getObjectPoints()[17].x - 20, faceTracker.getObjectPoints()[17].y - 40) * ofPoint(5)) ;
		ofPoint earLPos = ofPoint(faceTracker.getObjectPoints()[17].x, faceTracker.getObjectPoints()[17].y) * ofPoint(trackerScaleMultiplier);
		ofTranslate(earLPos + ofPoint(-20,-40)); // CORRECT THE POSITION A LITTLE BIT
		ofRotateY(ofRadToDeg(ofMap(normRotation,-1,1,0,HALF_PI * 0.5))); // SHAKE EARS
		ofDrawRotationAxes(50, 5, 60);
		ofScale(ofPoint(objectScaleMultiplier));
		earL.draw(-earL.getWidth(),-earL.getHeight()); // DRAW WITH ANCHOR TRANSPORTED TO BOTTOM RIGHT CORNER
		ofPopMatrix();

		// RIGHT EAR
		ofPushMatrix();
		ofPoint earRPos = ofPoint(faceTracker.getObjectPoints()[26].x, faceTracker.getObjectPoints()[26].y) * ofPoint(trackerScaleMultiplier);
		ofTranslate(earRPos + ofPoint(20, -40)); // CORRECT THE POSITION A LITTLE BIT
		ofRotateY(ofRadToDeg(ofMap(normRotation, -1, 1, TWO_PI, TWO_PI - (HALF_PI * 0.5)))); // SHAKE EARS
		ofDrawRotationAxes(50, 5, 60);
		ofScale(ofPoint(objectScaleMultiplier));
		earR.draw(0, -earL.getHeight()); // DRAW WITH ANCHOR TRANSPORTED TO BOTTOM LEFT CORNER
		ofPopMatrix();

		// NOSE
		ofPushMatrix();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofPoint nosePos = (ofPoint(faceTracker.getObjectPoints()[30].x, faceTracker.getObjectPoints()[30].y) * ofPoint(5));
		ofTranslate(nosePos);
		ofScale(ofPoint(objectScaleMultiplier));
		//ofRotateX(orientation.z);
		nose.draw(0, 0);
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofPopMatrix();
		

	ofPopMatrix();
	

	ofSetColor(255);
	imageB.getTextureReference().bind();
	//overlayMesh.draw();
	imageB.getTextureReference().unbind();
	
	//ofPopMatrix();

}

void ofApp::drawCloneFace() {

		if (faceTracker.getFound()) {
			ofMesh camMesh = faceTracker.getImageMesh();
			camMesh.clearTexCoords();
			camMesh.addTexCoords(sourcePoints);

			maskFbo.begin();
			ofClear(0, 255);
			camMesh.draw();
			maskFbo.end();

			sourceFbo.begin();
			ofClear(0, 255);
			imageB.bind();
			camMesh.draw();
			imageB.unbind();
			sourceFbo.end();

			clone.setStrength(16);
			clone.update(sourceFbo.getTextureReference(), camera.getTextureReference(), maskFbo.getTextureReference());
		}
	

}

void ofApp::drawFaceMeshPoints() {
	// RENDER FACETRACKER MESH POINTS

	ofPushMatrix();
	ofTranslate(faceTracker.getPosition() + ofVec3f(0, 0, 20));
	for (int i = 0; i < faceTracker.getObjectPoints().size(); i++)
	{
	ofPoint pointPos = ofPoint(faceTracker.getObjectPoints()[i].x, faceTracker.getObjectPoints()[i].y) * ofPoint(trackerScaleMultiplier);
	ofDrawBitmapString(ofToString(i), pointPos);
	}
	ofPopMatrix();
}

void ofApp::testTextureBinding() {

	ofPlanePrimitive simplePlane;
	simplePlane.set(500, 500);
	simplePlane.setResolution(1, 1);
	simplePlane.setPosition(0,0,0);

	ofMesh newMesh = ofPlanePrimitive().getMesh();
	

	ofPushMatrix();
	ofTranslate(ofGetMouseX(), ofGetMouseY());
	
	imageB.getTextureReference().bind();
	newMesh.draw();
	imageB.getTextureReference().unbind();

	ofSetColor(0, 0, 255);
	ofFill();
	for (unsigned int i = 0; i < newMesh.getVertices().size(); i++)
	{
		//float xCoord = newMesh.getVertex(i).x / newMesh.get;
		//newMesh.setTexCoord(i, ofVec2f());

		ofDrawBitmapString(ofToString(newMesh.getTexCoord(i).x) + ":" + ofToString(newMesh.getTexCoord(i).y), newMesh.getVertex(i).x, newMesh.getVertex(i).y);

	}

	ofPopMatrix();

}

void ofApp::calculateMeshBoundingBox(vector<ofVec3f>& _points)
{
	ofVec3f min(99999, 99999, 99999);
	ofVec3f max(-99999, -99999, -99999);

	for (unsigned int i = 0; i < _points.size(); i++)
	{
		ofVec3f p = _points.at(i);

		min.x = MIN(min.x, p.x);
		min.y = MIN(min.y, p.y);
		//min.z = MIN(min.z, p.z);

		max.x = MAX(max.x, p.x);
		max.y = MAX(max.y, p.y);
		//max.z = MAX(max.z, p.z);
	}

	meshBoundingBox.setSize(max.x - min.x, max.y - min.y);
	//setSize(max - min);
	//setPosition(min.getInterpolated(max, 0.5f));
}


void ofApp::createGUI() {

	objectDrawConfig.setup("OBJECT DRAWING CONFIG");
	objectDrawConfig.setPosition(10, 300);
	objectDrawConfig.add(objectScaleMultiplier.set("OBJECT SCALE", 1.0, 0.1, 4.0));

	trackerConfig.setup("TRACKER CONFIG");
	trackerConfig.setPosition(10, 100);
	trackerConfig.add(trackerIterations.set("ITERATIONS", 10, 1, 25));
	trackerConfig.add(trackerClamp.set("CLAMP", 3, 0, 4));
	trackerConfig.add(trackerTolerance.set("TOLERANCE", 0.01, 0.01, 1));
	trackerConfig.add(trackerAttempts.set("ATTEMPTS", 1, 1, 4));
	trackerConfig.add(trackerScaleMultiplier.set("TRACKER SCALING", 5, 0, 8));
	//trackerConfig.add(trackerReScale.set("RE-SCALE", 1, 1, 1));

	trackerConfig.loadFromFile("settings.xml");

	
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == '1')
	{
		gameMode = MESH_3D;
	}
	if (key == '2')
	{
		gameMode = GESTURE;
	}
	if (key == '3')
	{
		gameMode = FACE_TEXTURE;
	}
	if (key == '4')
	{
		gameMode = CLONE_FACE;
	}

	if (key == 'n') {
		selectedModel = (selectedModel + 1) % models3D.size();
	}


	if (key == 'r') {
		faceTracker.reset();
		faceClassifier.reset();
	}
	if (key == 'e') {
		faceClassifier.addExpression();
	}
	if (key == 'a') {
		faceClassifier.addSample(faceTracker);
	}
	if (key == 's') {
		faceClassifier.save("expressions");
	}
	if (key == 'l') {
		faceClassifier.load("expressions");
	}

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
