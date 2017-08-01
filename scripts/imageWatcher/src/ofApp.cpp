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


#include "ofApp.h"


void ofApp::setup()
{
    targetFolder = "/Users/grimus/Dropbox/GSPF2017/faces";
    
    watcher.registerAllEvents(this);
    folderToWatch = ofToDataPath("/Volumes/Data/code/instagram", false);
    bool listExistingItemsOnStart = false;
    bool sortAlphaNumeric = false;
    fileFilter.addExtension("jpg");
    fileFilter.addExtension("png");
    watcher.addPath(folderToWatch, listExistingItemsOnStart, sortAlphaNumeric, &fileFilter);
    
    faceTracker.setup();
    faceTracker.setIterations(15);
    faceTracker.setAttempts(5);
    faceTracker.setTolerance(1);
    
    curTime = prevTime = ofGetElapsedTimeMillis();
    waitTime = 1000;

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);
    ofFill();

    int y = TXT_HEIGHT;

    for (std::size_t i = 0; i < messages.size(); ++i)
    {
        ofSetColor(ofMap(i, 0, messages.size(), 255, 90));
        ofDrawBitmapString(messages[i], 10, y);
        y += TXT_HEIGHT;
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    curTime = ofGetElapsedTimeMillis();
    
    if(imagesToAdd.size() > 0 ){
        ofImage img;
        img.load(imagesToAdd.front());
        img.update();
        
        if(checkForFaces(img)) {
        
            if(curTime - prevTime > waitTime) {
                ofFile fileToRead(imagesToAdd.front());
                bool result = fileToRead.copyTo(targetFolder,false,false);
                if(result) {
                    ofLogNotice() << imagesToAdd.front() << " copied to target folder";
                    imagesToAdd.pop();
                    curTime = prevTime = ofGetElapsedTimeMillis();
                }
                else {
                    //ofLogError() << "Failed to copy " << imagesToAdd.front() << " to target folder";
                    imagesToAdd.pop();
                    curTime = prevTime = ofGetElapsedTimeMillis();
                }
            }
        } else {
            ofLogWarning() << "No face found in " << imagesToAdd.front();
            imagesToAdd.pop();
        }
        
    }
}

//--------------------------------------------------------------
bool ofApp::checkForFaces(ofImage img)
{
    faceTracker.reset();
    faceTracker.update(ofxCv::toCv(img));
    
    if(!faceTracker.getFound()) {
        ofLogNotice() << "FaceTracker didn't find a face! No points.";
        return false;
    }
    return true;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
    int height = ofGetHeight();

    std::size_t numLines = static_cast<std::size_t>(height / TXT_HEIGHT);

    messages.push_front(msg.message);

    while (messages.size() > numLines)
    {
        messages.pop_back();
    }
}
