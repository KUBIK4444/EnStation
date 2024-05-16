/*
 * ENEMY CHASER GAME 2024
 * 
 * Game to train reactions and have fun :)
 * 
 * Vytvořil Kubik4444
 * 
 * Potřeby ke hraní:
 * - Arduino (nejlépe UNO, nebo MEGA)
 * - LCD 16x2 Shield nebo LCD 16x2 displej, při
 * použití displeje samotného je nutné připojit tlačítka na pin A0
 * stejně, jak to má LCD Shield 16x2.
 * 
 * Jak hrát hru:
 * - Pro start stiskněte SELECT
 * - Pro pohyb na herním poli použijte UP (nahoru) a DOWN (dolů)
 * - Pro získání informací stiskněte v menu RIGHT (doprava)
 * - Pro vypnutí podsvícení displeje stiskni tlačítko LEFT
 * - Zařízení kdykoliv restartuj pomocí tlačítka RST na displeji, nebo na desce Arduino
 * 
 * Pokročilé hraní: (dostupné pouze pro desky MEGA)
 * Připojte JoyStick a KcubeM2 modul na piny 20,21,A8-11 a připojte 5V a GND piny 
 * - gamepad se sám načte do hry
 * - pokud je správně připojen, rozsvítí se na něm dioda zeleně
 * - data o připojení se vypisují do sériového monitoru
 * - pro pokročilý výpis dat zadej příkaz testmode
 * 
 * Nepřepisuj a nekopíruj jakoukoliv část programu této hry!
 * 
 * Správnou verzi si zkontroluj tlačítkem RIGHT:
 * - enemy_chaser
 * - version_2_1
 * 
 * Automatické připojení joysticku na pinech 20,21,A8-11 + 5V a GND
 * Pokud jsi připojil joystick v průběhu hraní, zadej příkaz connect pro manuální připojení gamepadu
 */

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
 byte enemySpeed = 100; 
 byte eX = 15; //X coords of enemy
 byte eY; //Y coords of enemy

 int score = 0;
 int customVAR;
 
 const byte pX = 1; //X coords of player 
 byte pY = 0; //Y coords of player

 byte oX = 15; //Powerup X
 byte oY = 0; //Powerup Y

 int CLK = 0;

 bool GAMEPAD = false;
 bool testmode = false; //Vypisuje pokročilá data ze hry na sériový monitor.
 
bool PWRshown = false; //Jestli je zobrazený powerup

 #define wM "  Enemy Chaser  " //Pozdrav v hlavním menu
 #define wM2 "START BY SELECT " //Podnadpis (druhý řádek)
 #define wM3 "SETTING ON RIGHT" //Druhý podnadpis

 byte enemy[] = {
  B00000,
  B00100,
  B10101,
  B01110,
  B01110,
  B10101,
  B00100,
  B00000
};
byte player[] = {
  B01110,
  B01000,
  B01110,
  B01111,
  B01110,
  B01110,
  B01010,
  B01010
};
byte powerup[] = {
  B01110,
  B00100,
  B01110,
  B10101,
  B10111,
  B10001,
  B01110,
  B00000
};

bool menu = true;
bool play = false;
byte page = 0;

long rfsh = 0;

bool const_spd = false; //konstatní rychlost animace

int chance = 150; //šance na padnutí powerupu

bool bl = true;
bool slowmode = false;

byte game = 0; //0-main menu, 1-enemy chaser, 2-striker
byte tempgame = 1; //temp for menu selection

//Striker Variables:

int power = 20;
byte targetX = 0;
const byte targetY = 1;

byte friction = 5; //1-min,10-max

byte s_page = 0; //page ve strikeru
byte s_score = 0; //score ve strikeru

bool s_menu = false; //jestli jste ve strikeru v menu

byte s_bX = 0;

byte tol = 50; //tolerance střely

//COUNTDOWN variables :)

