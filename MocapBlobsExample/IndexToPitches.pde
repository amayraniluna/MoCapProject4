//Programmer: Courtney Brown
//Purpose: just maps the index (eg. the y value) to the rhythm

import java.util.ArrayList;

class IndexToPitches
{
  ArrayList<Float> hz = new ArrayList(); 
  
  IndexToPitches()
  {
    hz.add(Frequency.ofPitch( "A4" ).asHz() ); //C4
    hz.add(Frequency.ofPitch( "A5" ).asHz() ); //B3
    hz.add(Frequency.ofPitch( "B4" ).asHz() ); //A3
    hz.add(Frequency.ofPitch( "B5" ).asHz() ); //G3
    hz.add(Frequency.ofPitch( "C4" ).asHz() );  //F3
    hz.add(Frequency.ofPitch( "C5" ).asHz() ); //E3
    hz.add(Frequency.ofPitch( "D4" ).asHz() ); //D3
    hz.add(Frequency.ofPitch( "D5" ).asHz() ); //C3
    hz.add(Frequency.ofPitch( "E4" ).asHz() );  //B2
    hz.add(Frequency.ofPitch( "E5" ).asHz() );  //A2
    hz.add(Frequency.ofPitch( "F4" ).asHz() );  //G2
    hz.add(Frequency.ofPitch( "F5" ).asHz() );  //F2
    hz.add(Frequency.ofPitch( "G4" ).asHz() );  //E2
    hz.add(Frequency.ofPitch( "G5" ).asHz() ); //D2
    hz.add(Frequency.ofPitch( "G2" ).asHz() );  //C2
  }
  
  float size()
  {
    return (float) hz.size(); 
  }
  
  float getPitch(int index)
  {
    if(index>=0 && index < hz.size())
      return hz.get(index).floatValue();
     else return hz.get(0).floatValue(); //silent fail oh well
  }
  
}
