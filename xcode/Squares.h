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
    protected:
    int N;
    virtual float count(ci::Rectf)=0;
    virtual float getDivisorOfSum()=0;
    void setN(int n){N=n;}


    virtual void drawRect(cv::Mat image)
    {
        int squareWidth = image.cols / N;
        int squareHeight = image.rows / N;
        ci::Rectf curSquare;
        
        //creating squares
        for(int i = 0 ; i < N ; i++){
            for(int j = 0 ; j < N ; j++){
                int x1 = i * squareWidth;
                int x2 = x1 + squareWidth;
                int y1 = j * squareHeight;
                int y2 = y1 + squareHeight;
                Rectf curSquare = Rectf(x1, y1, x2, y2);

                float sum = count(curSquare);
                //divide sum by the appropriate numbers
                //use the result from above to change color
                gl::color(sum/getDivisorOfSum(), 0 ,1 , 1);
                //draw squares
                gl::drawSolidRect(curSquare);
            }
        }
    }
};


class SquaresFrameDiff : public squares
{
private:
    cv::Mat frameDiff;
    float sum = 0.0;
public:
    virtual void drawRect(cv::Mat b)
    {
        frameDiff = b;
        squares::drawRect(b);
        
    }
    virtual float count(ci::Rectf curSquare)
    {
        //counting white pixels
        for(int x = curSquare.x1 ; x < curSquare.x2 ; x++){
            for(int y = curSquare.y1 ; y < curSquare.y2 ; y++){
                int pixel = frameDiff.at<uint8_t>(y,x);
                sum+=pixel;
            }
        }
        return sum;
    }
    
    float getDivisorOfSum()
    {
        float div = N*N*255;
        return div;
    }
    

};

class SquaresFeatures : public squares
{
    private:
    std::vector<cv::Point2f> features;
    
  public:
    virtual void drawRect(std::vector<cv::Point2f> pts)
    {
        features = pts;
    //get features size in class variable
        squares::drawRect();
        
    }

    virtual float count(ci::Rectf curSquare)
    {
        float sum = 0.0;
        //return the sum of the features points
        for(int i = 0 ; i < features.size(); i++){
            if(curSquare.contains(fromOcv(features[i]))){
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
#endif // Squares_h