float time_set = 0;
float time_ela = 0;


 
 void setup() {
  Serial.begin(250000);
  pinMode(10,OUTPUT);
  pinMode(20,OUTPUT);
  pinMode(21,OUTPUT);

  randomSeed(millis());
  digitalWrite(10,HIGH);
  
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Select your game");
  lcd.setCursor(0,1);
  lcd.print("SLTD:");
  if(tempgame == 1){
    lcd.print("EnemyChaser");
  }else if(tempgame == 2){
    lcd.print("STRIKEr");
  }

  lcd.createChar(1,player);
  lcd.createChar(2,enemy);
  lcd.createChar(3,powerup);
  
  checkConnectedDVCs();
  
  //Hra automaticky připojí gamepad, který je správně připojený na pinech:
  // piny 20 a 21 - ovládání LED diody na modulu
  // piny A8 a A9 - čtení X a Y osy joysticku
  // pin A10 - čtení stavu tlačítka joysticku
  // pin A11 - pin, který ovládá, jestli Arduino joystick rozpozná

  //Problémy:

  //Gamepad je připojený, Arduino píše, že je připojený, ale nic nefunguje a nesvítí dioda.
  //Tím pádem je připojený jen pin A11. Zkontroluj ostatní piny a pin A20, ten ovládá zelenou diodu. Pokud Arduino říká, že je gamepad připojen, musí dioda svítit.

  //Gamepad se nepřipojuje automaticky
  //Zkontroluj pin A11. Ten zaručuje, že Arduino rozpozná připojené zařízení.

}

void loop() {

  //EVERYGAME STUFF HERE
if(GAMEPAD == true){
  digitalWrite(20,HIGH);
}else{
  digitalWrite(20,LOW);
}
if(analogRead(A10) < 5){
  digitalWrite(21,HIGH);
}else{
  digitalWrite(21,LOW);
}

  
if(bl == true){
  digitalWrite(10,HIGH);
}else{
  digitalWrite(10,LOW);
}

 //END OF THIS SH*T PLEASE :)
  
if(game == 0){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select your game");
  lcd.setCursor(0,1);
  lcd.print("SLTD:");
  if(tempgame == 1){
    lcd.print("EnemyChaser");
  }else if(tempgame == 2){
    lcd.print("STRIKEr");
  }else if(tempgame == 3){
    lcd.print("CountDown:)");
  }else if(tempgame == 4){
    lcd.print("InConnected");
  }
  delay(20);
  
  if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5){// SELECT
  game = tempgame;
  lcd.clear();
  delay(500);
  if(tempgame == 2){
    s_menu = true;
  }
}
if(analogRead(A0) > 400 && analogRead(A0) < 500 || analogRead(A9) > 800){//LEFT
    if(tempgame > 1){
      tempgame--;
      delay(200);
    }
  }
  if(analogRead(A0) < 50 || analogRead(A9) < 300){ //RIGHT
    if(tempgame < 4){
      tempgame++;
      delay(200);
    }
  }
}




//START here for enemy chaser - higher is menu thing or some device-wide stuff

  if(game == 1){
  if(slowmode == true){
    delay(500);
  }
  


if(Serial.available() > 0){
  cmdHandle();
}
if(testmode == true){
  Serial.println("Report by: GAMEPAD()");
  Serial.print("A8:");
  Serial.println(analogRead(A8));
  Serial.print("A9:");
  Serial.println(analogRead(A9));
  Serial.print("A10:");
  Serial.println(analogRead(A10));
  Serial.print("A11:");
  Serial.println(analogRead(A11));

}
if(page > 0){
  menu = false;
}



  
  if(analogRead(A0) < 50 && play == false || analogRead(A9) < 300 && GAMEPAD == true && play == false){ //RIGHT
   /* lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Game programmer:");
    lcd.setCursor(0,1);
    lcd.print("   Kubik4444   ");
    delay(3000);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("enemy_chaser");
    lcd.setCursor(0,1);
    lcd.print("version_2_0");
    delay(3000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RFs: ");
    lcd.print(rfsh);
    lcd.setCursor(0,1);
    lcd.print("JST: ");
    lcd.print(GAMEPAD);
    delay(3000);
  */
    if(page != 9){
    page++;
    }
  }
if(analogRead(A0) > 400 && analogRead(A0) < 500 && play == false|| play == false && analogRead(A9) > 800 && GAMEPAD == true){ //LEFT
    if(page != 0){
      page--;
    }
    if(page == 0){
      menu = true;
    }
  }

  if(page == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Game programmer:");
    lcd.setCursor(0,1);
    lcd.print("   Kubik4444   ");
  }else if(page == 2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("enemy_chaser");
    lcd.setCursor(0,1);
    lcd.print("version_2_1");
  }else if(page == 3){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RFs: ");
    lcd.print(rfsh);
    lcd.setCursor(0,1);
    lcd.print("JST: ");
    lcd.print(GAMEPAD);
  }else if(page == 4){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("A8:");
    lcd.print(analogRead(A8));
    lcd.print(" ");
    lcd.print("A9:");
    lcd.print(analogRead(A9));
    lcd.setCursor(0,1);
    lcd.print("A10:");
    lcd.print(analogRead(A10));
    lcd.print(" ");
    lcd.print("A11:");
    lcd.print(analogRead(A11));
  }else if(page == 5){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("UP TO SLOWMODE");
    lcd.setCursor(0,1);
    lcd.print("SLOWMODE: ");
    lcd.print(slowmode);
  }else if(page == 6){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PWR_CHANCE: ");
    lcd.print(chance);
    lcd.setCursor(0,1);
    lcd.print("Hold-fast scroll");
  }else if(page == 7){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Connect gamepad");
    lcd.setCursor(0,1);
    lcd.print("using the UP key");
  }else if(page == 8){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CON_SPD: ");
    lcd.print(const_spd);
    lcd.setCursor(0,1);
    lcd.print("CON_SPD_VAL: ");
    lcd.print(enemySpeed);
  }else if(page == 9){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SELECT TO EXIT");
    lcd.setCursor(0,1);
    lcd.print("CLICK TO EXIT");

  }



if(testmode == true){
  Serial.print("Page: ");
  Serial.println(page);
}
if(page > 0){
  delay(50);
}
if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(page == 5){
      slowmode = !slowmode;
      Serial.println("Slowmode byl prepnut.");
    }
  }
  if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(page == 6 && chance < 500){
      chance = chance + 10;
    }
  }
  if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(page == 8 && enemySpeed < 110){
      enemySpeed = enemySpeed + 5;
    }
  }
  if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){ // DOWN
    if(page == 8 && enemySpeed > 10){
      enemySpeed = enemySpeed - 5;
    }
  }
  if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true){ // SELECT
  if(page == 8){
      const_spd = !const_spd;
    }
}

