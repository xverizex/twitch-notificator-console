# twitch notificator console
Bot irc for twitch.

# Convert mp3 to wav
`ffmpeg -i input.mp3 -ac 1 -f wav output.wav'

Before run fill fields in twitch.conf

# Clone
`git clone --recursive https://github.com/xverizex/twitch-notificator-console`

# Config file
```
[General]
plugins = ["plugins/test/libtest.so"]
lua_scripts = ["dir 0","dir 1"]

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

# How to write lua extensions?
Fill twitch.conf section General with field `lua_scripts`. This field is array of string.
There is simple primer of lua script lie in directory `lua_scripts`. It's name is `test.lua`. It contains one function of `notificator console` and it name `net_send_message`. 
In that primer is are that function.
