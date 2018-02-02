#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(30);
	ofBackground(0);
	ofSetWindowTitle("Insta");

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	//this->box2d.createBounds();
	this->box2d.setFPS(30);
	//this->box2d.registerGrabbing();

	this->cap.open(1);
	this->image.allocate(this->cap.get(CV_CAP_PROP_FRAME_WIDTH), this->cap.get(CV_CAP_PROP_FRAME_HEIGHT), OF_IMAGE_COLOR);
	this->frame = cv::Mat(this->image.getHeight(), this->image.getWidth(), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
	
	this->fbo.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {

	cv::Mat src;
	this->cap >> src;
	cv::flip(src, src, 1);
	cv::cvtColor(src, this->frame, cv::COLOR_BGR2RGB);


	float span = 15;
	if (ofGetFrameNum() > 10) {

		cv::Mat gap = this->pre_frame - src;
		vector<ofVec2f> gap_points;

		for (int y = 0; y < gap.rows; y += span) {

			cv::Vec3b* value = gap.ptr<cv::Vec3b>(y, 0);
			for (int x = 0; x < gap.cols; x += span) {

				cv::Vec3b v = value[x];
				if ((v[0] + v[1] + v[2]) > 32) {
					 
					shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
					circle.get()->setPhysics(3.0, 0.5, 0.1);
					circle.get()->setup(this->box2d.getWorld(), x + ofGetWidth() / 2 - this->image.getWidth() / 2 + ofRandom(-span / 2, span / 2), y + 30 + ofRandom(-span / 2, span / 2), span / 2);
					this->circles.push_back(circle);

					ofColor c;
					c.setHsb(ofRandom(255), 220, 220);
					this->circles_color.push_back(c);

				}
			}
		}
	}
	this->pre_frame = src;
	this->image.update();

	this->fbo.begin();
	ofClear(0);

	ofSetColor(255);
	this->image.draw(ofGetWidth() / 2 - this->image.getWidth() / 2, 30);

	for (int i = this->circles.size() - 1; i >= 0; i--) {
		
		if (this->circles[i].get()->getPosition().y > ofGetHeight()) {

			this->circles[i].get()->destroy();
			this->circles.erase(this->circles.begin() + i);
			this->circles_color.erase(this->circles_color.begin() + i);
		}
		else {

			ofSetColor(this->circles_color[i]);
			ofDrawCircle(this->circles[i].get()->getPosition(), this->circles[i].get()->getRadius());
		}
	}

	this->fbo.end();

	this->box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->fbo.draw(0, 0);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}