if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true){ // SELECT
  if(page == 9){
      game = 0;
      page = 0;
      menu = true;
      lcd.clear();
      delay(2000);
    }
}

  
  if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(page == 7){
      Serial.println("Manualni pripojeni - pokus zahajen.");
      checkConnectedDVCs();
      
      if(GAMEPAD == true){
            lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gamepad uspesne");
    lcd.setCursor(0,1);
    lcd.print("pripojen ke hre.");
    delay(2000);
    lcd.clear();
    page = 6;
      
    }else{
    lcd.setCursor(0,0);
    lcd.print("Gamepad se nepo-");
    lcd.setCursor(0,1);
    lcd.print("darilo pripojit.");
    delay(2000);
    lcd.clear();
    page = 6;
    }
  }
  }

  if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){ // DOWN
    if(page == 6 && chance > 20){
      chance = chance - 10;
    }
  }


  CLK++;
if(menu == true){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(wM);
  lcd.setCursor(0,1);
  if(CLK < 50){
  lcd.print(wM2);
  }else if(CLK < 100){
    lcd.print(wM3);
  }else{
    lcd.print(wM3);
    CLK = 0;
  }
}
  if(analogRead(A0) > 400 && analogRead(A0) < 500){
    if(page == 0 && play == false){
    if(bl == true){
      bl = false;
    }else{
      bl = true;
    }
    delay(100);
    }
  }
  
if(menu == true && analogRead(A0) > 600 && analogRead(A0) < 700 || menu == true && analogRead(A10) < 5 && GAMEPAD == true){ // Pokud je stisknuto SELECT, začne hru
  score = 0;
  setGame();
  play = true;
  menu = false;
  if(const_spd != true){
  enemySpeed = 100;
  }
  lcd.clear();
}
if(play == true){
  if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(pY == 1){
      pY = 0;
    }
  }
  if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){ // DOWN
    if(pY == 0){
      pY = 1;
    }
  }
  if(testmode == true){
      Serial.println("Report by: GAME()");
      Serial.print("pY:");
      Serial.println(pY);
    }
    
  refresh();
}



delay(enemySpeed);
}
//end here for enemy chaser - start for STRIKEr

if(game == 2){
  draw_striker_menu:
 if(s_menu == true){
  if(s_page == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" STRIKEr - menu ");
    lcd.setCursor(0,1);
    lcd.print(" PLAY BY SELECT ");
    delay(20);
  }else if(s_page == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("striker_mega");
    lcd.setCursor(0,1);
    lcd.print("version_beta_1_1");
    delay(20);
  }else if(s_page == 2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FRICTION LEVEL:");
    lcd.setCursor(0,1);
    lcd.print(friction);
    lcd.print("/10");
    delay(20);
  }else if(s_page == 3){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SHOT TOLERANCE:");
    lcd.setCursor(0,1);
    lcd.print(tol);
    lcd.print("/70");
    delay(20);
  }
  
if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(s_page == 2){
      if(friction < 10){
      friction++;
      delay(200);
      }
    }
  }
  if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){ // DOWN
    if(s_page == 2){
      if(friction > 2){
        friction--;
        delay(200);
      }
    }
  }
