# OpenFlexure-microscopy-enhancement
This project provides real-time image processing capabilities for the Raspberry Pi based OpenFlexure microscope platform for image enhancement including contrast and sharpness adjustment, edge detection/enhancement, and classification using k-means clustering.

<img src="https://openflexure.org/assets/ofm-photos/v7_side_view_crop.jpg" width="200" />


# dev instructions
## build openflexureplus
to build, go to main directory and in a terminal type 
```
bash build.sh
```
Then to run enter
```
bin/OpenFlexure
```

```build.sh``` has options:
```
Syntax: build.sh [-h|r|i|c]"
   options:"
   -h     Print this Help."
   -r     Build and run."
   -i     Build and install executeable onto path /usr/bin. Requires sudo."
   -c     Clear CMake Cache and build."
```

Example: If you want to build and run straight away enter:
```
bash build.sh -r 
```
or to build, remove the cmake cache, install and run straight away:
```
bash build.sh -r -c -i
```
If the -i option is used, OpenFlexurePlus can be executed from anywhere by typing 
```
OpenFlexure
```

### Note
Make sure you have opencv_build and opencv_src inside the main directory. install instructions or script to come. 

## Google Test

### Installation
It is necessary to also install Google Test, as unit tests are run when building.
To install, run the following commands:
```
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```
These commands to be added to setup script.

### Adding Tests

Unit tests should be written for new classes. The existing test files (```gtest_camera.h``` and ```gtest_edgedetect.h```) in the test folder can be used as examples of how tests can be implemented.

Tests are written inside test functions which take the form:
```
TEST(testSuiteName, testName){
   //test in here
}
```
There can be multiple different test functions within a test header file. These likely will belong to the same test suite but each have distinct test names. 

The actual checks carried out within the tests are called "assertions" and take two general forms: ```EXPECT``` which generate non-fatal failures, and ```ASSERT``` which generate fatal failures. For instance, the assertion
```
EXPECT_EQ(variable1, variable2)
```
can be used to check that two variables are equal as expected, and generates a non-fatal failure if untrue. Other possible assertions can be found in the Google Test documentation.

This gtest header file should include ```<gtest/gtest.h>``` as well as the header of the class under test. If testing an element of the callback chain structure, an "end tester" tool ```gtest_endtest.h``` has been written to get the returned output frame. An example implementation of this can be found in ```gtest_edgedetect.h```.

Once a test has been written, there are several more steps to get it integrated into the unit test framework. If it has not already been done, a library for the class being tested must be added to the ```CMakeLists.txt``` file in src. An example of this for the ```camera``` class is shown below.
```
add_library(camera camera.cpp camera.h)
target_link_libraries(camera ${OpenCV_LIBS} Threads::Threads)
```
Next, the ```target_link_libraries()``` line in ```test/CMakeLists.txt``` should be updated to append the newly created library in ```src/CMakeLists.txt``` to the end of the list. 

Finally, ```main.cpp``` in test should be updated to include the new test header file.

To run the tests, re-run ```build.sh``` as the unit tests are automatically run during building.

## GUI build
To update the GUI, copy ONLY the gui.ui file generated in QTCreator into  main directory.
Then in a terminal enter
```
bash rebuildGUI.sh
```
before building the whole program as described above.
Edits to connections and signals should be made inside src/gui.cpp