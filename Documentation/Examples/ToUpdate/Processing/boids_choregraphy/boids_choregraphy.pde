/**
 * boids_choregraphy
 * based on the Daniel Shiffman's Flocking example.
 * 
 * An implementation of Craig Reynold's Boids program to simulate
 * the flocking behavior of birds. Each boid steers itself based on 
 * rules of avoidance, alignment and cohesion.
 * 
 * Control the sketch using keyboard () or via OSC messages:
 *
 * (i) /flock/info i : display flock parameters (default true)
 * (c) /flock/clear : delete all existing boids
 * (k) /flock/kill i : delete each boids which are going out (default false)
 * (a) /flock/add : message to add a new boid at mouse position
 * (-) /flock/avoidance f : parameter to control avoidance weight (default 1.)
 * (-) /flock/alignment f : parameter to control alignment weight (default 1.)
 * (-) /flock/cohesion f : parameter to control cohesion weight (default 1.)
 * (-) /flock/distance/min f : parameter to control minimal distance between boids (default 50.)
 * (-) /flock/distance/max f : parameter to control maximal distance between boids (default 75.)
 * (f) /flock/follow/rate f : parameter to control how much boids follow the mouse (default 0.)
 * (-) /flock/size : returns the number of boids
 * (-) /flock/position/x : returns horizontal mean position of the flock
 * (-) /flock/position/y : returns vertical mean position of the flock
 * (-) /flock/direction/x : returns horizontal mean direction of the flock
 * (-) /flock/direction/y : returns vertical mean direction of the flock
 * (-) /flock/deviation/x : returns horizontal deviation of the flock
 * (-) /flock/deviation/y : returns vertical deviation of the flock
 * (-) /flock/killed : returned each time a boid is killed
 * (-) /flock/collision : returned each time at least one boid collide another one
 * (-) /mouse/click : returns 1 when mouse is pressed and 0 when released
 * (-) /text s : a textual content to display
 * (-) /sky/color i i i : setup sky color 
 */

import oscP5.*;
import netP5.*;

OscP5 osc_in;
NetAddress osc_out;

Flock flock;

int last_flock_size; // to filter repetitions
 
boolean info = true;

boolean follow_mouse = false; // while mappings are not possible in i-score

PFont font;
String text = "";

int sky_color[] = {50, 50, 50};

void setup() 
{
  size(1920, 1080);

  // listen osc message from i-score
  osc_in = new OscP5(this, 13001);

  // output osc messages to i-score
  osc_out = new NetAddress("127.0.0.1", 13002);
  
  // load font
  font = loadFont("ComicSansMS-48.vlw");
  textFont(font);

  flock = new Flock();
}

void draw()
{
  OscMessage osc_msg;
  
  background(sky_color[0], sky_color[1], sky_color[2]);

  // update flock (and get change of flock size)
  int size_change = flock.run();
  
  if (size_change < 0)
  {
    // a quick red flash each time a boid is killed 
    background(100, 0, 0);
    
    // send OSC
    osc_msg = new OscMessage("/flock/killed");
    osc_in.send(osc_msg, osc_out);
  }
  
  if (flock.getCollision())
  {    
    // send OSC message
    osc_msg = new OscMessage("/flock/collision");
    osc_in.send(osc_msg, osc_out);
  }
  
  // display text
  textSize(48);
  fill(200);
  text(text, width/10, height/2);

  // draw flock informations
  if (info)
    flock.draw_info();
    
  // send OSC message for flock size
  osc_msg = new OscMessage("/flock/size");
  osc_msg.add(flock.getSize());
  osc_in.send(osc_msg, osc_out);
  
  // send OSC message for flock mean position x
  osc_msg = new OscMessage("/flock/position/x");
  osc_msg.add(flock.getPosition().x);
  osc_in.send(osc_msg, osc_out);

  // send OSC message for flock mean position y
  osc_msg = new OscMessage("/flock/position/y");
  osc_msg.add(flock.getPosition().y);
  osc_in.send(osc_msg, osc_out);
  
  // send OSC message for flock mean direction x
  osc_msg = new OscMessage("/flock/direction/x");
  osc_msg.add(flock.getDirection().x);
  osc_in.send(osc_msg, osc_out);

  // send OSC message for flock mean direction y
  osc_msg = new OscMessage("/flock/direction/y");
  osc_msg.add(flock.getDirection().y);
  osc_in.send(osc_msg, osc_out);

  // send OSC message for flock deviation x
  osc_msg = new OscMessage("/flock/deviation/x");
  osc_msg.add(flock.getDeviation().x);
  osc_in.send(osc_msg, osc_out);

  // send OSC message for flock deviation y
  osc_msg = new OscMessage("/flock/deviation/y");
  osc_msg.add(flock.getDeviation().y);
  osc_in.send(osc_msg, osc_out);

  // use keyboard to control flock parameters
  if (keyPressed)
  {
    switch (key)
    {
    case 'f':
      {
        flock.setFollowDestination(mouseX, mouseY);
        break;
      }
    };
  }
  
  // while mappings are not possible in i-score
  // note : /follow/destination/rate needs to be set to 1.
  if (follow_mouse)
    flock.setFollowDestination(mouseX, mouseY);
}

