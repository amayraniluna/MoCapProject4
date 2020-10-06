/*
 *
 Programmer: Amayrani Luna
 Date: 9/19/20
 Notes: Modified from Cinder OpenCV sample on optical flow
 Purpose/Description:
 This program draws the optical flow of live video using feature detection.
 
 Uses:
 cv::goodFeaturesToTrack - in this case, the features that are "good to track" are simply corners/edges.
 
 cv::calcOpticalFlowPyrLK - creates the mFeatureStatuses array which is a map from current features (mFeatures) into the previous features (mPreviousFeatures).
 
 Output/Drawing:
 Previous Features are 50% transparent red (drawn first)
 Current Features are 50% transparent blue
 The optical flow or path from previous to current is drawn in green.
 *
 */

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/video.hpp>
#include "CinderOpenCV.h"
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h" //add - needed for capture
#include "cinder/Log.h" //add - needed to log errors

#include "Osc.h" //add to send OSC
#include <math.h>

#include "Squares.h"

#define SAMPLE_WINDOW_MOD 300 //how often we find new features -- that is 1/300 frames we will find some features
#define MAX_FEATURES 300 //The maximum number of features to track. Experiment with changing this number


using namespace cinder;
using namespace ci::app;
using namespace std;

class FeatureTrackingApp : public App {
  public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void keyDown( KeyEvent event) override;
protected:
    CaptureRef                 mCapture; //uses video camera to capture frames of data.
    gl::TextureRef             mTexture; //the current frame of visual data in OpenGL format.
    
    //for optical flow
    vector<cv::Point2f>        mPrevFeatures, //the features that we found in the last frame
                               mFeatures; //the feature that we found in the current frame
    cv::Mat                    mPrevFrame; //the last frame
    ci::SurfaceRef             mSurface; //the current frame of visual data in CInder format.
    vector<uint8_t>            mFeatureStatuses; //a map of previous features to current features
    SquaresFeatures            mFTrackingSquare;
    bool                       mDoBackgroundSubtraction = false;
    bool                       mFrameDiffOrOpFlow = false;
    cv::Ptr<cv::BackgroundSubtractor> mBackgroundSubtract;
    cv::Mat test;
    
    //for frame differencing
    SquaresFrameDiff           mFDiffSquare;
    cv::Mat                    mFrameDifference;
    
    void findOpticalFlow(); //finds the optical flow -- the visual or apparent motion of features (or persons or things or what you can detect/measure) through video
    void frameDifference(cv::Mat &outputImg);
};



void FeatureTrackingApp::setup()
{
    //set up our camera
    try {
        mCapture = Capture::create(640, 480); //first default camera
        mCapture->start();
    }
    catch( ci::Exception &exc)
    {
        CI_LOG_EXCEPTION( "Failed to init capture ", exc ); //oh no!!
    }
    
    mPrevFrame.data = NULL; //initialize our previous frame to null since in the beginning... there no previous frames!
    mFrameDifference.data = NULL;
    
    //create the background subtractor (it can be any type -- using KNN just for the example)
     mBackgroundSubtract = cv::createBackgroundSubtractorKNN();
}


//maybe you will add mouse functionality!
void FeatureTrackingApp::mouseDown( MouseEvent event )
{
}


void FeatureTrackingApp::update()
{
    //update the current frame from camera
    if(mCapture && mCapture->checkNewFrame()) //is there a new frame???? (& did camera get created?)
    {
        mSurface = mCapture->getSurface(); //get the current frame and put in the Cinder datatype.
        
        //translate the cinder frame (Surface) into the OpenGL one (Texture)
        if(! mTexture)
            mTexture = gl::Texture::create(*mSurface);
        else
            mTexture->update(*mSurface);
    }
    
    if(!mFrameDiffOrOpFlow)
        findOpticalFlow();//just what it says -- the meat of the program
    
        else frameDifference(mFrameDifference);//do the frame-differencing
}

void FeatureTrackingApp::findOpticalFlow()
{
    if(!mSurface) return; //don't go through with the rest if we can't get a camera frame!
    
    //convert gl::Texturer to the cv::Mat(rix) --> Channel() -- converts, makes sure it is 8-bit
    cv::Mat curFrame = toOcv(Channel(*mSurface));
    
    //the method will put the results (eg. the foreground mask) into the output frame
    cv::Mat outputFrame;
    if(mDoBackgroundSubtraction){
        mBackgroundSubtract->apply(curFrame, outputFrame);
    }
    else outputFrame = curFrame;
        
    //if we have a previous sample, then we can actually find the optical flow.
    if( mPrevFrame.data ){
        // pick new features once every SAMPLE_WINDOW_MOD frames, or the first frame
        
        //note: this means we are abandoning all our previous features every SAMPLE_WINDOW_MOD frames that we
        //had updated and kept track of via our optical flow operations.
        
        if( mFeatures.empty() || getElapsedFrames() % SAMPLE_WINDOW_MOD == 0 ){
            /*
             parameters for the  call to cv::goodFeaturesToTrack:
             curFrame - img,
             mFeatures - output of corners,
             MAX_FEATURES - the max # of features,
             0.005 - quality level (percentage of best found),
             3.0 - min distance
             
             note: its terrible to use these hard-coded values for the last two parameters. perhaps you will fix your projects.
             
             note: remember we're finding corners/edges using these functions
             */
            cv::goodFeaturesToTrack( outputFrame, mFeatures, MAX_FEATURES, 0.005, 3.0 );
        }
        
        vector<float> errors; //there could be errors whilst calculating optical flow
        
        mPrevFeatures = mFeatures; //save our current features as previous one
        
        //This operation will now update our mFeatures & mPrevFeatures based on calculated optical flow patterns between frames UNTIL we choose all new features again in the above operation every SAMPLE_WINDOW_MOD frames. We choose all new features every couple frames, because we lose features as they move in and out frames and become occluded, etc.
        if( ! mFeatures.empty() )
            cv::calcOpticalFlowPyrLK( mPrevFrame, outputFrame, mPrevFeatures, mFeatures, mFeatureStatuses, errors );
    }
    
    //set previous frame
    mPrevFrame = outputFrame;
}

