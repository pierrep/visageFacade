// =============================================================================
//
// Copyright (c) 2009-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include <deque>
#include "ofMain.h"
#include "ofxIO.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxFaceTracker.h"


class ofApp: public ofBaseApp
{
public:
    enum
    {
        TXT_HEIGHT = 34
    };

    void setup();
    void draw();
    void update();
    bool checkForFaces(ofImage img);
    void gotMessage(ofMessage msg);

    void onDirectoryWatcherItemAdded(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
    {
        ofLogNotice("ofApp::onDirectoryWatcherItemAdded") << "Added: " << evt.item.path();
        ofSendMessage("Added:    " + evt.item.path());
        imagesToAdd.push(evt.item.path());
        curTime = prevTime = ofGetElapsedTimeMillis();
    }

    void onDirectoryWatcherItemRemoved(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
    {
        ofLogNotice("ofApp::onDirectoryWatcherItemRemoved") << "Removed: " << evt.item.path();
        ofSendMessage("Removed:    " + evt.item.path());
    }

    void onDirectoryWatcherItemModified(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
    {
        ofLogNotice("ofApp::onDirectoryWatcherItemModified") << "Modified: " << evt.item.path();
        //ofSendMessage("Modified:    " + evt.item.path());
    }

    void onDirectoryWatcherItemMovedFrom(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
    {
        ofLogNotice("ofApp::onDirectoryWatcherItemMovedFrom") << "Moved From: " << evt.item.path();
        ofSendMessage("Moved From:    " + evt.item.path());
    }

    void onDirectoryWatcherItemMovedTo(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
    {
        ofLogNotice("ofApp::onDirectoryWatcherItemMovedTo") << "Moved To: " << evt.item.path();
        ofSendMessage("Moved To:    " + evt.item.path());        
    }

    void onDirectoryWatcherError(const Poco::Exception& exc)
    {
        ofLogError("ofApp::onDirectoryWatcherError") << "Error: " << exc.displayText();
    }


    ofxIO::DirectoryWatcherManager watcher;
    ofxIO::FileExtensionFilter fileFilter;
    std::deque<std::string> messages;
    
    queue<string> imagesToAdd;
    string folderToWatch;
    string targetFolder;
    ofxFaceTracker faceTracker;
    
    float curTime;
    float prevTime;
    float waitTime;

};
