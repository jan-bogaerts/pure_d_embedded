void CheckFilter()
{
  if (digitalRead(FilterIO)==1)
  {
    Error=1;
    ErrorAfhandeling();
    FilterVervangingenOld=1;    
  }
}
