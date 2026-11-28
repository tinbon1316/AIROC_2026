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
void re_phai(int goc_phai)
{
  LbMotion.runLRrpm(59, -59);
  LbMotion.waitRotationDeg(goc_phai);
  LbMotion.stopAndWait();
}
void di_thang(int kc_thang)
{
  LbMotion.runLRrpm(59, 59);
  LbMotion.waitDistanceMm(kc_thang * 10);
  LbMotion.stopAndWait();
}
void di_lui(int kc_thang)
{
  LbMotion.runLRrpm(-59, -59);
  LbMotion.waitDistanceMm(kc_thang * 10);
  LbMotion.stopAndWait();
}

void re_trai(int goc_trai)
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

void start()
{
    di_thang(7);
    re_trai(60);

    distance = 50;
    followLine();
}

void den_A4()
{
    re_phai(25);
    di_thang(6);

    mau = Obj_detectColor();
    LbDelay(500);
}

void xu_ly_A4()
{
    if (mau == "Blue")
    {
        di_lui(6);
        re_trai(110);

        distance = 300;
        followLine();

        re_trai(60);
        
        distance = 540;
        followLine();

        re_phai(16);
        di_thang(10);
        LbGripper.moveTo(50);
        di_lui(10);
        re_phai(120);

        distance = 540;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else if (mau == "Green")
    {
        di_lui(6);
        re_trai(110);

        distance = 300;
        followLine();
        
        re_trai(60);
        
        distance = 300;
        followLine();

        re_phai(60);

        distance = 200;
        followLine();

        re_trai(16);
        di_thang(10);
        LbGripper.moveTo(50);
        di_lui(10);
        re_trai(120);

        distance = 200;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else if (mau == "Purple")
    {
        di_lui(6);
        re_trai(110);

        distance = 300;
        followLine();
        
        re_trai(60);
        
        distance = 300;
        followLine();

        re_phai(60);

        distance = 300;
        followLine();

        LbGripper.moveTo(50);
        re_trai(155);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else
    {
        // Red / Yellow

        LbGripper.moveTo(55);
        di_lui(6);
        re_trai(110);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(25);
        di_thang(10);
        LbGripper.moveTo(0);

        // Ve diem chuan P1
        di_lui(10);
        re_phai(110);

        distance = 300;
        followLine();
    }
}

void den_A2()
{
    re_trai(25);
    di_thang(6);

    mau = Obj_detectColor();
    LbDelay(500);
}

void xu_ly_A2()
{
    if (mau == "Blue")
    {
        di_lui(6);
        re_phai(25);

        distance = 300;
        followLine();

        re_phai(60);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 540;
        followLine();

        re_phai(16);
        di_thang(10);
        LbGripper.moveTo(60);
        di_lui(10);
        re_phai(120);

        distance = 540;
        followLine();

        re_phai(60);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(110);
        
        LbGripper.moveTo(0);
        di_thang(13);
        // Ve diem chuan P1
        di_lui(13);
        re_phai(25);
    }

    else if (mau == "Green")
    {
        di_lui(6);
        re_phai(25);

        distance = 630;
        followLine();

        re_phai(60);

        distance = 540;
        followLine();

        re_trai(16);
        di_thang(10);
        LbGripper.moveTo(60);
        di_lui(10);
        re_trai(120);

        distance = 540;
        followLine();

        re_trai(60);

        distance = 630;
        followLine();

        re_phai(110);
        
        LbGripper.moveTo(0);
        di_thang(13);
        // Ve diem chuan P1
        di_lui(13);
        re_phai(25);
    }

    else if (mau == "Purple")
    {
        di_lui(6);
        re_phai(25);

        distance = 630;
        followLine();

        re_phai(60);

        distance = 630;
        followLine();

        LbGripper.moveTo(60);
        re_trai(155);

        distance = 630;
        followLine();

        re_trai(60);

        distance = 630;
        followLine();

        re_phai(110);
        
        LbGripper.moveTo(0);
        di_thang(13);
        // Ve diem chuan P1
        di_lui(13);
        re_phai(25);
    }

    else
    {
        // Red / Yellow

        LbGripper.moveTo(60);
        di_lui(6);
        re_phai(25);

        distance = 780;
        followLine();

        re_phai(60);
        di_thang(43);
        LbGripper.moveTo(0);

        // Ve diem chuan P1
        di_lui(43);
        re_phai(60);

        distance = 780;
        followLine();
        re_phai(155);
    }
}


void den_A3()
{
  // Tiep can khoi o nhiem C1
  re_trai(25);
  di_thang(6);

  mau = Obj_detectColor();
  LbDelay(500);
}
void xu_ly_A3()
{

  if (mau == "Purple")
  {
    // C1 - PURPLE
    // Lay khoi trung hoa tai S3

    // Lui khoi vi tri C1
    di_lui(6);
    re_trai(110);

    // Di ve huong khu luu tru S3
    distance = 300;
    followLine();

    re_phai(60);
    
    distance = 950;
    followLine();

    LbGripper.moveTo(60);
    re_trai(155);

    // Mang khoi Purple quay ve khu C
    distance = 950;
    followLine();

    re_trai(60);
    
    distance = 300;
    followLine();

    // Dua khoi trung hoa vao khu C
    re_trai(25);
    LbGripper.moveTo(0);
    di_thang(13);

    // Di ve diem chuan P3
    di_lui(13);
    re_trai(25);
  }

  else if (mau == "Green")
  {
    di_lui(6);
    re_trai(110);

    // Di ve huong khu luu tru S3
    distance = 300;
    followLine();

    re_phai(60);
    
    distance = 880;
    followLine();

    re_trai(16);
    di_thang(10);
    LbGripper.moveTo(60);
    di_lui(10);
    re_trai(110);

    // Mang khoi Purple quay ve khu C
    distance = 880;
    followLine();

    re_trai(60);
    
    distance = 300;
    followLine();

    // Dua khoi trung hoa vao khu C
    re_trai(25);
    LbGripper.moveTo(0);
    di_thang(13);
    // Di ve diem chuan P3
    di_lui(13);
    re_trai(25);
  }

  else if (mau == "Blue")
  {
    // C1 - BLUE: lay khoi trung hoa tai S4

    di_lui(6);
    re_trai(25);

    // Di ve huong khu luu tru S3
    distance = 630;
    followLine();

    re_trai(60);
    
    distance = 550;
    followLine();

    re_phai(16);
    di_thang(10);
    LbGripper.moveTo(60);
    di_lui(10);
    re_phai(120);
    
    distance = 550;
    followLine();

    re_phai(60);
    
    distance = 630;
    followLine();

    // Dua khoi trung hoa vao khu C
    re_trai(110);
    LbGripper.moveTo(0);
    di_thang(13);

    // Di ve diem chuan P3
    di_lui(13);
    re_trai(25);
  }

  else
  {
    // C1 - RED / YELLOW
    // Gap khoi o nhiem dua ve CRL

    LbGripper.moveTo(60);
    di_lui(6);
    re_trai(110);

    // Ra duong line tu khu C
    distance = 460;
    followLine();

    // Di ve phia CRL
    re_phai(60);

    di_thang(80);
    LbGripper.moveTo(0);

    // Lui ra khoi CRL
    di_lui(80);
    re_phai(60);

    distance = 460;
    followLine();
  
    re_trai(60);
  }
}
void den_A1()
{
  // Tu P3 tiep can E2
  re_phai(25);
  di_thang(22);

  mau = Obj_detectColor();
  LbDelay(500);
}

void xu_ly_A1()
{

  if (mau == "Blue")
    {
        LbGripper.moveTo(50);
        di_lui(6);
        LbGripper.moveTo(0);
        di_lui(12);
        re_trai(110);

        distance = 300;
        followLine();

        re_trai(60);
        
        distance = 540;
        followLine();

        re_phai(16);
        di_thang(10);
        LbGripper.moveTo(50);
        di_lui(10);
        re_phai(120);

        distance = 540;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else if (mau == "Green")
    {
        LbGripper.moveTo(50);
        di_lui(6);
        LbGripper.moveTo(0);
        di_lui(12);
        re_trai(110);

        distance = 300;
        followLine();
        
        re_trai(60);
        
        distance = 300;
        followLine();

        re_phai(60);

        distance = 200;
        followLine();

        re_trai(16);
        di_thang(10);
        LbGripper.moveTo(50);
        di_lui(10);
        re_trai(120);

        distance = 200;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else if (mau == "Purple")
    {
        LbGripper.moveTo(50);
        di_lui(6);
        LbGripper.moveTo(0);
        di_lui(12);
        re_trai(110);

        distance = 300;
        followLine();
        
        re_trai(60);
        
        distance = 300;
        followLine();

        re_phai(60);

        distance = 300;
        followLine();

        LbGripper.moveTo(50);
        re_trai(155);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(60);
        
        distance = 300;
        followLine();

        re_trai(25);
        di_thang(13);
        di_lui(1);
        LbGripper.moveTo(0);
        di_lui(10);
        // Ve diem chuan P1
        
        re_trai(110);

        distance = 300;
        followLine();

        re_phai(60);
    }

    else
    {
        // Red / Yellow

        LbGripper.moveTo(55);
        di_lui(22);
        re_trai(110);

        distance = 300;
        followLine();

        re_trai(60);

        distance = 300;
        followLine();

        re_phai(25);
        di_thang(10);
        LbGripper.moveTo(0);

        // Ve diem chuan P1
        di_lui(10);
        re_phai(110);

        distance = 300;
        followLine();
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
  den_A1();
  xu_ly_A1();
  LbMission.end();
}