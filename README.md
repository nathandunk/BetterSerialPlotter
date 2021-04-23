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

Once you are properly connected, on the left side of the screen, you should see a list of all of the variables that you have sent from your arduino code. If you want to configure, the name, plot color, or what plots a variable shows up on, you can right-click the variable name. A context menu should open up as SHOWN BELOW, where you can edit these items. Click on save to make your changes. 

## Drag and Drop

You can click and drag variables from the Data Panel to any plot to add it to that plot. You can also drag variables to a specific axis on any of the plots (x-axis, left y-axis, right y-axis) to add them to a specific axis.

## Plot Controls

There are several configuration options to change how to display your plots, including:
- Resize the plot by clicking and dragging the bottom of it
- Add variables to the plot by right clicking and selecting *Add Data*
- Add variables to the plot by right clicking and selecting *Remove Data*
- Autoscale to make the limits of the axes line up with the available data
- Scroll when hovering over any axis to change the limits (except when autoscale is on)
- Double click on the plot to set the y-limits to the currently available data
- Click and drag on the plot to pan along the non-locked axes
- If you have added a custom x-axis variable, you can right click and select *Realtime X-axis* to keep it's max x-axis limit to be the max x-axis variable value.

## Saving Data

You can save data that you are collecting by clicking on the **Export CSV** button. This should open a file explorer window where you can choose where to save your data as a .csv file. This will save all of the data that is currently available, meaning that it will still output all of the data from after you clicked the button to when you click save. I am hoping to add an option to save paused data as well, so that you can leave it in a specific state as you work on saving the data.

## Saving/loading configuration

You can save the configuration of your data and plots by clicking the **Save Config** button. This will save the names and colors of each of the variables, as well as the configuration of the plots, including each of the plot-specific configuration options and what variables are on what axes. This feature has not been extensively tested, so it is possible that this could crash if configured incorrectly.

## Serial Monitor

The serial monitor lets you view all of your data as if it was on the built-in arduino serial monitor. By default, the data will autoscroll to keep you looking at the most recent data input, but you can click or unclick the autoscroll checkbox to enable or disable this functionality.

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
- [x] fix autoscale range
- [ ] cross platform
- [x] fix data panel same name selection
- [x] fix comport saving as -1
- [x] add tests
- [x] make more functionalized
- [x] change to analyzing by line, not by char