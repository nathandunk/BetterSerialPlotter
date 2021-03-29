# BetterSerialPlotter
The *Better Serial Plotter* is A drop-in replacement for the arduino serial plotter. You should be able to set up things exactly the same in your arduino code, but work with the extended functionality of the Better Serial Plotter. Some beneficial features of the Better Serial plotter include:
- Comport and baud rate selection
- Support for multiple plots
- Change names and colors of variables
- Ability to view a serial monitor in the same application
- Auto-scroll option on serial monitor
- Plots with multiple y-axes
- x vs y plots
- Saving output to a file
- Autoscale plots

## What your arduino program should look like
This program is intended to work for any program that already uses the built-in Arduino Serial Plotter. That means that you need to open the Serial communication channel in your `setup()` function using `Serial.Begin(BAUD_RATE)`. Make sure you take note of what baud rate you use. Then in every iteration of your `loop()` function in your arduino code, you should write to the serial monitor with any amount of variables, each separated by either a tab or a space. Before the loop ends, you should output a newline character. See the Example Arduino Code section at the bottom to see how this could look.

## Running the application

Make sure that your arduino (or serial-interface device) is plugged into your computer and running. Run the **Better Serial Plotter** application, and a graphical interface should open up. From there, click on the **Serial Port** dropdown on the top of the screen. You should see a list of serial devices attached to your PC. Click on the serial port that your device is attached to. Then, you should click the dropdown for the baud rate and select the baud rate that you used in your `Serial.Begin()` function in your arduino code. If everything is set up properly, you should see your data on the left side of the application. If you are having trouble, make sure to check out the troubleshooting tips at the end of this page.

## Configuring Data
Once you are properly connected, on the left side of the screen, you should see a list of all of the variables that you have sent from your arduino code. If you want to configure, the name, plot color, or what plots a variable shows up on, you can right-click the variable name. A context menu should open up as SHOWN BELOW, where you can edit these items. Click on save to make your changes. You can also right click on each plot to configure several parts of the application

## Serial Monitor

## Example Arduino Code

```cpp
loop(){
    // get all of our variables of interest
    int t = millis();
    float var1 = sin((float)t/1000.0);
    float var2 = cos((float)t/1000.0);
    // write them all to console with tabs in between them
    Serial.print(t);
    Serial.print("\t");
    Serial.print(var1);
    Serial.print("\t");
    Serial.println(var2); // make sure to finish with a println!
    
    // at 2.5 seconds, this prints out like so, where \t
    // is the tab character, and \n is the newline character
    // 2500\t0.598\t-0.801\n
}
```

```cpp
loop(){
    // get the current program time in milliseconds
    // and write it to console, and add a space after
    int t = millis();
    Serial.print(t);
    Serial.print(" ");
    
    float var1 = sin((float)t);
    Serial.print(var1);
    Serial.print(" ");

    float var2 = cos((float)t);
    Serial.print(var2);
    Serial.println(); // this just prints a \n character

    // at 2.5 seconds, this prints out like so, where numbers
    // are separated by spaces, and \n is the newline character
    // 2500 0.598 -0.801\n
}
```

## Trouble shooting
- My device isn't showing up
Check that your device is showing up on your machine without the application. See this article provided by matlab for a useful way to find it https://www.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html.


There are several improvements that are currently being worked on, including:
- [x] add support for multiple plots
- [x] add serial monitor
- [x] add numeric next to data names
- [x] enable change of data names
- [x] close comport when switching
- [x] pause button
- [x] record data
- [x] save configuration
- [ ] add documentation
- [x] resize plots
- [x] selectable plot buttons on name window
- [x] drag to x-axis
- [x] multiple y-axes
- [x] fix right-click edit name persistance
- [ ] multi-thread to increase efficiency
- [x] change duration of plots
- [x] plot names
- [ ] change plot name
- [x] add colors to add/remove data
- [x] add axes labels to remove data
- [x] allow file explorer access for csv/read/write configs
- [x] fix data persistence for loading configs
- [ ] add scatter plots
- [x] fix data export offset
- [ ] fix autoscale range
- [ ] cross platform
- [x] fix data panel same name selection
- [x] fix comport saving as -1