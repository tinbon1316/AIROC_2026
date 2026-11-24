#include <Leanbot.h>
#include <Arduino_APDS9960.h>
int ObjRed, ObjGreen, ObjBlue;
int distance;
int target;
String mau;
int line;
int speed;
int test;
// ==================================================
// ĐỌC RGB — GIỮ CÁCH ĐO CỦA BẠN
// ==================================================
void _coreRead(int &r, int &g, int &b, CRGB ledColor)
{
  LbRGB.fillColor(ledColor);
  LbRGB.show();

  while (!APDS.colorAvailable())
    ;

  APDS.readColor(r, g, b);
}

void Obj_readRGB()
{
  int rRed, gGreen, bBlue;
  int rr, gg, bb;

  int originalBrightness = LbRGB.getBrightness();
  LbRGB.setBrightness(255);

  _coreRead(rRed, gg, bb, CRGB::Red);
  _coreRead(rr, gGreen, bb, CRGB::Lime);
  _coreRead(rr, gg, bBlue, CRGB::Blue);
  _coreRead(rr, gg, bb, CRGB::Black);

  LbRGB.setBrightness(originalBrightness);

  ObjRed = rRed - rr;
  ObjGreen = gGreen - gg;
  ObjBlue = bBlue - bb;
}

// ==================================================
// NHẬN DIỆN MÀU
// ==================================================
boolean BasicObj_isRed()
{
  if (ObjRed <= ObjGreen)
    return false;
  if (ObjRed <= ObjBlue)
    return false;
  if (ObjRed <= 3)
    return false;

  return true;
}

boolean BasicObj_isGreen()
{
  if (ObjGreen <= ObjRed)
    return false;
  if (ObjGreen <= ObjBlue)
    return false;
  if (ObjGreen <= 3)
    return false;

  return true;
}

boolean BasicObj_isPurple()
{
  if (ObjBlue <= ObjRed)
    return false;
  if (ObjBlue <= ObjGreen)
    return false;
  if (ObjBlue <= 3)
    return false;

  if (ObjRed <= 0)
    return false;
  if (ObjGreen <= 0)
    return false;

  // Purple: G < 2 × R.
  if ((long)ObjGreen >= 2L * ObjRed)
    return false;

  return true;
}

boolean BasicObj_isBlue()
{
  if (ObjBlue <= ObjRed)
    return false;
  if (ObjBlue <= ObjGreen)
    return false;
  if (ObjBlue <= 3)
    return false;

  if (ObjRed <= 0)
    return false;
  if (ObjGreen <= 0)
    return false;

  // Blue: G >= 2 × R.
  if ((long)ObjGreen < 2L * ObjRed)
    return false;

  return true;
}

// ==================================================
// ĐỌC MÀU VÀ HIỂN THỊ LÊN LED
// ==================================================
String Obj_detectColor()
{
  Obj_readRGB();

  if (BasicObj_isGreen())
  {
    LbRGB.fillColor(CRGB::Green);
    LbRGB.show();
    return "Green";
  }
  else if (BasicObj_isPurple())
  {
    LbRGB.fillColor(CRGB::Purple);
    LbRGB.show();
    return "Purple";
  }
  else if (BasicObj_isBlue())
  {
    LbRGB.fillColor(CRGB::Blue);
    LbRGB.show();
    return "Blue";
  }
  else
  {
    LbRGB.fillColor(CRGB::Red);
    LbRGB.show();
    return "Red";
  }
  LbRGB.fillColor(CRGB(50, 50, 50));
  LbRGB.show();
  return "Unknown";
}

// Describe this function...
void RP(int goc_phai)
{
  LbMotion.runLRrpm(59, -59);
  LbMotion.waitRotationDeg(goc_phai);
  LbMotion.stopAndWait();
}
void DT(int kc_thang)
{
  LbMotion.runLRrpm(59, 59);
  LbMotion.waitDistanceMm(kc_thang * 10);
  LbMotion.stopAndWait();
}
void DL(int kc_thang)
{
  LbMotion.runLRrpm(-59, -59);
  LbMotion.waitDistanceMm(kc_thang * 10);
  LbMotion.stopAndWait();
}

void RT(int goc_trai)
{
  LbMotion.runLRrpm(-59, 59);
  LbMotion.waitRotationDeg(goc_trai);
  LbMotion.stopAndWait();
}

// ==================================================
// DO LINE MOI - GHEP NGUYEN BO DIEU KHIEN THEO KHOANG CACH
// (khong dung Junction, khong doi cac lenh trong nhiem vu)
// ==================================================

// Dieu khien hai banh theo gia tri 4 cam bien line.
void runFollowLine(int lineValue) {
  LbIRLine.displayOnRGB(0x00ff00);

  switch (lineValue) {
    // Lech trai nhe
    case 0b0100:
      LbMotion.runLR(speed / 2, speed);
      break;

    // Lech phai nhe
    case 0b0010:
      LbMotion.runLR(speed, speed / 2);
      break;

    // Lech trai manh
    case 0b1100:
      LbMotion.runLR(0, speed);
      break;

    // Lech phai manh
    case 0b0011:
      LbMotion.runLR(speed, 0);
      break;

    // Cua trai gat
    case 0b1000:
      LbMotion.runLR(-speed / 2, speed / 2);
      break;

    // Cua phai gat
    case 0b0001:
      LbMotion.runLR(speed / 2, -speed / 2);
      break;

    // Cac trang thai khac: di thang (giu nhu code do line ban cung cap)
    default:
      LbMotion.runLR(speed, speed);
      break;
  }
}

