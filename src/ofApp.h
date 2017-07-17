#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "FaceAverage.h"
#include "ofxIO.h"
#include "ofxFaceTracker.h"

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
		
        void onDirectoryWatcherItemAdded(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt);
        void onDirectoryWatcherItemRemoved(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt);
        void onDirectoryWatcherItemModified(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt);
        void onDirectoryWatcherItemMovedFrom(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt);
        void onDirectoryWatcherItemMovedTo(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt);
        void onDirectoryWatcherError(const Poco::Exception& exc);


        bool calculatePoints(int i);
        void readImages(int i);
        bool getNextMorph();

        vector<vector<cv::Point2f> > allPoints;
        vector<cv::Mat> cvimages;
        ofImage ofimages[2];
        queue<ofImage> carousel;
        queue<string> newImages;
        int out_width, out_height;
        float aspectRatio;
        ofImage finalImage;
    
        ofxFaceTracker faceTracker;
        string dirname;

        /* images */
        ofImage face1,face2;
    

        ofxIO::DirectoryWatcherManager watcher;
        ofxIO::FileExtensionFilter fileFilter; // an example file filter
};
