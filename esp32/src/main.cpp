
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <sys/socket.h>
#include <type.h>
#include <core.h>

TFT_eSPI tft = TFT_eSPI();
WiFiUDP udp;
void setup(void)
{
    tft.init();
    tft.setRotation(4);
    tft.fillScreen(TFT_PINK);
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin("ChinaNet-UKTLH4", "gch123321");
    if (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    udp.begin(8081);
}
void net_info(void)
{
    Serial.print("主机名:");
    Serial.println(WiFi.softAPgetHostname());
    Serial.print("主机IP:");
    Serial.println(WiFi.softAPIP());
    Serial.print("主机IPV6:");
    Serial.println(WiFi.softAPIPv6());
    Serial.print("主机SSID:");
    Serial.println(WiFi.SSID());
    Serial.print("主机广播IP:");
    Serial.println(WiFi.softAPBroadcastIP());
    Serial.print("主机mac地址:");
    Serial.println(WiFi.softAPmacAddress());
    Serial.print("主机连接个数:");
    Serial.println(WiFi.softAPgetStationNum());
    Serial.print("主机网络ID:");
    Serial.println(WiFi.softAPNetworkID());
    Serial.print("主机状态:");
    Serial.println(WiFi.status());
    Serial.print("本机IP:");
    Serial.println(WiFi.localIP());
    Serial.printf("%s\n", WiFi.localIP().toString());
    Serial.printf("%x\n", inet_addr(WiFi.localIP().toString().c_str()));
}

void loop()
{
    net_info();
    int i = 0;
    XLnet net;
    net.ip = inet_addr("192.168.1.15");
    net.port = 8081;
    while (1)
    {
        //char *s ="UDP_send test ESP32";
        //char *s2 ="BROADCAST_send test ESP32";
        //TCP_send(&net, (DATA *)s, strlen(s) + 1);
        //Broadcast_send(s2, strlen(s2) + 1);
        delay(1000);
    }
}