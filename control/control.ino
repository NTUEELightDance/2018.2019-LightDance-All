long int cnt = 0;

void setup() {
  for(int i=0; i<=13; i++)
    pinMode(i, OUTPUT);

  Serial.begin(9600);
} 

void loop() {
  int tl = 500;
  cnt++;

  int d = cnt % 14;
  for(int i=0; i<=13; i++)
    digitalWrite(i, d==i ? HIGH : LOW);
  Serial.println(d);
  delay(tl);
}
