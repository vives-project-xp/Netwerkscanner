| Onderdeel                      | Toegelaten                   | Niet toegelaten       | Korte uitleg                                      |
| ------------------------------ | ---------------------------- | --------------------- | ------------------------------------------------- |
| Netwerkscan algemeen           | ✔                            |                       | Scanner mag enkel gebruikt worden met toestemming |
| IP-ranges                      | ✔ vooraf goedgekeurde ranges | ✖ andere netwerken    | Alleen wat ICT expliciet toestaat                 |
| ICMP / ping                    | ✔                            |                       | Enkel host-bereikbaarheid                         |
| TCP poortscan                  | ✔ beperkte poorten           | ✖ volledige range     | Enkel standaardpoorten                            |
| UDP scan                       |                              | ✖                     | Te belastend / onbetrouwbaar                      |
| Service detectie               | ✔ basis                      | ✖ deep fingerprinting | Enkel “welke service”, niet hoe kwetsbaar         |
| OS-detectie                    |                              | ✖                     | Te invasief                                       |
| Vulnerability scanning         |                              | ✖                     | Geen security-audit tool                          |
| Exploit testen                 |                              | ✖                     | Dit is hacken                                     |
| Firewall omzeilen              |                              | ✖                     | Absolute rode lijn                                |
| Scans tijdens kantooruren      | ✔ licht                      | ✖ zware scans         | Impact beperken                                   |
| Automatisch scannen            |                              | ✖                     | Enkel manueel gestart                             |
| Resultaten opslaan             | ✔ beperkt                    | ✖ onbeperkt           | GDPR-bewust                                       |
| Persoonlijke toestellen (BYOD) |                              | ✖                     | Privacy                                           |
| Productiesystemen              | ✔ lees-only                  | ✖ wijzigingen         | Geen impact veroorzaken                           |
| Logging                        | ✔                            |                       | Traceerbaarheid                                   |
| Resultaten delen               | ✔ intern                     | ✖ extern              | Enkel projectteam                                 |
