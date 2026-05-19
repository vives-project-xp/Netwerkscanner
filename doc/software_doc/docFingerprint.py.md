Hoe container hermaken als je aanpassingen hebt gemaakt.  
`docker compose up -d --build wifi_api `

Hoe ga je in de terminal van de container om individuele programma's te testen.  
` docker exec -it wifi_api /bin/bash`

werkt dit niet, de container is nog niet gestart. gebruik `docker logs wifi_api`

start python programma
`python3 fingerprint.py`

build het volledige project
`docker compose up -d --build`