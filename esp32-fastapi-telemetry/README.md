# ESP32 & FastAPI Industrial Telemetry (Digital Twin Approach)

<p align="center">
  <img src="/images/test1.jpeg" width="250">
  <img src="/images/test2.jpeg" width="250">
  <img src="/images/test3.jpeg" width="250">
  <img src="/images/test4.jpeg" width="250">
  <img src="/images/test5.jpeg" width="250">
</p>

Bu proje, 3 farklı endüstriyel makinenin sıcaklık verilerini ESP32 üzerinden toplayan/simüle eden ve bu verileri **FastAPI** tabanlı bir sunucuya ileten uçtan uca bir IoT telemetri sistemidir. Projede bellek (RAM) optimizasyonuna odaklanılmış ve standart JSON yerine daha düşük veri yüküne sahip "URL-encoded" iletişim protokolü tercih edilmiştir.

Veritabanı tarafında "Digital Twin" (Dijital İkiz) mantığı kurularak, veritabanı loglarla şişirilmek yerine sadece 3 makinenin anlık durumları (sıcaklık ve fan seviyesi) `UPDATE` komutlarıyla takip edilmiştir.

## 🚀 Öne Çıkan Özellikler (Engineering Highlights)

* **Low-Overhead HTTP İletişimi:** ESP32'nin RAM kullanımını minimumda tutmak için veriler JSON yerine `application/x-www-form-urlencoded` (ör: `temp=45&machine_id=1`) formatında gönderilmiştir. Sunucu cevabı da parse etme yükünü ortadan kaldırmak için `PlainText` olarak dönmektedir.
* **Tek Bağlantı (Stateless) Mimarisi:** ESP32, POST isteğini atıp veriyi sunucuya yazarken, aynı isteğin cevabında (Response) hesaplanan fan hızını geri alır. İkinci bir GET isteğine gerek kalmaz.
* **Digital Twin Veritabanı:** SQLite üzerinde oluşturulan tek bir tablo (`machines`), 3 makinenin anlık statüsünü tutar. Sistem başlangıcında veritabanı otomatik olarak tohumlanır (seed).
* **Özel LCD HMI (Human-Machine Interface):** 16x2 I2C LCD ekran üzerinde özel karakterler (Custom Characters) oluşturularak, sunucudan dönen 1, 2 ve 3 seviyeli fan hızları dinamik bir bar grafiğine (`[■][■][ ]`) dönüştürülmüştür.

## ⚙️ Sistem Mantığı

Sistem her 3 saniyede bir sırayla 3 makinenin sıcaklık değerini sunucuya bildirir:
1. **Makine 1 (Fiziksel):** ADC pini (Pin 33) üzerinden okunan potansiyometre verisi (20°C - 96°C arasına haritalanır).
2. **Makine 2 (Sanal):** 45°C - 70°C arası rastgele üretilen simüle sıcaklık.
3. **Makine 3 (Sanal):** 70°C - 96°C arası rastgele üretilen simüle sıcaklık.

**Sunucu Tarafı Fan Kuralları (FastAPI):**
* `T < 45°C` ➔ Fan Seviyesi: **1**
* `T < 70°C` ➔ Fan Seviyesi: **2**
* `T ≥ 70°C` ➔ Fan Seviyesi: **3**

## 📂 Dosya Yapısı

```text
esp32-fastapi-telemetry/
│
├── esp32_iot_client.ino      # ESP32 C++ kaynak kodu (Sensör okuma ve HTTP POST)
├── fastapi_iot_server.py     # Python FastAPI backend ve SQLite veritabanı yönetimi
├── dashboard.html            # Anlık makine durumlarını gösteren Jinja2 arayüzü
└── README.md                 # Proje dokümantasyonu
