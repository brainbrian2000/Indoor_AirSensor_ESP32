#ifndef _OLED096_FUNCTION_CPP
#define _OLED096_FUNCTION_CPP
    #include <Adafruit_SSD1306.h>
    #include <Adafruit_GFX.h>
    #include <Wire.h>
    #define OLED_WIDTH 128
    #define OLED_HEIGHT 64
    #define OLED_ADDR 0x3C
    #define OLED_RESET -1
class OLED096{
    public:
        bool LCD=false;
        Adafruit_SSD1306 *display;
        void init(){
            display = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
            if (!(*display).begin(SSD1306_SWITCHCAPVCC, 0x3C))
                {
                    LCD = false;
                    Serial.println("display disabled");
                    return;
                }
            else
                {
                    LCD = true;
                    return;
                }

        }

        void print_oled(String info){
            if(LCD == false){
                Serial.println("[Warning]LCD not Working.");
                return;
            }

        // Serial.println("displaying...");
            display->clearDisplay();
            display->setTextSize(1);
            display->setTextColor(SSD1306_WHITE);
            display->setCursor(0, 0);
            display->print(info);
            // Serial.println(info); 
            display->display();
        return;
        }
    

};

#endif