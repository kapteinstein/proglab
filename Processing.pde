class PLabOrientationEvent {
  /**
   * alpha - magnetic direction (in degrees)
   */
  float alpha;
  /**
   * beta - tilt front-to-back, front is positive (in degrees)
   */
  float beta;
  /**
   * gamma - tilt left-to-right, right is positive(in degrees)
   */
  float gamma;
}
/**
 * PLabAccelerationEvent is the event object received at regular intervals, telling what acceleration is
 */
class PLabAccelerationEvent {
  /**
   * x - acceleration in x direction
   */
  float x;
  /**
   * y - acceleration in y direction
   */
  float y;
  /**
   * z - acceleration in z direction
   */
  float z;
  /**
   * timestamp - when the acceleration was meassured
   */
  float timestamp;
}
/**
 * PLabBridge is the interface we have for communicating with the plab app.
 */
interface PLabBridge {
  /**
   * gets the width we have available to draw on
   */
  public int getWidth ();
  /**
   * gets the height we have available to draw on
   */
  public int getHeight ();
  /**
   * sends a string to a connected bluetooth device. Can not send more than 20 characters.
   */
  public void send (String string);
  /**
   * register callback function that will get the data sent from the connected device
   */
  public void subscribeMessages (PLabRecv sub);
  /**
   * Disconnects the device and returns to main menu
   */
  public void disconnect();
  /**
   * hides the back button
   */
  public void hideBackButton();
  /**
   * display the back button
   */
  public void showBackButton();
  
  /**
   * make the device vibrate for given amount of time. On iOS time will be ignored.
   */
  public void vibrate(int milliseconds);
  /**
   * Listen for device orientation changes
   */
  public void addDeviceOrientationListener(PLabOrientationListener listener);
  /**
   * remove device orientation changes listener
   */
  public void removeDeviceOrientationListener(PLabOrientationListener listener);
  /**
   * Listen for device acceleration changes
   */
  public void addDeviceAccelerationListener(PLabAccelerationListener listener);
  /**
   * remove device acceleration changes listener
   */
  public void removeDeviceAccelerationListener(PLabAccelerationListener listener);
  /**
   * Set milliseconds between each acceleration update
   */
  public void setDeviceAccelerationUpdateInterval(int milliseconds);
}

/**
 * A simple interface that defines the callback read function
 */
interface PLabRecv {
  /**
   * The callback function. Will be called when the connected device sends something to this program.
   */
  public void receive(String message);
}

/**
 * PLabOrientationListener is the interface describing an orientation change listener
 */
interface PLabOrientationListener {
  public void deviceOrientation(PLabOrientationEvent event);
}

/**
 * PLabAccelerationListener is the interface describing an acceleration event listener
 */
interface PLabAccelerationListener {
  public void deviceAcceleration(PLabAccelerationEvent event);
}

private PLabBridge pBridge;

/**
 * Bruker denne som setup
 *
 * Uses this as setup
 */
void bindPLabBridge (PLabBridge bridge) {
  // Bind brua til den faktiske instansen
  // Bind the bridge to the instance
  pBridge = bridge;
  
  // Vi trenger noe som hoerer etter endringer i verdi
  // We need something that listens to a change in the value
  // Vi bruker en anonym indre klasse for dette
  // We use an anonymous inner class for this
  bridge.subscribeMessages (new PLabRecv() {
    public void receive (String string) {
      i = int (string);
    }
  });
  
  // Sett stoerrelsen basert paa vindustoerrelsen 
  // Set the size based on window size
  size (bridge.getWidth (), bridge.getHeight ());
}

// For aa faa relativ posisjon definerer vi verdens stoerrelse
// To get relative positions, we define world size
int worldWidth = 100;
int worldHeight = 100;

int barX=25;
int barY=0;
int barSize=50;

// Vi definerer egenskapene til knappen i verdenssystemet
// We define properties of our button in the world system
int buttonX = 25;                         // worldWidth / 4
int buttonY = 25;                         // worldHeight / 4
int buttonSize = 50;                      // worldWidth / 2 or worldHeight / 2
boolean buttonPressed = false;

