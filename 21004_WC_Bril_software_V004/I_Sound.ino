void Sound()
{
  if ((Error!=0) and (SoundGespeeld==1))
  {
    digitalWrite(AudioOnOff,HIGH); 
    delay(2000); 
    Player.volume(Volume); 
    delay(100);
    if (Error==1)
    {     
      Player.play(1);  //speel eerste mp3
      delay(2000);
    }
    if (Error==2)
    {     
      Player.play(2);  //speel tweede mp3
      delay(2000);
    }
    if (Error==3)
    {     
      Player.play(3);  //speel derde mp3
      delay(2000);
    }
    if (Error==4)
    {     
      Player.play(4);  //speel vierde mp3
      delay(2000);
    }
    if (Error==5)
    {     
      Player.play(5);  //speel vijde mp3
      delay(2000);
    }
    if (Error==6)
    {     
      Player.play(6);  //speel zesde mp3
      delay(2000);
    }    
  }
  digitalWrite(AudioOnOff,LOW); 
  SoundGespeeld=0;
}
