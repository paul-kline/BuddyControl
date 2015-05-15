import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import g4p_controls.*; 
import processing.net.*; 
import processing.dxf.*; 
import processing.serial.*; 
import java.net.*; 
import java.io.*; 
import java.util.Date; 
import java.text.DateFormat; 
import java.text.SimpleDateFormat; 
import java.util.Calendar; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class EmailSenderNoCameraVersion extends PApplet {

//import processing.video.*;
















String urlStr = "https://script.google.com/macros/s/AKfycby-30VxVIFPKvNyLLTrBsfGczxtexBfb02BCOFEqiUloBdYSIMl/exec?task=arduino";
String buffer = "";
Serial port;
PFont font;
boolean sentEmail = false;
GTextField txf1;
GTextArea txa1;
public void sendEmail(String message) {
  println("sending email");
  try {
    URL url = new URL(urlStr + "&message=" + URLEncoder.encode(message, "US-ASCII"));
    URLConnection conn = url.openConnection();
    conn.connect();

    BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
  } 
  catch (Exception ex) {
    ex.printStackTrace();
    System.out.println("ERROR: " + ex.getMessage());
  }
  txa1.appendText("Attempted Email: " + message);
  txa1.appendText("\n");
  sentEmail = true;
}

//Capture cam;
public void setup() {
  //String[] cameras = Capture.list();

  //if (cameras.length == 0) {
    //println("There are no cameras available for capture.");
    //exit();
  //} else {
    //println("Available cameras:");
    //for (int i = 0; i < cameras.length; i++) {
      //println(cameras[i]);
    //}
    //cam = new Capture(this);
    //cam.start();
  //}



  txf1 = new GTextField(this, 10, 10, 200, 40);
  txf1.tag = "txf1";
  txf1.setPromptText("Commands");

  txa1 = new GTextArea(this, 210, 10, 290, 100, G4P.SCROLLBARS_VERTICAL_ONLY);
  txa1.tag = "txa1";
  txa1.setTextEditEnabled (false);
  // txa1.setText("tefiej\nfiej\nfei\nfj\neif\nefi\nejf\niejf\neij\nfeifjeifjeif\n\n\n\nje\nifje\nigihe\nigheig");
  //txa1.setPromptText("Text area 1");
  txa1.setText(" ");
  String arduinoPort = Serial.list()[0];
  port = new Serial(this, arduinoPort, 9600);

  size(700, 780);
  frameRate(60);
  font = loadFont("TimesNewRomanPSMT-32.vlw");
  fill(255);
  textFont(font, 32);
  println(Serial.list());
}


public void draw() {
  //if(cam.available()){
    //cam.read();
  //}
  //image(cam,10, 100);
    
  // println("drawing");
  //  text("text here:", 10, 10, 70, 80);
  if (port.available() > 0) {
    int inByte = port.read();
    //print(char(inByte));
    //txa1.appendText(Character.toString(char(inByte)));
    if (inByte != 10) {

      buffer = buffer + PApplet.parseChar(inByte);
    } else {
      if (buffer.length() > 1) {
        //buffer = buffer.substring(0,buffer.length()-1);
        txa1.appendText(buffer);
        txa1.appendText("\n");
        //  txa1.insertText(buffer,true, false);
        // text(buffer,10,100);
        println(buffer);
        //text("\n",10,100);
        if (!sentEmail && buffer.indexOf("got this: 1122334455")>=0 ) {
          sendEmail("wire tripped. TIME: " + GetCurrentDateTime());
        }
        if (buffer.indexOf("got this: 1223334444") >= 0 ) {
          sendEmail("Proximity sensor triggered! TIME: " + GetCurrentDateTime());
        } // port.clear(); 
        buffer = "";
      }
    }
  }
}

public void sendEmail() {
  //internet here!
  println("sending email");
  try {
    URL url = new URL(urlStr);
    URLConnection conn = url.openConnection();
    conn.connect();

    BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));

    sentEmail = true;
  } 
  catch (Exception ex) {
    ex.printStackTrace();
    System.out.println("ERROR: " + ex.getMessage());
  }
}

public void handleTextEvents(GEditableTextControl textcontrol, GEvent event) { 
  // println("event: " + textcontrol.getClass());
  switch (event) {
  case ENTERED:
    if (txf1.getText().indexOf("test email") >= 0) {
      sendEmail(txf1.getText());
    } else {
      port.write(txf1.getText());
      //println("enter hit:" + txf1.getText());
    }
    txf1.setText("");
    break;
  default:
    //  println("not an enter");
  }
}

public String GetCurrentDateTime() {
  DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
  //get current date time with Date()
  Date date = new Date();
  System.out.println(dateFormat.format(date));

  //get current date time with Calendar()
  Calendar cal = Calendar.getInstance();
  return dateFormat.format(cal.getTime());
}

  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "EmailSenderNoCameraVersion" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
