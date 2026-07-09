#include <Adafruit_GFX.h>      // core graphics library
#include <Adafruit_ST7789.h>   // driver for the ST7789 screen
#include <SPI.h>

// --- Pin assignments (change these to match how YOU wired it) ---
#define TFT_CS 3
#define TFT_DC   7
#define TFT_SCLK 21
#define TFT_RST 8
#define TFT_MOSI 20
#define UP 0
#define OK 1
#define DOWN 2
#define BACK 4
#define MODE 5
#define BUZZER 10

// --- Objects live out here, outside any function ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

enum Screen {
  CLOCK,
  MODE_SELECT,
  STUDY,
  SETTINGS,
  SET_STUDY,
  SET_ALARM,
  SET_CLOCK,
};

Screen currentScreen = CLOCK;

// MODES
int modeselection = 0; 
// STUDY 
int sessions = 1;
int settingsselection = 0;
bool studying = false;
bool paused = false;
bool onbreak = false;
//CLOCK
int clockhour = 0;
int clockminute = 0;
unsigned long clockStartMillis = 0;
//ALARM
int alarmhour = 0;
int alarmminute = 0;
bool choosehour=true;
bool chooseminute=false;
bool alarmon=false;
//SET STUDY
int studyminutes = 25;
int breakminutes = 5;
bool choosestudy = true;
bool choosebreak = false;
int studytime = studyminutes*60;
int breaktime = breakminutes*60;
unsigned long studyStartMillis = 0;

