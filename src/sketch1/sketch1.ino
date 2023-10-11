  float t = 12.5687;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); delay(10);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(t, 3);
}
