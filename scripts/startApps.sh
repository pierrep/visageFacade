#!/bin/bash

cd ~/Desktop/
./startImageWatcher.sh &

sleep 1 

cd ~/Desktop/
./getImages.sh &

cd ~/Desktop/visageFacade/visageFacade.app/Contents/MacOS/
./visageFacade &