void mousePressed()
{
  OscMessage osc_msg = new OscMessage("/mouse/click");
  osc_msg.add(1);
  osc_in.send(osc_msg, osc_out);
  
  // while mappings are not possible in i-score
  follow_mouse = true;
}

void mouseReleased()
{
  OscMessage osc_msg = new OscMessage("/mouse/click");
  osc_msg.add(0);
  osc_in.send(osc_msg, osc_out);
  
  // while mappings are not possible in i-score
  follow_mouse = false;
}

void oscEvent(OscMessage osc_msg)
{
  if (osc_msg.checkAddrPattern("/flock/info")) 
  {
    if (osc_msg.checkTypetag("i")) 
    {
      info = osc_msg.get(0).intValue() == 1;
      println("### " + osc_msg.addrPattern());
    }
  } else if (osc_msg.checkAddrPattern("/flock/clear")) 
  {
    flock.clear();
    println("### " + osc_msg.addrPattern());
  } else if (osc_msg.checkAddrPattern("/flock/kill")) 
  {
    if (osc_msg.checkTypetag("i")) 
    {
      flock.setKill(osc_msg.get(0).intValue() == 1);
      println("### " + osc_msg.addrPattern());
    }
  } 
  else if (osc_msg.checkAddrPattern("/flock/add")) 
  {
    flock.addBoid(new Boid(mouseX, mouseY));
    println("### " + osc_msg.addrPattern());
  }
  
  if (osc_msg.checkAddrPattern("/flock/avoidance"))
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float avoidance = osc_msg.get(0).floatValue();
      flock.setAvoidance(avoidance);
      println("### " + osc_msg.addrPattern());
    }
  } else if (osc_msg.checkAddrPattern("/flock/alignment"))
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float alignment = osc_msg.get(0).floatValue();
      flock.setAlignment(alignment);
      println("### " + osc_msg.addrPattern());
    }
  } else if (osc_msg.checkAddrPattern("/flock/cohesion"))
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float cohesion = osc_msg.get(0).floatValue();
      flock.setCohesion(cohesion);
      println("### " + osc_msg.addrPattern());
    }
  } else if (osc_msg.checkAddrPattern("/flock/distance/min"))
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float distance_min = osc_msg.get(0).floatValue();
      flock.setDistanceMin(distance_min);
      println("### " + osc_msg.addrPattern());
    }
  } else if (osc_msg.checkAddrPattern("/flock/distance/max"))
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float distance_max = osc_msg.get(0).floatValue();
      flock.setDistanceMax(distance_max);
      println("### " + osc_msg.addrPattern());
    }
  } else  if (osc_msg.checkAddrPattern("/flock/follow/rate")) 
  {
    if (osc_msg.checkTypetag("f")) 
    {
      float follow_rate = osc_msg.get(0).floatValue();
      flock.setFollowRate(follow_rate);
      println("### " + osc_msg.addrPattern());
    }
  } else  if (osc_msg.checkAddrPattern("/text")) 
  {
    if (osc_msg.checkTypetag("s")) 
    {
      text = osc_msg.get(0).stringValue();
      println("### " + osc_msg.addrPattern());
    }
  } else  if (osc_msg.checkAddrPattern("/sky/color")) 
  {
    if (osc_msg.checkTypetag("fff")) 
    {
      sky_color[0] = (int)osc_msg.get(0).floatValue();
      sky_color[1] = (int)osc_msg.get(1).floatValue();
      sky_color[2] = (int)osc_msg.get(2).floatValue();
      println("### " + osc_msg.addrPattern());
    }
  }
}

void keyPressed()
{
  switch (key)
  {
  case 'i':
    {
      info = !info;
      break;
    }
  case 'c':
    {
      flock.clear();
      break;
    }
  case 'a':
    {
      flock.addBoid(new Boid(mouseX, mouseY));
      break;
    }
  case 'f':
    {
      noCursor();
      flock.setFollowRate(1.0);
      break;
    }
  case 'k':
    {
      flock.setKill(true);
      break;
    }
  };
}

void keyReleased()
{
  switch (key)
  {
  case 'f':
    {
      cursor();
      flock.setFollowRate(0.0);
      break;
    }
    
  case 'k':
    {
      flock.setKill(false);
      break;
    }
  };
}

