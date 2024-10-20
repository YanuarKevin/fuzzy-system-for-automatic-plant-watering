# fuzzy-system-for-automatic-plant-watering
Projoect penyiraman tanaman otomatis berbasis sistem fuzzy &amp; skc

Bahan-bahan & pin out:
1. ESP32 (penggunaan pin ADC banyak)
2. Rain sensor (AO ADC1_4 GPIO_32)
3. LDR module (AO ADC1_6 GPIO_34)
4. Kelembaban tanah (AO ADC1_7 GPIO_35)
5. DHT11 (DO GPIO_33)
6. Relay (DO GPIO_27)
7. LCD 16x2 I2C
8. Pompa DC / solenoid

Tampilan LCD:
1. saat booting nama kelompok
2. state 1 menampilkan pembacaan kelembaban tanah, intensitas cahaya, suhu udara & kelembaban udara
3. state 2 menampilkan pembacaan rain sensor & estimasi waktu dan durasi penyiraman 