//find the difference between 2 frames + some useful image processing
void FeatureTrackingApp:: frameDifference(cv::Mat &outputImg)
{
    outputImg.data = NULL;
    if(!mSurface) return;

    //the current surface or frame in cv::Mat format
    cv::Mat curFrame = toOcv(Channel(*mSurface));
    
    if(mPrevFrame.data){
        //blur --> this means that it will be resilient to a little movement
        //params are: cv::Mat Input1,
        //cv::Mat Result,
        //cv::Size - size of blur kernel (correlates to how blurred - must be positive & odd integers),
        // the bigger the size, the more the blur & also the larger the sigmas the more the blur.
        // double size of sigma X Gaussian kernel standard deviation in X direction
        // double size of sigma Y Gaussian kernel standard deviation in Y direction (optional, not used)
        //More on Gaussian blurs here: https://homepages.inf.ed.ac.uk/rbf/HIPR2/gsmooth.htm
        //Interestingly, we can think of them as a low-pass filter in 2D -- (if you know them from audio dsp, sound)
        cv::GaussianBlur(curFrame, curFrame, cv::Size(3,3), 0);

        
        //find the difference
        //params are: cv::Mat Input1, cv::Mat Input2, cv::Mat Result
        cv::absdiff(curFrame, mPrevFrame, outputImg);
        
        
        //take threshhold values -- think of this as image segmentation, see notes above in desc. header
        //we will go further into image segmentation next week
        //        https://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
        //Parameters:
        //  src – input array (single-channel, 8-bit or 32-bit floating point).
        //  dst – output array of the same size and type as src.
        //  thresh – threshold value.
        //  maxval – maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding types.
        //  type – thresholding type (see the details below).
        cv::threshold(outputImg, outputImg, 25, 255, cv::THRESH_BINARY);
    }

    mPrevFrame = curFrame;
}


void FeatureTrackingApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    
    //color the camera frame normally
    gl::color( 1, 1, 1, 1 );

    //draw the camera frame
    if( mTexture )
    {
        gl::draw( mTexture );
    }
    
    // draw all the old points @ 0.5 alpha (transparency) as a circle outline
    gl::color( 1, 0, 0, 0.55 );
    for( int i=0; i<mPrevFeatures.size(); i++ )
        gl::drawStrokedCircle( fromOcv( mPrevFeatures[i] ), 3 );
    
    // draw all the new points @ 0.5 alpha (transparency)
    gl::color( 0, 0, 1, 0.5f );
    for( int i=0; i<mFeatures.size(); i++ )
        gl::drawSolidCircle( fromOcv( mFeatures[i] ), 3 );
    
    //draw lines from the previous features to the new features
    //you will only see these lines if the current features are relatively far from the previous
    gl::color( 0, 1, 0, 0.5f );
    gl::begin( GL_LINES );
    for( size_t idx = 0; idx < mFeatures.size(); ++idx ) {
        if( mFeatureStatuses[idx] ) {
            gl::vertex( fromOcv( mFeatures[idx] ) );
            gl::vertex( fromOcv( mPrevFeatures[idx] ) );
        }
    }
    
    gl::end();
     //drawing output for Frame Differencing
      if(mFrameDiffOrOpFlow && mFrameDifference.data)
      {
          gl::draw( gl::Texture::create(fromOcv(mFrameDifference) ) );
          mFDiffSquare.drawRect(mFrameDifference);
      }
        //drawing output for Optical Flow
        else if((!mFrameDiffOrOpFlow) && mPrevFrame.data)
       {
           mFTrackingSquare.drawRect(mPrevFeatures, mPrevFrame);
           gl::color( 1, 1, 1, 0.5f);
           gl::draw( gl::Texture::create(fromOcv(mPrevFrame) ) );
       }
}

//runs commands from pressing a certain key
void FeatureTrackingApp::keyDown( KeyEvent event )
{
    //changing the number of squares on the screen
    if(event.getChar() == '1'){
        if(mFrameDiffOrOpFlow)
           mFDiffSquare.setN(10);
        else mFTrackingSquare.setN(10);
    }
    else if(event.getChar() == '2'){
        if(mFrameDiffOrOpFlow)
            mFDiffSquare.setN(20);
        else mFTrackingSquare.setN(20);
    }
    else if(event.getChar() == '3'){
        if(mFrameDiffOrOpFlow)
            mFDiffSquare.setN(30);
        else mFTrackingSquare.setN(30);
    }
    //switching between Frame Differencing and Optical Flow
    else if(event.getChar() == 'f'){
        mFrameDiffOrOpFlow = !mFrameDiffOrOpFlow;
    }
    //adding and removing background subtraction
    else if(event.getChar() == ' '){
        mDoBackgroundSubtraction = !mDoBackgroundSubtraction;
    }
}


CINDER_APP( FeatureTrackingApp, RendererGl )
