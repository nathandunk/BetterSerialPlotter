// Better Serial Plotter example code 
// https://github.com/nathandunk/BetterSerialPlotter

void setup(){
    Serial.begin(9600);
}

void loop(){
    float t = millis()/1000.0; // get program time
    Serial.print(t);           // output time in seconds as first variable
    Serial.print(" ");         // add spacing between variables
    
    float var_sin = sin(t); 
    Serial.print(var_sin);     // output sin(t) variable
    Serial.print(" ");      // add spacing between variables

    float var_cos = cos(t); 
    Serial.print(var_cos);     // output cos(t) variable
    Serial.println();       // this just prints a \n character if you don't provide an argument

    // at 2.5 seconds, this prints out like so, where numbers
    // are separated by spaces, and \n is the newline character
    // 2500 0.598 -0.801\n
}
