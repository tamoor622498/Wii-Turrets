#include <Wire.h>
#include <ContinuousRotationServo.h>

ContinuousRotationServo topLeft;
ContinuousRotationServo topRight;
ContinuousRotationServo bottomLeft;
ContinuousRotationServo bottomRight;
ContinuousRotationServo upDown;
ContinuousRotationServo trigger;

static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

void setup()
{
  Serial.begin(9600);

  nunchuck_setpowerpins(); // use analog pins 2&3 as fake gnd & pwr
  nunchuck_init(); // send the initilization handshake
  topLeft.begin(11);
  topRight.begin(10);
  bottomLeft.begin(9);
  bottomRight.begin(6);
  upDown.begin(5);
  trigger.begin(3);
    //Serial.print ("Finished setup\n");
  }

  void loop()
{

  nunchuck_get_data();

  if (getJoyXAxis() < 100) {
    int val = -100;

    topLeft.rotate(val);
    topRight.rotate(val);
    bottomLeft.rotate(val);
    bottomRight.rotate(val);
  } else if (getJoyXAxis() > 130) {
    int val = 100;

    topLeft.rotate(val);
    topRight.rotate(val);
    bottomLeft.rotate(val);
    bottomRight.rotate(val);
  } else {
    topLeft.noMovement();
    topRight.noMovement();
    bottomLeft.noMovement();
    bottomRight.noMovement();
  }

  if (getJoyYAxis() < 100) {
    int val = -100;
    upDown.rotate(val);
  } else if (getJoyYAxis() > 150) {
    int val = 100;
    upDown.rotate(val);
  } else {
    upDown.rotate(-14);
  }

  if (getZButton()) {
  trigger.rotate(100);
  } else {
    trigger.rotate(3);
  }

    if (getCButton()) {
  trigger.rotate(-100);
  } else {
    trigger.rotate(3);
  }



  //  JSONencoder["xaxis"] = getJoyXAxis();
  //  JSONencoder["yaxis"] = getJoyYAxis();
  //
  //  JSONencoder["C"] = getCButton();
  //  JSONencoder["Z"] = getZButton();
  //
  //  serializeJson(JSONencoder, Serial);

  //  Serial.print("{");
  //  Serial.print("\"NUNCHUK\":");
  //  JSONencoder.printTo(Serial);
  //  Serial.println("}");


  //  Serial.print(",");
  //  Serial.print(getJoyXAxis());
  //
  //  Serial.print(",");
  //  Serial.print(getJoyYAxis());
  //
  //  Serial.print(",");
  //  Serial.print(getZButton());
  //
  //  Serial.print(",");
  //  Serial.println(getCButton());
  //
  // un-comment next line to print data to serial monitor
  nunchuck_print_data();
}


//
// Nunchuck functions
//

// Uses port C (analog in) pins as power & ground for Nunchuck
static void nunchuck_setpowerpins()
{
#define pwrpin PORTC3
#define gndpin PORTC2
  DDRC |= _BV(pwrpin) | _BV(gndpin);
  PORTC &= ~ _BV(gndpin);
  PORTC |=  _BV(pwrpin);
  delay(100);  // wait for things to stabilize
}

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
void nunchuck_init()
{
  Wire.begin();                      // join i2c bus as master
  Wire.beginTransmission(0x52);     // transmit to device 0x52
  Wire.write(0x40);            // sends memory address
  Wire.write(0x00);            // sends sent a zero.
  Wire.endTransmission();     // stop transmitting
}

// Send a request for data to the nunchuck
// was "send_zero()"
void nunchuck_send_request()
{
  Wire.beginTransmission(0x52);     // transmit to device 0x52
  Wire.write(0x00);            // sends one byte
  Wire.endTransmission();     // stop transmitting
}

// Receive data back from the nunchuck,
int nunchuck_get_data()
{
  int cnt = 0;
  Wire.requestFrom (0x52, 6);     // request data from nunchuck
  while (Wire.available ()) {
    // receive byte as an integer
    nunchuck_buf[cnt] = nunchuk_decode_byte(Wire.read());
    cnt++;
  }
  nunchuck_send_request();  // send request for next data payload
  // If we recieved the 6 bytes, then go print them
  if (cnt >= 5) {
    return 1;   // success
  }
  return 0; //failure
}

int getJoyXAxis() {
  return nunchuck_buf[0];
}

int getJoyYAxis() {
  return nunchuck_buf[1];
}

int getZButton() {
  return !((nunchuck_buf[5] >> 0) & 1);
}

int getCButton() {
  return !((nunchuck_buf[5] >> 1) & 1);
}

// Print the input data we have recieved
// accel data is 10 bits long
// so we read 8 bits, then we have to add
// on the last 2 bits.  That is why I
// multiply them by 2 * 2
void nunchuck_print_data()
{
  static int i = 0;
  int joy_x_axis = nunchuck_buf[0];
  int joy_y_axis = nunchuck_buf[1];

  int accel_x_axis = nunchuck_buf[2]; // * 2 * 2;
  int accel_y_axis = nunchuck_buf[3]; // * 2 * 2;
  int accel_z_axis = nunchuck_buf[4]; // * 2 * 2;


  int z_button = 0;
  int c_button = 0;

  // byte nunchuck_buf[5] contains bits for z and c buttons
  // it also contains the least significant bits for the accelerometer data
  // so we have to check each bit of byte outbuf[5]
  if ((nunchuck_buf[5] >> 0) & 1)
    z_button = 1;
  if ((nunchuck_buf[5] >> 1) & 1)
    c_button = 1;

  if ((nunchuck_buf[5] >> 2) & 1)
    accel_x_axis += 2;
  if ((nunchuck_buf[5] >> 3) & 1)
    accel_x_axis += 1;

  if ((nunchuck_buf[5] >> 4) & 1)
    accel_y_axis += 2;
  if ((nunchuck_buf[5] >> 5) & 1)
    accel_y_axis += 1;

  if ((nunchuck_buf[5] >> 6) & 1)
    accel_z_axis += 2;
  if ((nunchuck_buf[5] >> 7) & 1)
    accel_z_axis += 1;

  Serial.print(i, DEC);
  Serial.print("\t");

  Serial.print("joy:");
  Serial.print(joy_x_axis, DEC);
  Serial.print(",");
  Serial.print(joy_y_axis, DEC);
  Serial.print("  \t");

  Serial.print("acc:");
  Serial.print(accel_x_axis, DEC);
  Serial.print(",");
  Serial.print(accel_y_axis, DEC);
  Serial.print(",");
  Serial.print(accel_z_axis, DEC);
  Serial.print("\t");

  Serial.print("but:");
  Serial.print(z_button, DEC);
  Serial.print(",");
  Serial.print(c_button, DEC);

  Serial.print("\r\n");  // newline
  i++;
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

//// this sample code provided by www.programmingboss.com
//void setup() {
//  Serial.begin(115200);
//}
//void loop() {
//  Serial.println("TEST TEST TEST TEST TEST");
//}
