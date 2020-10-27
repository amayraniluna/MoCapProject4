/* Programmer: Amayrani Luna
   date: 9/7/20
   Squares.h
   MoCapProject1
   
    This class holds the variable n and, uses this variable to divided the screen into n*n
    squares, determines where the white pixels are(indicating movement) in each square, and
    colors those pixels.
*/

#ifndef Squares_h
#define Squares_h
using namespace cinder;

class squares{
    protected:
    int N;
    int maxX;
    int maxY;
    int highestSum;
    
    virtual int count(ci::Rectf)=0;
    virtual float getDivisorOfSum()=0;
    
    public:
    void setN(int n){N=n;}
    int getMaxX(){return maxX;}
    int getMaxY(){return maxY;}
    int getHighestSum(){return highestSum;}
    
    //constructor
    squares(){
        N=10;
    }
    
    //draws the squares given "Mat image" to get the size of the screen
    virtual void drawRect(cv::Mat image)
    {
        int squareWidth = image.cols / N;
        int squareHeight = image.rows / N;
        
        highestSum = 0;
        //creating squares
        for(int i = 0 ; i < N ; i++)
        {
            for(int j = 0 ; j < N ; j++)
            {
                int x1 = i * squareWidth;
                int x2 = x1 + squareWidth;
                int y1 = j * squareHeight;
                int y2 = y1 + squareHeight;
                Rectf curSquare = Rectf(x1, y1, x2, y2);

                int sum = count(curSquare);
                if(sum > highestSum){
                    highestSum = sum;
                    maxX = x1;
                    maxY = y1;
                }
                //divide sum by the appropriate numbers
                //use the result from above to change color
                gl::color(sum/(getDivisorOfSum()), 1 ,1 , .5);
                //draw squares
                gl::drawSolidRect(curSquare);
            }
        }
    }
};


//PROJ 1 FRAME DIFFERENCING
class SquaresFrameDiff : public squares
{
private:
    cv::Mat frameDiff;
    
public:
    //passes Mat b to super class draw function
    virtual void drawRect(cv::Mat b)
    {
        frameDiff = b;
        squares::drawRect(frameDiff);
    }
    
    //returns the number of white pixels per "curSquare"
    virtual int count(ci::Rectf curSquare)
    {
        int tempSum = 0;
        //counting white pixels
        for(int x = curSquare.x1 ; x < curSquare.x2 ; x++)
        {
            for(int y = curSquare.y1 ; y < curSquare.y2 ; y++)
            {
                int pixel = frameDiff.at<uint8_t>(y,x);
                tempSum+=pixel;
            }
        }
        
        std::cout << "sum: " << tempSum << std::endl;
        return tempSum;
    }
    
    
    float getDivisorOfSum()
    {
        return (float)(N*N*255);
    }
    
    //sets number of squares on screen
    void setN(int num)
    {
        squares::setN(num);
    }
};


//PROJ 2 FEATURE TRACKING
class SquaresFeatures : public squares
{
    private:
    std::vector<cv::Point2f> features;
    float featuresSize;
    
  public:
    //passes Mat image to super class draw function
    virtual void drawRect(std::vector<cv::Point2f> pts, cv::Mat image)
    {
        features = pts;
        featuresSize = features.size();
     
        squares::drawRect(image);
    }

    //returns number of features per "curSquare"
    virtual int count(ci::Rectf curSquare)
    {
        int tempSum = 0;
        for(int i = 0 ; i < features.size(); i++)
        {
            if(curSquare.contains(fromOcv(features[i])))
            {
                tempSum++;
            }
        }
        return tempSum;
    }
    
    float getDivisorOfSum()
    {
        return (float)(featuresSize/10);
    }
    
    //sets number of squares on screen
    void setN(int num)
    {
        squares::setN(num);
    }
    
};
#endif // Squares_h

