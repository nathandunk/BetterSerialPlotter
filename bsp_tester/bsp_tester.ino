int loop_size = 6;

void setup() {
  // Choose serial monitor baud rate here. 
  // This number must match in the BetterSerialPlotter application
  Serial.begin(9600);
}

void loop() {
  // This could be useful if you want to keep track of this program time since
  // starting. Try dragging this first variable to an x-axis.
  double seconds = millis()/1000.0;
  
  Serial.print(seconds);
  Serial.print("\t");

  // for as many variables as the loop size
  for(int i = 0; i < loop_size-1; i++){
    double val = (i%2 == 0) ? sin(seconds*(i+1)) : cos(seconds*(i+1));
    Serial.print(val);
    Serial.print("\t");
  }

  // These two variables make a spiral pattern if plotted against eachother. Try plotting
  // one of these on an x-axis and one of them on a y-axis of the same plot
  Serial.print(0.1*seconds*sin(seconds*2));
  Serial.print("\t");
  Serial.println(0.1*seconds*cos(seconds*2));
  delay(1);
}