void setup() {
  Serial.begin(115200);          // lets the board talk to your computer

  tft.init(76, 284);             // screen dimensions
  tft.setRotation(2);            // orientation, could be different, try 0-3
  Serial.println("TFT Initialized!");

  tft.fillScreen(ST77XX_BLACK);  // clear the screen
  tft.setCursor(0, 0);           // top-left corner
  tft.setTextSize(2);            // a readable size
  tft.print("Hello, world!");    // draw some text so we know it works

  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(OK, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(BACK, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  clockStartMillis = millis();
}

// loop() runs OVER and OVER, forever
void loop() {
  bool upPressed = (digitalRead(UP) == LOW);
  bool downPressed = (digitalRead(DOWN) == LOW);
  bool okPressed = (digitalRead(OK) == LOW);
  bool modePressed = (digitalRead(MODE) == LOW);
  bool backPressed = (digitalRead(BACK) == LOW);

  if (modePressed) { //if MODE is pressed:
    beep();
    currentScreen=MODE_SELECT; //open screen MODE_SELECT
  }
  if (currentScreen==MODE_SELECT) {
    if (upPressed) {
      if (modeselection<2) {
          modeselection++;
      }
      else {
          modeselection=0;
      }
    }
    if (downPressed) {
        if (modeselection>0) {
            modeselection--; 
        }
        else {
            modeselection=2;
        }
    }

    if (okPressed) {
      if (modeselection==0) {
        currentScreen=CLOCK;
      }
      if (modeselection==1) {
        currentScreen=STUDY;
      }
      if (modeselection==2) {
        currentScreen=SETTINGS;
      }
    }

    if (backPressed) {
      currentScreen=CLOCK;
    }
  }
  if (currentScreen == STUDY) {
    if (!studying && !paused) {
      if (upPressed) {
        sessions++;
      }

      if (downPressed) {
        if (sessions > 1) {
          sessions--;
        }
      }

      if (okPressed) {
        studying = true;
        paused = false;
        onbreak = false;
        studyStartMillis = millis();
        studytime = studyminutes * 60;
        breaktime = breakminutes * 60;
      }

      if (backPressed) {
        currentScreen = CLOCK;
      }
    }

    if (studying && !paused) {
      if (backPressed) {
        paused = true;
      }

      if (millis() - studyStartMillis >= 1000) {
        studyStartMillis = millis();

        if (!onbreak) {
          studytime--;
        } else {
          breaktime--;
        }
      }

      if (!onbreak && studytime <= 0) {
        beep();
        onbreak = true;
        breaktime = breakminutes * 60;
        studyStartMillis = millis();
      }

      if (onbreak && breaktime <= 0) {
        beep();
        sessions--;

        if (sessions <= 0) {
          studying = false;
          paused = false;
          onbreak = false;
          sessions = 1;
        } else {
          onbreak = false;
          studytime = studyminutes * 60;
          studyStartMillis = millis();
        }
      }
    }

    if (paused) {
      if (okPressed) {
        paused = false;
        studying = true;
        studyStartMillis = millis();
      }

      if (backPressed) {
        studying = false;
        paused = false;
        onbreak = false;
        studytime = studyminutes * 60;
        breaktime = breakminutes * 60;
      }
    }
  }

  if (currentScreen==CLOCK) {
    // showtime // unfinished
  }
  //SETTINGS
  if (currentScreen==SETTINGS) {
    if (upPressed) {
      if (settingsselection<3) {
          settingsselection++;
      }
      else {
          settingsselection=0;
      }
    }
    if (downPressed) {
        if (settingsselection>0) {
            settingsselection--; 
        }
      else {
          settingsselection=3;
      }
    }

    if (okPressed) {
      if (settingsselection==0) {
        currentScreen=SETTINGS;
      }
      if (settingsselection==1) {
        currentScreen=SET_STUDY;
      }
      if (settingsselection==2) {
        currentScreen=SET_ALARM;
      }
      if (settingsselection==3) {
        currentScreen=SET_CLOCK;
      }
    }

    if (backPressed) {
      currentScreen=CLOCK;
    }
  } 

  //SET POMODORO
  if (currentScreen==SET_STUDY) {
    if (choosestudy) {
      if (upPressed) {
        studyminutes++;
      }
      if (downPressed) {
        if (studyminutes>0) {
          studyminutes--; 
        }
      }
      if (okPressed) {
        choosestudy=false;
        choosebreak=true;
      }
    }
    if (choosebreak) {
      if (upPressed) {
        breakminutes++;
      }
      if (downPressed) {
        if (breakminutes>0) {
          breakminutes--; 
        }
      }
      if (okPressed) {
        choosestudy=true;
        choosebreak=false;
        currentScreen=CLOCK; // todo: toggle alarm on/off
      }
    }
  }
  //SET ALARM
  if (currentScreen==SET_ALARM) {
    if (choosehour) {
      if (upPressed) {
        if (alarmhour<23) {
          alarmhour++;
        }
        else {
          alarmhour=0;
        }
      }
      if (downPressed) {
        if (alarmhour>0) {
          alarmhour--; 
        }
        else {
            alarmhour=23;
        }
      }
      if (okPressed) {
        choosehour=false;
        chooseminute=true;
      }
    }
    if (chooseminute) {
      if (upPressed) {
        if (alarmminute<59) {
          alarmminute++;
        }
        else {
          alarmminute=0;
        }
      }
      if (downPressed) {
        if (alarmminute>0) {
          alarmminute--; 
        }
        else {
            alarmminute=59;
        }
      }
      if (okPressed) {
        choosehour=true;
        chooseminute=false;
        currentScreen=CLOCK; // todo: toggle alarm on/off
      }
    }
  }
  //SET CLOCK
  if (currentScreen==SET_CLOCK) {
    if (choosehour) {
      if (upPressed) {
        if (clockhour<23) {
          clockhour++;;
        }
        else {
          clockhour=0;
        }
      }
      if (downPressed) {
        if (clockhour>0) {
          clockhour--; 
        }
        else {
            clockhour=23;
        }
      }
      if (okPressed) {
        choosehour=false;
        chooseminute=true;
      }
    }
    if (chooseminute) {
      if (upPressed) {
        if (clockminute<59) {
          clockminute++;
        }
        else {
          clockminute=0;
        }
      }
      if (downPressed) {
        if (clockminute>0) {
          clockminute--; 
        }
        else {
            clockminute=59;
        }
      }
      if (okPressed) {
        choosehour=true;
        chooseminute=false;
        clockStartMillis = millis();
        currentScreen=CLOCK;
      }
    }
  }
  int totalMinutes = clockhour * 60 + clockminute + (millis() - clockStartMillis) / 60000UL;
  int currentHour = (totalMinutes / 60) % 24;
  int currentMinute = totalMinutes % 60;
  if (alarmhour == currentHour && alarmminute == currentMinute) {
    beep();
  }
  drawScreen();
  delay(150);
}

void drawScreen() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);

  if (currentScreen == CLOCK) {
    int totalMinutes = clockhour * 60 + clockminute + (millis() - clockStartMillis) / 60000UL;
    int currentHour = (totalMinutes / 60) % 24;
    int currentMinute = totalMinutes % 60;    
    if (currentHour < 10) tft.print("0");
    tft.print(currentHour);
    tft.print(":");
    if (currentMinute < 10) tft.print("0");
    tft.print(currentMinute);

  }

  if (currentScreen == MODE_SELECT) {
    tft.print("MODE");
    tft.setCursor(0, 30);

    if (modeselection == 0) tft.print("> Clock");
    if (modeselection == 1) tft.print("> Study");
    if (modeselection == 2) tft.print("> Settings");
  }

  if (currentScreen == STUDY) {
    if (currentScreen == STUDY) {
      if (onbreak) tft.print("BREAK");
      else tft.print("STUDY");

      int displayTime = onbreak ? breaktime : studytime;
      int mins = displayTime / 60;
      int secs = displayTime % 60;

      tft.setCursor(0, 30);
      if (mins < 10) tft.print("0");
      tft.print(mins);
      tft.print(":");
      if (secs < 10) tft.print("0");
      tft.print(secs);

      tft.setCursor(0, 55);
      tft.print("Sess:");
      tft.print(sessions);
    }
  }

  if (currentScreen == SETTINGS) {
    tft.print("SETTINGS");
    tft.setCursor(0, 30);

    if (settingsselection == 0) tft.print("> Menu");
    if (settingsselection == 1) tft.print("> Pomodoro");
    if (settingsselection == 2) tft.print("> Alarm");
    if (settingsselection == 3) tft.print("> Clock");
  }

  if (currentScreen == SET_STUDY) {
    tft.print("POMODORO");
    tft.setCursor(0, 30);
    tft.print("Study:");
    tft.print(studyminutes);
    tft.setCursor(0, 55);
    tft.print("Break:");
    tft.print(breakminutes);
  }

  if (currentScreen == SET_ALARM) {
    tft.print("ALARM");
    tft.setCursor(0, 30);
    tft.print(alarmhour);
    tft.print(":");
    tft.print(alarmminute);
  }

  if (currentScreen == SET_CLOCK) {
    tft.print("SET CLOCK");
    tft.setCursor(0, 30);
    tft.print(clockhour);
    tft.print(":");
    tft.print(clockminute);
  }
}

void beep() {
  digitalWrite(BUZZER, HIGH);
  delay(150);
  digitalWrite(BUZZER, LOW);
}