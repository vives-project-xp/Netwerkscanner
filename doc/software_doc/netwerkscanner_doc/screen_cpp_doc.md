# SCREEN
Deze code laat de ESP toe om tekst en vormen op het scherm te tonen
Er zijn 3 lagen:
- laag 1: communicatie met het scherm, LcdCommand() en LcdData() sturen bytes via SPI naar het scherm
- laag 2: Basistekenfuncties: DrawPixel(), DrawLine(), FillScreen() en DrawRectFilled()
- laag 3: Tekst en Iconen, DrawStringFast() en DrawWifiIcon()
