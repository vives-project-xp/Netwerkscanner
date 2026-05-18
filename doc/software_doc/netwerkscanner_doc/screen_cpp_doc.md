# SCREEN
Deze code laat de ESP toe om tekst en vormen op het scherm te tonen
Er zijn 3 lagen:
- laag 1: communicatie met het scherm, LcdCommand() en LcdData() sturen bytes via SPI naar het scherm
- laag 2: Basistekenfuncties: DrawPixel(), DrawLine(), FillScreen() en DrawRectFilled()
- laag 3: Tekst en Iconen, DrawStringFast() en DrawWifiIcon()

De eigen implementatie van het schrijven naar het scherm en niet met een library is omdat het ons niet gelukt is om een library toe te voegen aan de ESP-IDF-programmeeromgeving.
