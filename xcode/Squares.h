/*
 *
   Programmer: Amayrani Luna
   date: 9/7/20
   Squares.h
   MoCapProject1
   
    This class holds the variable n and, uses this variable to divided the screen into n*n
    squares, determines where the white pixels are(indicating movement) in each square, and
    colors those pixels.
 *
 */

#ifndef Squares_h
#define Squares_h
using namespace cinder;

class squares{
private:
    int n;
    
public:
    
    //constructor
    squares(){
        n = 10;
    }
 
    void setN(int n1){
        n = n1;
    }
   
   //divides screen into n*n squares and colors white pixels that indicate movement
   void draw(cv::Mat frameDiff)
   {
       int squareWidth = frameDiff.cols / n;
       int squareHeight = frameDiff.rows / n;
       
       //creating squares
       for(int i = 0 ; i < n ; i++){
           for(int j = 0 ; j < n ; j++){
               int x1 = i * squareWidth;
               int x2 = x1 + squareWidth;
               int y1 = j * squareHeight;
               int y2 = y1 + squareHeight;
               Rectf curSquare = Rectf(x1, y1, x2, y2);
                
               int sum = 0;
               //counting white pixels
               for(int x = x1 ; x < x2 ; x++){
                   for(int y = y1 ; y < y2 ; y++){
                       int pixel = frameDiff.at<uint8_t>(y,x);
                       sum+=pixel;
                   }
               }
               //coloring white pixels
               gl::color(sum/(float)(n*n*255), 0, 1, 1);
               gl::drawSolidRect(curSquare);
            
                
           }
        }
   }
};
#endif // Squares_h







/*
//
//  Header.h
//
//
//  Created by courtney on 9/17/20.
//

#ifndef Header_h
#define Header_h

class Squares
{
protected:
    int N;
    virtual float count(ci::Rectf)=0;
    virtual float getDivisorOfSum()=0;
    void setN(int n){N=n;}


    virtual void drawRect()
    {
        int squareWidth = frameDiff.cols / n;
        int squareHeight = frameDiff.rows / n;
        ci::Rectf curSquare;
        
        
        //creating squares
        for(int i = 0 ; i < n ; i++){
            for(int j = 0 ; j < n ; j++){
                int x1 = i * squareWidth;
                int x2 = x1 + squareWidth;
                int y1 = j * squareHeight;
                int y2 = y1 + squareHeight;
                Rectf curSquare = Rectf(x1, y1, x2, y2);

                float sum = count(curSquare);
            
                gl::color(sum/getDivisorOfSum(), 0 ,1 , 1);
                gl::drawSolidRect(curSquare);
            }
        }
        //do all the things to find the squares -
        

        
        
        //divide sum by the appropriate numbers
        //use the result from above to change color
        //draw squares
    }
    

};



class SquaresFrameDiff : public Squares
{
private:
    cv::Mat frameDiff;
public:
    virtual void drawRect(cv::Mat b)
    {
        frameDiff = b;
        Squares::drawRect();
        
    }
    virtual float count(ci::Rectf curSquare)
    {
        //return the sum from the framedifferencing
        return 0;
    }
    
    float getDivisorOfSum()
    {
        return 1;
    }
    

};

class SquaresFeatures : public Squares
{
    private:
    std::vector<cv::Point2f> features;
    
  public:
    virtual void drawRect(std::vector<cv::Point2f> pts)
    {
        features = pts;
    //get features size in class variable
        Squares::drawRect();
        
    }

    virtual float count(ci::Rectf curSquare)
    {   sum = 0;
        //return the sum of the features points
        for(int i = 0 ; i < features.size(); i++){
            if(curSquare.contains(fromOcv(features[i])){
                sum++;
            }
        }
        return sum;
    }
    
    float getDivisorOfSum()
    {
        return 1; //return features size class variable 
    }
    
};





#endif /* Header_h*/
 