if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){ // UP
    if(s_page == 3){
      if(tol < 70){
      tol++;
      delay(100);
      }
    }
  }
  if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){ // DOWN
    if(s_page == 3){
      if(tol > 10){
        tol--;
        delay(100);
      }
    }
  }



 if(analogRead(A0) > 400 && analogRead(A0) < 500 || analogRead(A9) > 800 && GAMEPAD == true){//LEFT
    if(s_page > 0){
      s_page--;
      delay(100);
    }
  }
  if(analogRead(A0) < 50 || analogRead(A9) < 300 && GAMEPAD == true){ //RIGHT
    if(s_page < 3){
      s_page++;
      delay(100);
    }
  }

  if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true){
    if(s_page == 0){
      s_menu = false;
      lcd.clear();
      delay(500);
    }
  }
    
 }else{

      power = 20;
      targetX = random(3,15);
      lcd.clear();
      lcd.setCursor(targetX,targetY);
      lcd.write(2);
      lcd.setCursor(0,1);
      lcd.write(1);

    checker:
    if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true){
      //jde to dál
    }else{
      
      lcd.setCursor(0,0);
      lcd.print("PWR: ");
      if(power < 100){
        lcd.print(" ");
      }
      lcd.print(power);
      delay(20);
      
      if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){
        if(power < 350){
          power=power+5;
        }
        
      }
      if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){
        if(power > 20){
          power=power-5;
        }
      }
      goto checker;
    }
    //mám nabito
    s_bX = 1;
    while(s_bX != targetX){
      
      lcd.setCursor(s_bX,1);
      lcd.print("o");
      if(s_bX > 1){
        lcd.setCursor(s_bX - 1,1);
        lcd.print(" ");
      }
      s_bX++;

      if(power < power - friction * 3){
        s_gameOver();
        goto draw_striker_menu;
      }

      
      power = power - friction * 3;
      lcd.setCursor(0,0);
      lcd.print("PWR: ");
      if(power < 100 && power > 9){
        lcd.print(" ");
      }else if(power < 10 || power > 250){
        s_gameOver();
        goto draw_striker_menu;
      }
      lcd.print(power);
    delay(1000);
    if(power < 1){
      s_gameOver();
      goto draw_striker_menu;
    }
      
    }
    if(power > 50){
      s_gameOver_p();
    }else{
      s_gameWon();
    }
    s_menu = true;
    goto draw_striker_menu;




    
  }
 }

if(game == 3){
  time_set = 0.05;
  checker2:
    if(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true){
      //jde to dál
    }else{
      
      lcd.setCursor(0,0);
      lcd.print("   TM SET: ");
      if(time_set < 10){
        lcd.print(" ");
      }
      lcd.print(time_set);
      delay(20);
      
      if(analogRead(A0) > 50 && analogRead(A0) < 150 || analogRead(A8) < 300 && GAMEPAD == true){
        if(time_set < 100){
          time_set=time_set+0.01;
        }
        
      }
      if(analogRead(A0) > 200 && analogRead(A0) < 300 || analogRead(A8) > 800 && GAMEPAD == true){
        if(time_set > 0.05){
          time_set=time_set-0.01;
        }
      }
      goto checker2;
    }
    while(time_set > 0){
      time_set = time_set - 0.004167;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CountDown: ");
      if(time_set < 10){
        lcd.print(" ");
      }
      lcd.print(time_set);
      delay(250);
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("BOO, BOO, BOOM!");
    lcd.setCursor(0,1);
    lcd.print("BOO, BOO, BOOM!");

    while(!(analogRead(A0) > 600 && analogRead(A0) < 700 || analogRead(A10) < 5 && GAMEPAD == true)){
    digitalWrite(10,LOW);
    delay(250);
    digitalWrite(10,HIGH);
    delay(250);
    }
    digitalWrite(10,HIGH);
    lcd.clear();
    game = 0;
    delay(2000);
    

  
}
//-----------END OF COUNTDOWN --- START OF INCONNECTED ----
if(game == 4){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PRESS UP TO");
  lcd.setCursor(0,1);
  lcd.print("CONNECT GAMEPAD");

  checker3:
    if(analogRead(A0) > 50 && analogRead(A0) < 150){
      checkConnectedDVCs();
      if(GAMEPAD == true){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gamepad uspesne");
    lcd.setCursor(0,1);
    lcd.print("pripojen ke hre.");
    delay(2000);
    lcd.clear();
      
    }else{
    lcd.setCursor(0,0);
    lcd.print("Gamepad se nepo-");
    lcd.setCursor(0,1);
    lcd.print("darilo pripojit.");
    delay(2000);
    lcd.clear();
    }
    game = 0;
    }else if(analogRead(A0) > 200 && analogRead(A0) < 300){

      game = 0;
      GAMEPAD = false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Gamepad byl");
      lcd.setCursor(0,1);
      lcd.print("odpojen ze hry.");

      delay(1000);
      lcd.clear();
      
    }else{
      goto checker3;
    }

    
}
//END OF INCONNECTED

}

