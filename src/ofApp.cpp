#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("Visage Facade");
    
    font.load("fonts/Gotham-Medium.otf", 20);
    smallfont.load("fonts/Gotham-Medium.otf", 14);
    
    port = 12345;
    hostname = "localhost";
    oscSender.setup(hostname, port);
    
    aspectRatio = 1.21875f;
    out_width = 720;
    out_height = out_width * aspectRatio;
    small_width = 400;
    small_height = 400;

    faceTracker.setup();
    faceTracker.setIterations(15);
    faceTracker.setAttempts(5);
    faceTracker.setTolerance(1);
    
    setupSyphon();
    
    /* directory watcher */
    watcher.registerAllEvents(this);
    std::string folderToWatch = ofToDataPath("faces", true);
    bool listExistingItemsOnStart = true;
    bool sortAlphaNumeric = false;
    fileFilter.addExtension(".jpg");
    fileFilter.addExtension(".png");    
    watcher.addPath(folderToWatch, listExistingItemsOnStart, sortAlphaNumeric, &fileFilter);

    dirname = "faces";
    
    ofDirectory dir;
    dir.allowExt("jpg");
    dir.allowExt("png");
    dir.listDir(dirname);
    //dir.sort();

    for(int i = 0; i < dir.size();i++)
    {
        ofImage img;
        img.load(dir.getPath(i));
        img.update();
        carousel.push_back(img);
    }

    getNextMorph();

}

//--------------------------------------------------------------
void ofApp::update(){

    if(newImages.size() > 0 ){
        ofImage img;
        img.load(newImages.front());
        img.update();
        carousel.push_back(img);
        newImages.pop();
        ofLogNotice() << "Added new image";
    }
}

//--------------------------------------------------------------
void ofApp::draw(){


    if (ofimages[0].isAllocated()) {
        ofimages[0].draw(ofGetWidth()/2,10,small_width,small_height);
        font.drawString("LEFT PORTRAIT", ofGetWidth()/2,small_height+50);
    }
    if (ofimages[1].isAllocated()) {
        ofimages[1].draw(ofGetWidth()/2+small_width+10,10,small_width,small_height);
        font.drawString("RIGHT PORTRAIT", ofGetWidth()/2+small_width+10,small_height+50);
    }

    if (finalImage.isAllocated()) {
    finalImage.draw(10,10);
    }

    /* draw carousel */
    int i = 0;
    for (std::list<ofImage>::iterator it = carousel.begin(); it != carousel.end(); it++) {
        (*it).draw(ofGetWidth()/2+(i%16)*50, 500+(i/16)*50,50,50);
        i++;;
    }
    
    smallfont.drawString("Port: "+ofToString(port)+ "  Host: "+hostname+"  /newmorph",ofGetWidth()/2, ofGetHeight()-30);
    
    updateSyphonImages();
 
}

//--------------------------------------------------------------
void ofApp::readImages(int i)
{

    cv::Mat img = ofxCv::toCv(ofimages[i]).clone();

    img.convertTo(img, CV_32FC3, 1.0);

    if(!img.data)
    {
        cout << "image not read properly" << endl;
    }
    else
    {
        cvimages.push_back(img);
    }

}

//--------------------------------------------------------------
bool ofApp::calculatePoints(int i)
{
        
    faceTracker.reset();
    faceTracker.update(ofxCv::toCv(ofimages[i]));

    int faceWidth = ofimages[i].getWidth();
    int faceHeight = ofimages[i].getHeight();
    if(!faceTracker.getFound()) {
        ofLogNotice() << "FaceTracker didn't find a face! No points.";
        return false;
    } else {
        ofPolyline p1 = faceTracker.getImageFeature(ofxFaceTracker::LEFT_EYE);
        ofPolyline p2 = faceTracker.getImageFeature(ofxFaceTracker::RIGHT_EYE);

        ofPoint leftEye = p1.getClosestPoint(ofPoint(0,faceHeight/2.0f));
        ofPoint rightEye = p2.getClosestPoint(ofPoint(faceWidth,faceHeight/2.0f));
       // ofLogNotice() << "Left Eye " << i+1 << "  x:" << leftEye.x << " y:" << leftEye.y;
       // ofLogNotice() << "Right Eye " << i+1 << "  x:" << rightEye.x << " y:" << rightEye.y;

        ofMesh mesh_face;
        mesh_face = faceTracker.getImageMesh();

        vector<cv::Point2f> points;

        for(int i = 0; i < mesh_face.getNumVertices(); i++)
        {
            points.push_back(cv::Point2f((float) mesh_face.getVertex(i).x, (float) mesh_face.getVertex(i).y));
        }
        points.push_back(cv::Point2f(leftEye.x, leftEye.y));
        points.push_back(cv::Point2f(rightEye.x, rightEye.y));

        allPoints.push_back(points);
    }
    return true;
}

//--------------------------------------------------------------
bool ofApp::getNextMorph()
{
    ofimages[1] = carousel.front();
    ofimages[1].update();
    carousel.push_back(carousel.front());
    carousel.pop_front();

    ofimages[0] = carousel.front();
    ofimages[0].update();

    finalImage.clear();

    bool fail1 = false;
    bool fail2 = false;

    readImages(0);
    fail1 = calculatePoints(0);

    readImages(1);
    fail2 = calculatePoints(1);

    if(!fail1 || !fail2) {
        ofLogError() << "Failed to detect a face";
        cvimages.clear();
        allPoints.clear();
        return false;
    }
    finalImage = processImages(cvimages,allPoints,out_width,out_height);

    cvimages.clear();
    allPoints.clear();
    
    /* Notify of new morph */
    ofxOscMessage m;
    m.setAddress("/newmorph");
    m.addBoolArg(true);
    oscSender.sendMessage(m, false);

    return true;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == ' ') {
        getNextMorph();
    }
    if(key == 'm') {

    }
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherItemAdded(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
{
    ofLogNotice() << "Added:    " << evt.item.path();
    newImages.push(evt.item.path());
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherItemRemoved(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
{
    ofLogNotice() << "Removed:    " << evt.item.path();
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherItemModified(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
{
    ofLogNotice() << "Modified:    " << evt.item.path();
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherItemMovedFrom(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
{
    ofLogNotice("ofApp::onDirectoryWatcherItemMovedFrom") << "Moved From: " << evt.item.path();
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherItemMovedTo(const ofxIO::DirectoryWatcherManager::DirectoryEvent& evt)
{
    ofLogNotice("ofApp::onDirectoryWatcherItemMovedTo") << "Moved To: " << evt.item.path();
}

//--------------------------------------------------------------
void ofApp::onDirectoryWatcherError(const Poco::Exception& exc)
{
    ofLogError("ofApp::onDirectoryWatcherError") << "Error: " << exc.displayText();
}

//--------------------------------------------------------------
void ofApp::updateSyphonImages()
{
#ifdef TARGET_OSX
    if (ofimages[0].isAllocated()) leftPortrait.publishTexture(&(ofimages[0].getTexture()));
    
    if (ofimages[1].isAllocated()) rightPortrait.publishTexture(&(ofimages[1].getTexture()));
    
    if (finalImage.isAllocated()) mainPortrait.publishTexture(&(finalImage.getTexture()));
#endif
}

//--------------------------------------------------------------
void ofApp::setupSyphon()
{
#ifdef TARGET_OSX
    leftPortrait.setName("Left Portrait");
    rightPortrait.setName("Right Portrait");
    mainPortrait.setName("Main Portrait");
#endif
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
