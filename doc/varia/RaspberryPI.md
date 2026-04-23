#Netwerk Scanner
---

## Raspberry Pi 4

### USB
| Product / Antenne                    | Type / Opmerking                          | Frequenties             | Extra info / Link                                                                                                                                                                                     |
| ------------------------------------ | ----------------------------------------- | ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **TP‑Link Archer T2U Plus**          | USB Wi‑Fi adapter met externe antennes    | 2.4 / 5 GHz Dual‑Band   | Goede all‑round keuze, dual-band, Linux-compatibel                                                                                                                                                    |
| **TP‑Link Archer T4U AC1300**        | USB Wi‑Fi adapter met externe antennes    | 2.4 / 5 GHz             | Sterke prestaties, externe antennes                                                                                                                                                                   |
| **BrosTrend Linux WiFi Adapter**     | USB Wi‑Fi adapter                         | Dual-Band               | Ontworpen voor Linux, goede antennes                                                                                                                                                                  |
| **Hoco HI34 WiFi6 USB Dual Antenna** | USB Wi‑Fi adapter                         | Wi‑Fi 5/6 (2.4 / 5 GHz) | Wi‑Fi 6, externe antennes                                                                                                                                                                             |
| **TP‑Link TL‑WN722N**                | USB Wi‑Fi adapter met externe antenne     | 2.4 GHz                 | Klassiker voor Raspberry Pi (2.4 GHz)                                                                                                                                                                 |
| **Raspberry Pi sc0480 Antennekit**   | Externe antenne (SMA) voor Compute Module | Dual-band Wi-Fi         | Specifiek voor **CM4 externe antenne** [Kamami.pl](https://kamami.pl/en/accessories-for-raspberry-pi/1188448-sc0480-24g5g-wifi-antenna-for-raspberry-pi-compute-module-4.html?utm_source=chatgpt.com) + antenne connector dongle to usb|
| **Compute Module CM4 Antenna Kit**   | Antennekit met U.FL-SMA kabel             | Dual-band Wi-Fi         | Kit voor **CM4**, inclusief kabel [asdstore.de](https://www.asdstore.de/cm4-cm5-antenna-kit_1?utm_source=chatgpt.com)                                                                                 |
| **Compatible Antenna for CM4/5**     | Basis antenne voor CM4/5                  | Dual-band Wi-Fi         | Budget-antenne voor Compute Module + antenne connector dongle to usb                                                                                                                                                                  |
| **Waveshare Compatibel Antenne**     | Externe antenne voor CM4/5                | Dual-band Wi-Fi         | Horn-antenne met SMA aansluiting [RobotShop USA](https://www.robotshop.com/products/waveshare-compatible-antenna-for-raspberry-pi-compute-module-4-24g-5g-wifi?utm_source=chatgpt.com) + antenne connector dongle to usb                |
| **Waveshare CM4/5 Antenna Kit**      | Complete antennekit                       | Dual-band Wi-Fi         | Kit met accessoires, CM4/5 compatible 

### Niet USB
| Antenne                                                   | Frequentie |  Connector type | Opmerkingen                                                                             |
| --------------------------------------------------------- | ---------: | :-------------: | --------------------------------------------------------------------------------------- |
| **[Otronic 2.4GHz WiFi antenne met SMA naar uFL/IPEX]()** |    2.4 GHz | SMA → u.FL/IPEX | Klein externe antenne met adapter → ideaal voor u.FL antenne‑connector ESP‑boards.      |
| **[SMA 2.4GHz 2dB antenne]()**                            |    2.4 GHz |       SMA       | Basis omnidirectionele antenne – vereist adapter naar u.FL/IPEX om te koppelen aan ESP. |
| **[Equipment 2.4GHz antenne]()**                          |    2.4 GHz |   (SMA/RP‑SMA)  | Externe antenne – converter kabel nodig voor aansluiting op ESP boards met u.FL.        |
| **[Pinwheel 2.4GHz SMA antenne]()**                       |    2.4 GHz |       SMA       | Robuuste antenne met SMA – combineer met u.FL‑SMA pigtail.                              |
| **[JARFT 2.4GHz WiFi Omni antenne]()**                    |    2.4 GHz |      RP‑SMA     | All‑round antenne – goede keuze voor IoT projecten, adapter vereist.                    |
| **[2.4GHz Duck Antenna RP‑SMA]()**                        |    2.4 GHz |      RP‑SMA     | Whip‑antenne met rubber dop; past via adapter op boards met u.FL/IPEX.                  |
| **[2.4GHz Whip Rubber Antenna]()**                        |    2.4 GHz |       SMA       | Eenvoudige whip antenne – werkt met adapters.                                           |
| **[Alfa Network 2.4GHz 15dBi omni antenne]()**            |    2.4 GHz |      RP‑SMA     | Hoog‑gain omni antenne – geschikt voor lange‑afstand Wi‑Fi (met adapter).               |
                                                                                                                                                                |
| Module                     | CPU / Core                | Wi-Fi        | Bluetooth     | Externe antenne | Voordelen | Nadelen | Aanbevolen gebruik |
|----------------------------|--------------------------|-------------|--------------|-----------------|-----------|----------|--------------------|
| ESP32-WROOM-32U            | Dual-core 240 MHz        | 2.4 GHz     | BT + BLE     | Ja (u.FL)      | Zeer stabiel, veel support, goed voor promiscuous mode | Iets hoger verbruik | Allround netwerk scanner |
| ESP32-S3-WROOM-1U          | Dual-core 240 MHz LX7    | 2.4 GHz     | BLE 5        | Ja (u.FL)      | Native USB, moderner, betere power management | Iets duurder | USB logging + data analyse |
| ESP32-C3-WROOM-02U         | Single-core 160 MHz RISC-V | 2.4 GHz  | BLE 5        | Ja (u.FL)      | Energiezuinig, compact, goedkoop | Minder rekenkracht | Batterij-geoptimaliseerde scanner |
| ESP32-C6-WROOM-1U          | Single-core 160 MHz RISC-V | 2.4 GHz (Wi-Fi 6) | BLE 5 + 802.15.4 | Ja (u.FL) | Nieuwste generatie, toekomstgericht | Nog minder mature support | Experimenteel / Wi-Fi 6 scanning |
| ESP8266 (ESP-07)           | Single-core 80–160 MHz   | 2.4 GHz     | Geen         | Ja (u.FL)      | Zeer goedkoop | Beperkte sniffing support, ouder | Low-budget project |

| Optie                         | Type                 | Nominale spanning | Capaciteit (typisch) | Extra elektronica nodig | Voordelen | Nadelen | Aanbevolen gebruik |
|--------------------------------|----------------------|-------------------|----------------------|--------------------------|------------|----------|--------------------|
| USB Powerbank (5V output)     | Li-ion intern        | 5V uitgang        | 5.000–20.000 mAh     | Nee                      | Plug & play, ingebouwde bescherming, stabiele 5V | Minder controle over power management | Snelle prototype / eenvoudige voeding |
| 1x 18650 Li-ion cel           | Li-ion               | 3.7V (4.2V max)   | 2.000–3.500 mAh      | Ja (TP4056 + boost)      | Compact, goedkoop, vervangbaar | Extra laad- en boostcircuit nodig | Custom compacte scanner |
| 2x 18650 in serie (2S)        | Li-ion               | 7.4V (8.4V max)   | 2.000–3.500 mAh      | Ja (BMS + buck converter)| Hogere efficiëntie bij buck naar 5V | Complexer, BMS vereist | Langere runtime systemen |
| 1S LiPo batterij              | LiPo                 | 3.7V (4.2V max)   | 500–5.000 mAh        | Ja (laadmodule + evt. boost) | Licht, plat, ideaal voor handheld | Mechanisch gevoeliger | Compacte handheld scanner |
| 2S LiPo batterij              | LiPo                 | 7.4V              | 1.000–5.000 mAh      | Ja (BMS + buck)          | Efficiënte spanningsregeling mogelijk | Hogere complexiteit | Geavanceerde builds |
| 18650 DIY Powerbank module    | Li-ion (18650)       | 5V uitgang        | Afhankelijk van cel  | Nee (meestal ingebouwd)  | Zelf cellen kiezen, 5V output | Kwaliteit varieert | Semi-custom oplossing |

## Displays:

| 5 inch touchscreen 52 euro |https://www.hobbyelectronica.nl/product/5-0-ips-capacitive-touchscreen-esp32-8048s050c-i/ |
| 4.3 inch touchscreen 39 euro | https://www.hobbyelectronica.nl/product/4-3-ips-capacitive-touchscreen-esp32-8048s043c-i |
| 3.5 inch touch 24 euro (specifiek voor ESP) | https://www.hobbyelectronica.nl/product/esp32-3-5-tft-480-320-touch/ |