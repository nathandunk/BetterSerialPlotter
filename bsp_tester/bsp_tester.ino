int loop_size = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  double seconds = (double)millis()/1000.0;

  for(int i = 0; i < loop_size-1; i++){
    double val = (i%2 == 0) ? sin(seconds) : cos(seconds);
    Serial.print(val + 0.1*(double)i + seconds/10.0);
    Serial.print("\t");
  }
  Serial.println(sin(seconds)+0.1*(double)loop_size + seconds/10.0);
//  Serial.println();
  // put your main code here, to run repeatedly:
//  Serial.print(sin(static_cast<double>(millis())/1000.0));
//  Serial.print("\t");
//  Serial.println(millis()%1000>500 ? 1 : -1);
//  Serial.print("\t");
//  Serial.print(sin(static_cast<double>(millis())/1000.0)+0.1);
//  Serial.print("\t");
//  Serial.print(sin(static_cast<double>(millis())/1000.0)+0.2);
//  Serial.print("\t");
//  Serial.print(sin(static_cast<double>(millis())/1000.0)+0.3);
//  Serial.print("\t");
//  Serial.print(sin(static_cast<double>(millis())/1000.0)+0.4);
//  Serial.print("\t");
//  Serial.print(sin(static_cast<double>(millis())/1000.0)+0.5);
//  Serial.print("\t");
//  Serial.println(cos(static_cast<double>(2*millis())/1000.0));
  delay(1);
}
