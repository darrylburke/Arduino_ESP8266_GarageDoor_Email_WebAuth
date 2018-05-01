Arduino_ESP8266_GarageDoor_Email_WebAuth
An ESP8266 project to create a simple wifi enabled gateway to the garage doors with sensors on open/close and email alerts on when activated.

Wifi:

setup your SSID / password

Email

Option 1) Setup a free postmark account, and get your api/auth key and put in appropriate location Option 2) setup a SMTP server and configure server with Base64 encoded userid/pasword for outbound SMTP

default is double 5v relay for the doors and 2 magnetic reed switches to detect if they are open / close

prevents unintentional calls by using a sequence # in the html page which must match on the GET request.