void s_gameOver(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   GAME OVER!   ");
  lcd.setCursor(0,1);
  lcd.print("NOT ENOUGH POWER");
  delay(3000);
  s_menu = true;
}
void s_gameOver_p(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   GAME OVER!   ");
  lcd.setCursor(0,1);
  lcd.print("LITTLE TOO POWER");
  delay(3000);
  s_menu = true;
}
void s_gameWon(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("   GREAT WORK   ");
  lcd.setCursor(0,1);
  lcd.print(" YOU STRIKED IT ");
  delay(3000);
  s_menu = true;
}



void refresh(){
  rfsh++;
  lcd.clear();

  lcd.setCursor(pX,pY);
  lcd.write(1);

  lcd.setCursor(eX,eY);
  lcd.write(2);

  if(PWRshown == true){
  lcd.setCursor(oX,oY);
  lcd.write(3);

  oX = oX - 1;
  }
  
  eX = eX - 1;
  randomSeed(millis());
  int a = random(0,1);

if(a > 500){
    if(eY == 0){
      eY = 1;
    }else{
      eY = 0;
    }
}
  
  if(eX == 0){
    score++;
    setGame();
    if(const_spd != true){
    enemySpeed--;
    }
  }

  if(oX == 0){
    PWRshown = 0;
    oX = 15;
  }

  if(pX == oX && pY == oY && PWRshown == true){
    score = score + 5;
    PWRshown = false;
    if(const_spd != true){
    enemySpeed = enemySpeed + 25;
    }
    oX = 15;
  }
  
  if(eX == pX && eY == pY){
    gameOver();
  }
  randomSeed(millis());
  int c = random(0, chance);
  if(c == 1){
    PWRshown = true;
    int b = random(0,1);
    if(b == 1){
      oY = 1;
    }else{
      oY = 0;
    }
  }


if(testmode == true){
  Serial.println("Report by: REFRESH()");
  Serial.print("eX:");
  Serial.println(eX);
  Serial.print("eY:");
  Serial.println(eY);
  Serial.print("Score:");
  Serial.println(score);
  Serial.print("eSpeed:");
  Serial.println(enemySpeed);
}
}

void setGame(){
    randomSeed(millis());
    int i = random(0, 10000);

    
   
    
    if(i < 5000){
      eY = 0;
    }else{
      eY = 1;
    }
  

  eX = 15; //X coords of enemy


  
  
}

void gameOver(){
  lcd.clear();
  digitalWrite(21,HIGH);
  lcd.setCursor(0,0);
  lcd.print("   GAME OVER!   ");
  lcd.setCursor(0,1);
  lcd.print("    SCORE: ");
  lcd.print(score);
  delay(5000);
  digitalWrite(21,LOW);
  menu = true;
  play = false;
  lcd.clear();
}
void checkConnectedDVCs(){
  if(analogRead(A11) > 1000){
    GAMEPAD = true;
    Serial.println("GamePad pripojen uspesne. Zkontrolujte, zda dioda sviti zelene a uzijte si hrani.");
    
  }else{
    Serial.println("Zadne zarizeni nenalezeno. Zkontrolujte zapojeni kabelu podle instrukci, hlavne pin A11.");
  }

  
}
void cmdHandle(){
  String entered = Serial.readString();

  if(entered == "testmode"){
    testmode = !testmode;
    Serial.println("Testovaci rezim byl prepnut. Prijemnou hru.");
  }else if(entered == "connect"){
    checkConnectedDVCs();
  }else if(entered == "slowmode"){
    slowmode = !slowmode;
  }
}















//end
