#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

int trigger_pin = 5;
int echo_pin = 18;
float deep_max = 4;

const char *ssid = "Chcken";
const char *password = "0377752925";

const char *mqttServer = "6013f541d4ae432aa706c26260fcf199.s1.eu.hivemq.cloud";
const char *mqttUser = "tranngocbao";
const char *mqttPassword = "Tranbao12623";

const char *root_ca =
	"-----BEGIN CERTIFICATE-----\n"
	"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\n"
	"-----END CERTIFICATE-----\n";

float sum = 0;
int cnt = 0;

WiFiClientSecure espClient;
PubSubClient client(espClient);

void reconnectWiFi()
{
	Serial.println("");

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("Connecting to WiFi...");
		delay(1000);
	}

	Serial.println("");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}

void reconnectMQTTBroker()
{
	Serial.println("");

	while (!client.connected())
	{
		Serial.println("Connecting to MQTT...");
		// Attempt to connect
		if (client.connect("ESP32Client", mqttUser, mqttPassword))
		{
			Serial.println("Connected");
		}
		else
		{
			Serial.print("Failed with state ");
			Serial.println(client.state());
		}
		delay(1000);
	}
}

void setup()
{
	Serial.begin(115200);
	pinMode(trigger_pin, OUTPUT);
	pinMode(echo_pin, INPUT);

	// Set up WiFi
	WiFi.begin(ssid, password);

	// Connect to MQTT Broker
	espClient.setCACert(root_ca);
	espClient.setCertificate(root_ca);
	client.setServer(mqttServer, 8883);
}

void loop()
{
	// Create pulse 10 micro seconds
	digitalWrite(trigger_pin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigger_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigger_pin, LOW);

	// t = time echo high
	long duration = pulseIn(echo_pin, HIGH);

	// s = v * t (v: sound speed)
	float distance = ((float)duration / 2) / 2909;
	float deep = deep_max - distance;
	Serial.println(deep);

	if (WiFi.status() != WL_CONNECTED)
	{
		reconnectWiFi();
	}

	if (!client.connected())
	{
		reconnectMQTTBroker();
	}
	client.loop();

	sum += deep;
	++cnt;
	if (cnt % 6 == 5)
	{
		// Publish data to MQTT Broker
		String msg = (String)(sum / 5);
		Serial.println(msg);
		char buffer[msg.length() + 1];
		msg.toCharArray(buffer, msg.length() + 1);
		client.publish("device/6590d4eddcaf2c17b80c210e", buffer); // Ha Noi
		client.publish("device/659968e7be1746b7a83c2bf1", buffer); // Nam Dinh
		sum = 0;
		++cnt;
	}

	// Sync
	unsigned long t = cnt * 10000;
	delay(t - millis());
}