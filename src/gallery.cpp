#include "gallery.h"
/**
* Initialises gallery file paths, directories, and indexing of the capture counter which is preserved between runs of cellUview
**/
Gallery::Gallery(){

//---- find or create gallery directory----
    pathname = getenv("HOME");
    pathname += + "/cellUviewGallery/"; 
    flatFieldPath = pathname + ".FlatFieldGallery";
    //updates index to find highest existing file with matching name to avoid overwriting
    initialiseDirectory(pathname, "cellUview Gallery");
    initialiseDirectory(flatFieldPath, "Flat field capture gallery");
    updateIndex();
    //getCaptures(false);
    
}

/**
* Creates a directory if it does not already exist.
* @param path The file path with which to initialise at
* @param description String describing directory purpose to be used in terminal output
**/
void Gallery::initialiseDirectory(std::string path, std::string description){
        //if it doesn't exist
    if ((dir = opendir(path.c_str())) == NULL){
        //try to make the directory
        if (mkdir(path.c_str(), S_IRWXU) == -1){
            //if failed:
            std::cerr << "Error :  " << std::strerror(errno) << std::endl;
            std::cout << description << " directory not found/created";
            //ADD. disable button if failed
            pathname = ""; 
            return;
        }
        else{//if gallery succesfully made
            std::cout << description << " directory created at " + path << std::endl;
        }
    }else{//if gallery already exists
        std::cout << description << " directory found at " + path << std::endl;
        closedir(dir);
    }
    
    return;

    //updates index to find highest existing file with matching name to avoid overwriting
}

/**
* Captures a frame and saves into the appropriate gallery directory
* @param capFrame Frame structure containing metadata and cv::Mat of image
* @param updateFlatField optional and if true captures into the flatfield directory, defaults to false and captures into main gallery directory
* @param flatFieldCounter Adds index of the number of flat field counter as this will be called sequentially
**/
void Gallery::captureFrame(frame capFrame, bool updateFlatField, int flatFieldCounter){
    if (pathname == ""){ //error checking, if pathname not set then skip
        return;
    }
    //add ability to set custom string before number
    if (updateFlatField == true){ //flatfield capture

        captureFname = pathname + "/.FlatFieldGallery/flatField" + std::to_string(flatFieldCounter) + ".jpg";
        std::string flatFieldPath = captureFname;

        //save image
        img = capFrame.image;
        cv::imwrite(captureFname, img); 
            
    }
    else{ //default capture
        //build output name string 
        captureFname = pathname + imgName + std::to_string(captureImgCounter) +".jpg";

        //save image
        img = capFrame.image;
        cv::imwrite(captureFname, img); 

        captureImgCounter++;
        //add metadata into captured frame
        writeMetadata(capFrame, captureFname);
        
        std::cout<<"Capturing"<<std::endl;   

    }   
    
} 

/**
* Inserts metadata into saved capture using cpp_exiftool.
* @param f Frame structure containing metadata and cv::Mat of image
* @param captureFname Name of file to have metadata inserted into
**/
void Gallery::writeMetadata(frame f, std::string captureFname){
    MetadataToWrite = f.encodeMetadata();
    et->SetNewValue("XMP:Description", MetadataToWrite.c_str());
    et->WriteInfo(captureFname.c_str());
    int result = et->Complete();
    if (result<=0) std::cerr << "Error writing metadata" << std::endl;

    //remove original image left over from exiftool library
    std::string origCap = captureFname + "_original";
    std::remove(origCap.c_str());
}

