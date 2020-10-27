//Programmer: Amayrani Luna
//Desc: Demonstrates receiving OSC with no clutter of the fancy example.
//Prints values receives from another program. 

//Go to the top menu Sketch->Import Library
//then search for oscP5 and import that
import netP5.*;
import oscP5.*;
import java.util.Random;

OscP5 oscP5; //the object that will send OSC
NetAddress remoteLocation; //where we are receiving OSC from

float maxPixelsValue; 

//where are the frame diff values on the screen in %, that is 0.-0.1
float x; 
float y; 

//the OSC addresses we are receiving
//MUST match what we are sending
final String MAXSUM_OSC_ADDRESS = "/MakeItArt/MaxSum"; 
final String WHERE_OSCADDRESS = "/MakeItArt/Where"; //where are the frame diff values? x & y in 0.0-1.0 values

//The port we are listening to... MUST match DESTPORT in the C++ example 
final int LISTENING_PORT = 8888;

void setup()
{
  size(500, 500);
  background(255, 179, 179);  
  
  //
  oscP5 = new OscP5(this, LISTENING_PORT); //listening for incoming!!
}

//this is the overloaded function to receive our OSC message 
void oscEvent(OscMessage msg)
{
  String addr = msg.addrPattern(); //get the address
  
  if(addr.equals(MAXSUM_OSC_ADDRESS))
  {
    maxPixelsValue = msg.get(0).floatValue(); //look at sendOSC() function in xcode to see what msg[0] is and see what you sent! 
    
  }
  else if(addr.equals(WHERE_OSCADDRESS)) //otherwise it indicates position, so set those
  {
     x = msg.get(0).floatValue(); //.floatValue() translates the value into a float 
     y = msg.get(1).floatValue(); 
  }
  
  //print the values
  //println("maxPixelsValue: " + maxPixelsValue + " x:" + x + " y:" + y );
}

void draw()
{
  if(maxPixelsValue < 500){ //when square has less than 500 white pixels(indicating frame difference)fill the circle white or when there's few motion color the circle white
     fill(255);
  }
   else{ //else randomly color the circles with green/blue colors
     Random rand = new Random(); 
     fill(rand.nextInt(20), rand.nextInt(240), rand.nextInt(255));
   }
   
   ellipse(x*width, y*height, 100, 100);
}
