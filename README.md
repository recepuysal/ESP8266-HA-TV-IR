# ESP8266-HA-TV-IR

Wemos D1 mini (ESP8266) kullanarak televizyonu kızılötesi (IR) sinyalle açıp kapatma ve Home Assistant üzerinden kontrol etme projesi.

## Özellikler

- IR kumanda kodu okuma (NEC protokolü)
- Harici IR LED ile TV güç kontrolü
- WiFi ve MQTT haberleşmesi
- Home Assistant MQTT Discovery ile otomatik cihaz tanıma
- Panoda tek dokunuşla **TV Aç/Kapa** butonu

## Donanım

| Parça | Açıklama |
|-------|----------|
| Wemos D1 mini | ESP8266 tabanlı geliştirme kartı |
| IR LED (LD271 vb.) | 940–950 nm kumanda tipi LED |
| NPN transistör | 2N2222 veya BC547 |
| 100 Ω direnç | LED seri direnci |
| 1 kΩ direnç | Transistör base direnci |

### Bağlantı şeması

```
D1 (GPIO5) ──[1 kΩ]── Transistör Base (B)

5V ──[100 Ω]── IR LED (+) ── IR LED (−) ── Transistör Collector (C)

Transistör Emitter (E) ── GND
```

**Not:** IR LED doğrudan GPIO pinine bağlanmamalıdır. Mutlaka transistör devresi kullanın.

## Kurulum

### 1. Gizli ayar dosyası

```bash
cp include/secrets.example.h include/secrets.h
```

`include/secrets.h` dosyasını düzenleyin:

- WiFi adı ve şifresi
- Home Assistant MQTT broker adresi (IP önerilir)
- MQTT kullanıcı adı ve şifresi

> `secrets.h` dosyası `.gitignore` içindedir; şifreler repoya yüklenmez.

### 2. Derleme ve yükleme

```bash
pio run -t upload
```

### 3. Seri port kontrolü

Serial Monitor: **115200 baud**

Başarılı bağlantı örneği:

```
[WiFi] IP: 192.168.1.x
[MQTT] Bağlandı
[MQTT] Home Assistant discovery yayınlandı
```

## Home Assistant

### Gereksinimler

1. **Mosquitto broker** eklentisi çalışır durumda olmalı
2. **MQTT** entegrasyonu yapılandırılmış olmalı
3. MQTT Discovery açık olmalı (varsayılan)

### Cihazı bulma

**Ayarlar → Cihazlar ve Hizmetler → MQTT** altında **TV IR Kontrol** cihazı otomatik görünür.

### Panoya buton ekleme

1. Ana ekran → **Panoyu düzenle**
2. **Kart ekle → Buton** (veya **Döşeme**)
3. **TV Aç/Kapa** varlığını seçin
4. İkon: `mdi:television`

### Manuel MQTT testi

| Alan | Değer |
|------|-------|
| Topic | `3bfab/tv/power/cmd` |
| Payload | `PRESS` |

## TV IR kodu

Varsayılan kod `include/tv_ir_codes.h` içinde tanımlıdır:

| Alan | Değer |
|------|-------|
| Protokol | NEC |
| Hex | `0x4FB4AB5` |
| Bit | 32 |

Farklı bir TV için kodu IR alıcı modunda okuyup bu dosyayı güncelleyin.

## Lisans

MIT