/**
* This reads back the saved metadata in a capture using cpp_exiftool
* @param fname Frame structure containing metadata and cv::Mat of image
* @returns restoredParams a map of the paramater and value as defined in frame.h 
**/
std::map<std::string, std::string> Gallery::getMetadata(std::string fname){
    if (fname == ""){ //default to most recent capture
        fname = captureFname;
    }

    receivedMetadata="";
    TagInfo *info = et->ImageInfo(fname.c_str());
    if (info){
        for (TagInfo *i=info; i; i=i->next){
            tagName = i->name;
            if (tagName == "Description"){
                receivedMetadata = i->value;

                //build back into param map matching frame
                std::string pair;
                std::string item;
                std::istringstream iss(receivedMetadata);
                while (std::getline(iss, pair, *metaDataPairDelim)){


                    std::vector<std::string> rec ={};
                    std::istringstream iss2(pair);
                    while (std::getline(iss2, item, *metaDataItemDelim)){
                        rec.push_back(item);
                    }
                    restoredParams[rec[0]] = rec[1];

                    rec.clear();
                    
                }
                break;
            }
        }

        delete info;

    }else if (et->LastComplete()<=0){
        std::cerr << "Metadata read error" << std::endl;
    }else{
        std::cout << "No metadata to read" << std::endl;
    }

    return restoredParams;
}

/**
* Iterates through items in gallery directory to find the highest index of captureXYZ.jpg where XYZ is an integer counter of arbitrary length. 
* this avoids captures being overwritten on following runs of cellUview
* @param fname Frame structure containing metadata and cv::Mat of image
* @returns restoredParams a map of the paramater and value as defined in frame.h 
**/
void Gallery::updateIndex(){
//(re)open already existing/newly created directory 
    //to find if files with current name already exis.=t
    //to avoid overwriting the files
    if ((dir = opendir(pathname.c_str())) != NULL) {
        
        //iterate over all entries in the gallery directory
        while ((ent = readdir(dir)) != NULL) {

            //get name of file
            existingCaptureFname = ent->d_name;

            //if file matches the set filename template
            if (existingCaptureFname.substr(0, imgName.length()) == imgName){
   
                //get number of digits of file index in filename
                indexLen = existingCaptureFname.length() - imgName.length() - 4; //4 for .jpg chars

                //get file index
                index = std::stoi(existingCaptureFname.substr(imgName.length(), indexLen));
                 
                //if index is higher than last highest found
                if (index > lastHighestIndex){
                    captureImgCounter = index + 1;
                    lastHighestIndex = index;
                }
            } 
        }
    closedir(dir);
    galleryDisplayIndex = captureImgCounter -4;

    }  

}

/**
 * Loads saved captures and notes to form list of pairs to provide to gui for display
* Keeps track of gallery location, checking at extremes to not load non existent captures.
* @param directionIsNext if true gets the next four saved captures, otherwise the previous.  
* @returns stringCapPairs a list of pairs of strings (labels) and cv::Mat (image)
**/
std::list<std::pair<std::string, cv::Mat>>  Gallery::getCaptures(bool directionIsNext){
    //adjust increment of top left index to display
    if (directionIsNext == true){
        galleryDisplayIndex += 4;
    }else{
        galleryDisplayIndex -= 4;
    }

    if (galleryDisplayIndex < 0){
        galleryDisplayIndex = 0;
    }else if (galleryDisplayIndex > captureImgCounter - 4){
        galleryDisplayIndex = captureImgCounter - 4;
    }


    std::list<std::pair<std::string, cv::Mat>>  stringCapPairs;
    std::string capturePathName;
    cv::Mat captureMat;
    std::string displayString;
    int panelIndex;
    std::map<std::string, std::string> metadataGalleryLabel;
    std::string note;
    galleryDisplayFname.clear();
    for (int i = 0; i<4; i++){
        panelIndex = galleryDisplayIndex + i;
        try{

            capturePathName = pathname + imgName + std::to_string(panelIndex) + ".jpg";
            captureMat = cv::imread(capturePathName);
            metadataGalleryLabel= getMetadata(capturePathName);
            note = metadataGalleryLabel["note"];
            displayString = std::to_string(panelIndex) + ":   " + note; 
            galleryDisplayFname.push_back(capturePathName);


        }catch(...){
            displayString = "";
            cv::Mat emptyMat;
            captureMat = emptyMat;
        }
        stringCapPairs.push_back({displayString, captureMat});
    
    }
    return stringCapPairs;
}

std::string Gallery::getGalleryDisplayFname(int pos){
    return galleryDisplayFname[pos];
}

bool Gallery::galleryAtEnd(){
    return (galleryDisplayIndex + 5 == captureImgCounter);
}