// Oversettelse fra verdenskoordinater til skjermkoordinater
// Conversion from world coordinates to screen coordinates
int bToScreenX (int x) {
  // Eksplisitt casting paa grunn av appen har javascript oppfoersel
  // Explicit cast done because of app javascript behaviour
  return (int) (width * x / worldWidth);
}
int bToScreenY (int y) {
  // Eksplisitt casting paa grunn av appen har javascript oppfoersel
  // Explicit cast done because of app javascript behaviour
  return (int) (height * y / worldHeight);
}

// Oversettelse fra skjermkoordinater til verdenskoordinater
// Conversion from screen coordinates to world coordinates
int bToWorldX (int sx) {
  // Eksplisitt casting paa grunn av appen har javascript oppfoersel
  // Explicit cast done because of app javascript behaviour
  return (int) (worldWidth * sx / width);
}
int bToWorldY (int sy) {
  // Eksplisitt casting paa grunn av appen har javascript oppfoersel
  // Explicit cast done because of app javascript behaviour
  return (int) (worldHeight * sy / height);
}
int i;

// setup () brukes kun til testing
// setup () is only used for testing purposes
void setup () {
  // Denne "Sett stoerrelsen" vil bli overkjoert naar brua blir bundet
  // This size set will be overridden when bridge is bound
  i=0;
  size (240, 360);
}

// Gjoer tegningen. Hovedloopen til eksempelet
// Does the drawing. Main loop of example
void drawRect(int x,int y,int w,int h, int r)
{
  stroke(0,0,0);
  fill(r,0,0);
  rect(x,y,w,h);
}
void draw () {
  // Kjoer update () for aa sjekke om knappen er trykt ned
  // Run update () to check if button is pressed
  bUpdate ();
  // graa bakgrunn
  // grey background
  background (#7F7F7F);
  // svart omriss til knappen
  // black stroke around the button
  stroke (#000000);
  // Fyll farge settes basert paa om knappen er trykt ned
  // Set fill color based on pressed status
  if (buttonPressed) {
    // Trykket -> svart knapp
    // Pressed -> black button
    fill (#000000);
  } else {
    // Uroert -> hvit knapp
    // Unpressed -> white button
    fill (#FFFFFF);
  }
  // Tegn knappen
  // Draw the button
  // Husk aa oversette til verdenskoordinater
  // Remember to translate coordinates to world coordinates
  rect (bToScreenX (buttonX), bToScreenY (buttonY), bToScreenX (buttonSize), bToScreenY (buttonSize));
  //i=pBridge.recv();
  drawRect(25,10,300,10, 0);
  drawRect(25,10,i,10,128);
}

void bUpdate () {
  boolean oldPressed = buttonPressed;
  // Standardverdien til knappen er alltid av
  // Default press status to false
  buttonPressed = false;
  // Hvis musa er trykt
  // If mouse is pressed
  if (mousePressed) {
    // Oversett musekoordinater (skjermkoordinater) til verdenskoordinater
    // Translate mouse coordinates (screen coordinates) to world coordinates
    int mX = bToWorldX (mouseX);
    int mY = bToWorldY (mouseY);
    // Sjekk om musa er innenfor i x-retning
    // Check if mouse is within bounds in x direction
    buttonPressed = mX >= buttonX && mX <= (buttonX + buttonSize);
    // Sjekk om musa er innenfor i y-retning
    // Check if mouse is within bounds in y direction
    buttonPressed = buttonPressed && mY >= buttonY && mY <= (buttonY + buttonSize); 
  }
  
  // Hvis statusen til knappen er endret, si ifra til brua. Dette gir oss minimalt med kommunikasjon
  // If the new status is changed, notify the bridge. This minimizes communication
  if (oldPressed != buttonPressed) {
    btnChanged ();
  }
}

void btnChanged () {
  // Denne kalles naar statusen til knappen har endra seg. Si ifra til brua
  // This is called when status of button has changed. Notify bridge
  // SJEKK ALLTID OM DET ER NULL FOR AA UNNGAA FEIL
  // ALLWAYS CHECK FOR NULL TO AVOID ERRORS
  
  if (pBridge != null) {
    String send;
    if (buttonPressed) {
      send = "ON";
    } else {
      send = "OFF";
    }
    pBridge.send (send);
  }
  
}