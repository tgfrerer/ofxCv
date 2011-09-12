#include "testApp.h"

void testApp::setup() {
	cam.initGrabber(640, 480);
	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(200);
	unwarped.allocate(150, 100, OF_IMAGE_COLOR);
}

void testApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
		contourFinder.setThreshold(threshold);
		contourFinder.findContours(cam);
		
		int n = contourFinder.size();
		quads.clear();
		quads.resize(n);
		for(int i = 0; i < n; i++) {
			vector<cv::Point> convexHull = contourFinder.getConvexHull(i);			
			// simplify the convex hull until it's only 4 points
			double epsilon = 1;
			while(quads[i].size() > 4 || quads[i].empty()) {
				approxPolyDP(Mat(convexHull), quads[i], epsilon++, true);
			}
			rotate(quads[i].begin(), quads[i].end() - 2, quads[i].end());
			if(quads[i].size() == 4) {
				// quads[i] is full of integer cv::Points because they come from image
				// coordinates. we need to convert to floating point for the unwarp.
				Mat warpMat; // we'll build a floating point Mat
				Mat(quads[i]).convertTo(warpMat, CV_32FC2); // converts int to float
				vector<Point2f> warpPoints = (vector<Point2f>) warpMat; // convert Mat to vector
				unwarpPerspective(cam, unwarped, warpPoints);
				unwarped.update();
			}
		}
	}
}

void testApp::draw() {
	ofSetColor(255);
	cam.draw(0, 0);	
	
	ofSetLineWidth(2);
	contourFinder.draw();
	
	ofNoFill();
	ofSetColor(magentaPrint);
	for(int i = 0; i < quads.size(); i++) {
		toOf(quads[i]).draw();
	}

	ofSetColor(255);
	drawHighlightString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
	drawHighlightString(ofToString((int) threshold) + " threshold", 10, 40);
	
	ofTranslate(8, 75);
	ofFill();
	ofSetColor(0);
	ofRect(-3, -3, 64+6, 64+6);
	ofSetColor(targetColor);
	ofRect(0, 0, 64, 64);
	
	ofSetColor(255);
	unwarped.draw(0, 70);
}

void testApp::mousePressed(int x, int y, int button) {
	targetColor = cam.getPixelsRef().getColor(x, y);
	contourFinder.setTargetColor(targetColor, TRACK_COLOR_HSV);
}

void testApp::keyPressed(int key) {
}