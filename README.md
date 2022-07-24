# twitch notificator console
Bot irc for twitch.

Before run fill fields in twitch.conf

# Clone
`git clone --recursive https://github.com/xverizex/twitch-notificator-console`

# Config file
```
[General]
plugins = ["plugins/test/libtest.so"]

[Twitch]
tmi = "your tmi with oauth:..."
channel = ["channel 0","channel 1"]
nick = "nickname"
alert = "sound alert mono 16 bit wav file"

[History]
enabled = false
file = "filepath"
```
# How to write plugins?
See directory plugins/test for example. There is very simple primer to understand how to plugin works.
