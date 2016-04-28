/*
 * Poseidon tremolo
 * Copyright 2016 - Christian Roring
 */

#include "Arduino.h"
#include "WaveGenerator.h"
#include "Rotary.h"
#include "MedusaDisplay.h"
#include "MenuSystem.h"

unsigned int bpm = 60;
unsigned int depth = 100; // set depth
int wave = 0;
float mult = 1;
int mod = 10; // mod depth
unsigned int lfo = 8;

int maxDisplayDepth = 99;
int minDisplayDepth = 0;
int displayDepth = map(depth, 255, 0, minDisplayDepth, maxDisplayDepth);

int button_pin = 3;
int buttonState = 0;
int debounce = 0;

Menu m_root("ROOT");
Menu m_trem("TREM");
Menu m_prog("PROG");
Menu m_conf("CONF");
MenuItem t_bpm("BPM ");
MenuItem t_dept("DEPT");
MenuItem t_wave("WAVE");
MenuItem t_multi("MULT");
MenuItem t_mod("MOD ");
MenuItem p_p1("PR 1");
MenuItem p_p2("PR 2");
MenuItem p_p3("PR 3");
MenuItem p_p4("PR 4");
MenuItem c_mode("MODE");
MenuItem c_exp("EXP ");
MenuItem c_cal("CAL ");
MenuItem m_exit("EXIT");

Rotary r = Rotary(2, 4);
WaveGenerator waveGenerator = WaveGenerator(bpm, &depth, wave, mult, &mod);
MedusaDisplay medusaDisplay;
MenuSystem poseidonMenu;

char pedalName[] = "   POSEIDON   ";

void bootAnimation();
void setupMenu();
void displayMenuItem();
void toDisplay(char*);

void on_selected(MenuItem* _menuItem)
{
    // Serial.println("selected");
}

void back(MenuItem* _menuItem)
{
    poseidonMenu.reset();
}

void setup()
{
    medusaDisplay.begin(0x70, 1);
    medusaDisplay.clear();

    pinMode(button_pin, INPUT_PULLUP);

    bootAnimation();
    setupMenu();

    delay(1000);

    // set the prescaler
    TCCR1B = _BV(CS10);
}

void loop() {
    buttonState = digitalRead(button_pin);

    if (buttonState == LOW && debounce == 0) {
        poseidonMenu.select();
        displayMenuItem();
        debounce = 1;
    }
    if (buttonState == HIGH && debounce == 1) {
        debounce = 0;
    }

    unsigned char result = r.process();
    if (result == DIR_NONE) {
      // do nothing
    }
    else if (result == DIR_CW) {
        poseidonMenu.next();
        displayMenuItem();
    }
    else if (result == DIR_CCW) {
        poseidonMenu.prev();
        displayMenuItem();
    }
}

void bootAnimation()
{
    delay(1000);
    int counter =0;
    char buf[4];
    while (counter < 11) {
        buf[0] = pedalName[counter];
        buf[1] = pedalName[counter + 1];
        buf[2] = pedalName[counter + 2];
        buf[3] = pedalName[counter + 3];

        medusaDisplay.writeDisplay(buf);
        counter ++;

        delay(200);
    }

    medusaDisplay.clear();
}

void setupMenu()
{
    m_root.add_menu(&m_trem);
    m_root.add_menu(&m_prog);
    m_root.add_menu(&m_conf);

    m_trem.add_item(&t_bpm, &on_selected);
    m_trem.add_item(&t_dept, &on_selected);
    m_trem.add_item(&t_wave, &on_selected);
    m_trem.add_item(&t_multi, &on_selected);
    m_trem.add_item(&t_mod, &on_selected);
    m_trem.add_item(&m_exit, &back);

    m_prog.add_item(&p_p1, &on_selected);
    m_prog.add_item(&p_p2, &on_selected);
    m_prog.add_item(&p_p3, &on_selected);
    m_prog.add_item(&p_p4, &on_selected);
    m_prog.add_item(&m_exit, &back);

    m_conf.add_item(&c_mode, &on_selected);
    m_conf.add_item(&c_exp, &on_selected);
    m_conf.add_item(&c_cal, &on_selected);
    m_conf.add_item(&m_exit, &back);

    poseidonMenu.set_root_menu(&m_root);
    //end menu

    displayMenuItem();
}

void displayMenuItem()
{
    medusaDisplay.writeDisplay((char*)poseidonMenu.get_current_menu()->get_selected()->get_name());
}

void toDisplay(char* _name)
{
    medusaDisplay.writeDisplay(_name);
}
