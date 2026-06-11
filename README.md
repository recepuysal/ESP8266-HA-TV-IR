# esp8266-ha-tv-ir

Wemos D1 mini (ESP8266) ile TV acma/kapama — IR ogrenme, MQTT ve Home Assistant entegrasyonu.

## Ozellikler

- IR kumanda kodu okuma (NEC)
- Harici IR LED ile TV kontrolu (transistor devresi)
- WiFi + MQTT
- Home Assistant MQTT discovery (`TV Ac/Kapa` butonu)

## Donanim

| Parca | Not |
|-------|-----|
| Wemos D1 mini | ESP8266 |
| IR LED (LD271 vb.) | 940–950 nm |
| NPN transistor | 2N2222 / BC547 |
| 100 Ω | LED seri direnc |
| 1 kΩ | Base direnc |

```
D1 (GPIO5) --[1k]-- Base
5V --[100R]-- IR LED(+) -- IR LED(-) -- Collector
Emitter -- GND
```

## Kurulum

1. `include/secrets.example.h` dosyasini `include/secrets.h` olarak kopyalayin
2. WiFi ve MQTT bilgilerini doldurun
3. PlatformIO ile derleyip yukleyin:

```bash
pio run -t upload
```

## Home Assistant

1. Mosquitto broker eklentisini baslatin
2. MQTT entegrasyonunu yapilandirin
3. Cihaz otomatik gorunur: **TV IR Kontrol → TV Ac/Kapa**
4. Panoya **Button** veya **Tile** karti ekleyin

MQTT test:

- Topic: `3bfab/tv/power/cmd`
- Payload: `PRESS`

## Lisans

MIT
