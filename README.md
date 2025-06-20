## ibus-ime
A simple Telex input method engine for IBus

### Why build this shjt

I’ve tried several Telex input methods, but many of them felt inconvenient to use, had visual rendering issues, or contained lingering bugs. So I decided to implement my own input method engine.

However, during the process, I realized the real problem wasn’t the input method engine or IBus itself — it was actually caused by my window manager. Because of that, I’ve lost motivation and will likely abandon this project.

This input method currently only supports basic Vietnamese typing using the Telex input method. It does not support features like tone removal or automatic capitalization. While those features would be relatively easy to add, I no longer have the motivation to continue development.

### How to use

1. Make sure you have ibus installed and properly configured.

2. Clone this repository.

3. Run the following commands:

```
ibus exit             # stop the current ibus daemon
make clean install    # build and install the engine
bus-daemon -drx      # restart ibus
ibus-setup
```

Then go to your input method settings and add the new engine.