// Do line theo quang duong encoder (mm).
void followLineDistance(int distanceMm) {
  int targetMm = LbMotion.getDistanceMm() + distanceMm;

  while (LbMotion.getDistanceMm() < targetMm) {
    LbIRLine.read();
    int lineValue = LbIRLine.value();
    runFollowLine(lineValue);
  }

  LbMotion.stopAndWait();
}

// Giu nguyen giao dien cu: distance = 300; followLine();
void followLine() {
  followLineDistance(distance);
}

void den_diemchuan1()
{
  DT(11);
  RT(25);
  distance = 300;
  followLine();
  RT(60);
  distance = 300;
  followLine();


}

void den_H1()
{
  RP(60);
  distance = 200;
  followLine();
  RP(60);
  DT(3);

  mau = Obj_detectColor();
  LbDelay(500);
  DL(3);

  RP(60);
  distance = 200;
  followLine();


  

}

void xu_ly_H1()
{
    if (mau == "Blue")
    {
      

      distance = 380;
      followLine();
      RT(60);
      DT(30);

       
      LbGripper.moveTo(60);
      DL(30);
      RT(60);
      distance = 580;
      followLine();
      RP(60);
      DT(7);
      LbGripper.moveTo(0);
      DL(7);
      RP(60);
      DT(18);
      RP(60);



        
    }

    else if (mau == "Green")
    {
      
      

      distance = 470;
      followLine();
      RT(12);
      DT(15);
      LbGripper.moveTo(60);
      DL(15);
      RT(128);
      distance = 680;
      followLine();
      RP(60);
      DT(7);
      LbGripper.moveTo(0);
      DL(7);
      RP(60);
      DT(18);
      RP(60);


        
    }

    else if (mau == "Purple")
    {
      distance = 630;
      followLine();
      LbGripper.moveTo(60);
      DL(83);
      RT(60);
      DT(7);
      LbGripper.moveTo(0);
      DL(7);
      RP(60);
      DT(18);
      RP(60);


       
    }

    else
    {
     // Red / Yellow
      DL(20);
      RT(60);
      DT(3);
      LbGripper.moveTo(60);
      DL(3);
      RP(60);
      distance = 530;
      followLine();
      RT(25);
      DT(25);
      LbGripper.moveTo(0);
      DL(25);
      RT(110);
      distance = 300;
      followLine();
      RT(60);





        

        
    }
}

void den_F3()
{
  
  

  mau = Obj_detectColor();
  LbDelay(500);
}

void xu_ly_F3()
{

  if (mau == "Purple")
  {
    
  }
  else if (mau == "Green")
  {
    
  }

  else if (mau == "Blue")
  {
    
  }

  else
  {
    // RED / YELLOW / MYSTERY - F3
    // Gap khoi o nhiem va dua ve CRL

    
  }
}
void den_C1()
{
  // Tiep can khoi o nhiem C1
  

  mau = Obj_detectColor();
  LbDelay(500);
}
void xu_ly_C1()
{

  if (mau == "Purple")
  {
    // C1 - PURPLE
    // Lay khoi trung hoa tai S3

    // Lui khoi vi tri C1
    
  }

  else if (mau == "Green")
  {
    // C1 - GREEN: lay khoi trung hoa tai S6

    
  }

  else if (mau == "Blue")
  {
    // C1 - BLUE: lay khoi trung hoa tai S4

    
  }

  else
  {
    // C1 - RED / YELLOW / MYSTERY
    // Gap khoi o nhiem dua ve CRL

    
  }
}
void den_E2()
{
  

  mau = Obj_detectColor();
  LbDelay(500);
}

void xu_ly_E2()
{

  if (mau == "Purple")
  {
    
    
  }

  else if (mau == "Green")
  {
    // E2 - GREEN: Lay khoi trung hoa tai S6

    // Roi vi tri E2
    
    LbMotion.stopAndWait();
  }

  else if (mau == "Blue")
  {
    // E2 - BLUE: Lay khoi trung hoa tai S4

    // Roi vi tri E2
    
    LbMotion.stopAndWait();
  }

  else
  {
    // RED / YELLOW / MYSTERY
    // Gap khoi E2
    
  }
}

void setup()
{
  Leanbot.begin(); // initialize Leanbot
  LbIRLine.doManualCalibration(TB1A);
  APDS.begin();
  LbDelay(100);
}

void loop()
{
  LbMission.begin(TB1A + TB1B);
  speed = 2000; // Cai dat mot lan cho toan bo cac doan followLine()
  den_diemchuan1();
    den_H1();
    xu_ly_H1();
  den_F3();
  xu_ly_F3();
    den_C1();
  xu_ly_C1();
  den_E2();
  xu_ly_E2();
  LbMission.end();
}