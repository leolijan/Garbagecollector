Gruppmöten:
17/12:
Vi kom överens om:
- 1x front_bumber
- Vårt program kommer ha reserverade spaces




------

HEADER: 

Informationen om header typ är hittas på de två sist bitarna i HEADER_DATA:
tex:   HEADER_DATA: 00110101 01000010 00110101 01000010 00110101 01000010 00110101 010000__   <---- Läs av dem

HEADER_DATA är en massa som pekar mot antingen: 
- format sträng
- forwarding address
- tillgängligt för utökning
- bitvektor med layout information

---STRUKTUR---
|(HEADER_DATA | 8 bytes)|

** AVLÄSNING AV HEADER **
- HEADER_DATA, läses av genom av hoppa fram 8 bytes, TOLKA header_datan igenom att LÄSA de 2 LSB o sedan nollställa dem
- size, läses av genom att använda str_format på värdet av format sträng.  
--------------



------

ALLOKERING:
  | header | objekt | header | object | ... | header | object |header | objekt | header | object | ... | header | object |
   Page 1                                      Page 2

------
