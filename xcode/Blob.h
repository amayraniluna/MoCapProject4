//
//  Blob.h
//  BlobTrackingLab
//
//  Programmer: Courtney Brown
//  Created: Aug 2018, Modified Oct. 2020
//  Desc: Represents a blob and calculates blob features given a stream of keypoints.
//

#ifndef Blob_h
#define Blob_h

#include <sstream>

class Blob
{
protected:
    std::vector<cv::KeyPoint> keyPoints; //circular buffer of keypoints that is bufferSize
    int b_id; // the blob id
    int lastIndex; //where it was last frame
    int bufferSize; //how big is our buffer of past keypoints?
    cv::KeyPoint curAvg; //current windowed avg.
    int currX;
    int currY;
public:
    Blob(cv::KeyPoint pt, int _id)
    {
        update(pt);
        b_id = _id;
        lastIndex = -1;
        bufferSize = 10;
    }
    
    int getBlobID()
    {
        return b_id;
    }
    
    int getCurrX()
    {
//        float lastInd = keyPoints.size();
//        currX = keyPoints[lastInd-1].pt.x;
        currX = curAvg.pt.x;
        return currX;
    }
    
    int getCurrY()
    {
//        float lastInd = keyPoints.size();
//        currY = keyPoints[lastInd-1].pt.y;
        currY = curAvg.pt.y;
        return currY;
    }
    
    //get the average, yo
    cv::KeyPoint avg()
    {
        return curAvg;
    }
    
    //updates all the values, incl. avg.
    void update(cv::KeyPoint pt)
    {
        keyPoints.push_back(pt);
        while( keyPoints.size() > bufferSize )
        {
            keyPoints.erase(keyPoints.begin());
        }
        calc_avg();
    }
    
    //blob has existed long enough to produce a velocity value, if you were to implement it.
    bool hasExisted()
    {
        return keyPoints.size() >= 2;
    }
    
    
    //calculate the average of keypoints in buffer for x, y, size
    void calc_avg()
    {
        cv::KeyPoint avg;
        
        avg.pt.x = 0;
        avg.pt.y = 0;
        avg.size = 0;
        
        //avoid dividing by 0
        if( keyPoints.size() <= 0) return ;

        for(int i=0; i<keyPoints.size(); i++)
        {
            avg.pt.x += keyPoints[i].pt.x;
            avg.pt.y += keyPoints[i].pt.y;
            avg.size += keyPoints[i].size;
        }
        
        avg.pt.x /= (float) keyPoints.size();
        avg.pt.y /= (float) keyPoints.size();
        avg.size /= (float) keyPoints.size();
        
        curAvg = avg;
    }
    
    
    
    //draw the blob on the screen with its id
    void draw()
    {
        ci::gl::color( 0.65, 0.5, 0.65, 0.5 );
        ci::gl::drawSolidCircle( ci::fromOcv( keyPoints[0].pt ), keyPoints[0].size );
        ci::gl::color( 1, 1, 1, 1);
        ci::gl::draw(drawText(),  ci::fromOcv( keyPoints[0].pt ) );
    };
    
    //see the method name -- just draws the id # as a label for the blob
    ci::gl::Texture2dRef drawText()
    {
        ci::TextLayout simple;
        simple.setColor( ci::Color( 1, 0, 0.1f ) );
        
        std::stringstream str; str << b_id;
        simple.addLine( str.str() );
        return ci::gl::Texture2d::create( simple.render( true, false ) );
    };
    
    //This is not used anymore for the current tracking algorithm
//    int getLastIndex()
//    {
//        return lastIndex;
//    }
    
};






#endif /* Blob_h */

