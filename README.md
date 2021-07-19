# Game Engine Template: Low Rez Game Jam

## What is this?
I like to write game engines designed for some very specific tasks. I find that the control I get from being able to mold the game engine to a specific set of constraints yields better games.

This project is an attempt to do something I previously thought impossible.

Given the timeline for most game jams, and the seeming consensus that it's just not possible to create your own game engine specifically for a game jam, I asked "could it be done?"

In a spiritual sense, most game jams are about rapid prototying, not getting too married to a specific game idea, and taking some time to explore the space of a game (usually only at the surface level unfortunately). They are intentionally low-fidelity because being high fidelity gets in the way of exploring the idea. You do that part later on, once you've locked onto something worth doing at high detail.

The ideal game engine for a game jam first and foremost ports the game to web. As much as I acknowledge that the web is a steaming turd sandwich on top of mind-blowingly fast computer hardware, even I cannot deny the importance of not forcing people to download your game in order to try it out.

The web is automatically cross-platform, and this carries many other advantages as well.

That said, I also prefer not to waste my efforts. That means if I happen to make a game for a game jam, and I want to pursue the idea further and with greater depth, I don't want to redo the majority of my work.

This is why I prefer not to use other engines. They lock me into their style of abstraction, and it becomes more work to divorce the game from it later on. Everything is easy at the beginning, but you fight and fight later on when you're finishing the product.

That's a really long-winded way of saying this game engine (and others like it) has two primary goals.

1. Make the game available on the web easily and without much fuss.
2. Open the door to port the game natively to other platforms later on.

## Should you be doing this?
Some may break with me here, so I feel the need to at least address it. So far as I understand it, you can bring any "engine code" to any game jam.

The point of a game jam is to explore a specific game idea within the constraints of the jam, not to spend all of your time deep in the lower levels of software development.

I figure if other people can use other game engines in the game jam, then I can surely create a game engine for the jam, share it with others, and set myself up to work in a way that I prefer. I can do this before the jam begins, and I can use that work later on as the basis for other game engines I use in other game jams.

The only thing I would consider "unfair" is starting to make the game's rules and art before the jam, although that has often been to done some extent by many people as well.

If an artist has a base template she uses to make sprite characters, is that "cheating?" I don't think so. It's more about originality and the new thing being created. If you are standing on top of your previous efforts to do it, that's fine. We all do this, to some extent, in everything we pursue.

## How can I use it?
This game engine probably isn't like anything you're used to. It isn't a framework or a visual editing tool (although it could be modified to provide such tools). It isn't object oriented. There are no classes, no inheritance, nothing like that.

It is C++ based, although it doesn't use all of the features of C++. It's more like straight C. Many refer to this style as "data oriented". Go do a YouTube search and you'll find some useful videos.

For the moment, this game engine only builds from a Mac (and probably linux but I haven't tested that). To get it to build on a PC, you would need to create a build.bat file with a list of commands to do that build on Windows.

To build, open terminal and go into the build directory. Then run the build.sh script. This script will output the necessary files into the build directory.

To play any games created with this engine, you just need to copy the files in the build/web_assembly directory to some web server somewhere. You can also run your own web server locally to test it out.

The intent is for you to use this game engine as a base template for a specific game. You would have it handy at the start of the game jam, then modify it during the jam to make your own game. That's the idea.

## What are the features?
You can think of this as mostly a web platform layer for a sprite-based 2D game that uses Open GL ES 2.0 to draw textured pixel art rectangles to a screen. 

It doesn't have multiple drawing layers, although you could add that yourself. It's lacking physics and collision, but again you could bring in other code if you want.

It does load PNG files, and it has a way of setting up those textures in Open GL so they can be drawn to the screen.

The default texture size is 8px by 8px, and the default screen size matches the game jam. The game's fragment shader uses nearest neighbor texture sampling to create that pixelated look, which works so long as you always use integer size multipliers for the textures.

If you want bigger or smaller sprites, you'll have to add them yourself. I'm still not sure what size of sprites I'll use in the jam itself, since it hasn't started yet and I don't know what kind of game I'll be making. This engine at least gives you the basis to make sprites of any size you like.

I will be adding more as the jam approaches. This week, I am working on music and sound effects, which will likely use SDL 2.0 since it's web, and I don't want to go super deep on the details. Native platform layers will have better audio mixing.

## How can I port this to other platforms?
Many of the ideas in this game engine are more or less taken from [Casey Muratori's Handmade Hero](https://handmadehero.org). If you want to learn how to do cross-platform game engine development, I would recommend watching his series and following along with it.

With this engine, I am basically doing the same thing he does, except I make web assembly the primary deployment target. I use graphics libraries to do the rendering from the outset as well (instead of exploring rendering with the CPU first).

If you wanted to port this to other platforms, you would need to create a platform layer for the target platform, then do the necessary setup in those layers. But before you do that, I would definitely recommend watching Casey's videos because it is best done with a proper understanding of the approach that will make porting much less painful. He has a very specific design philosophy that is really important to grasp first.

## I have a question about this game engine. How can I contact you?
Just [send me and email](mailto:tedbendixson@me.com). If enough people find this work valuable, I'll create a discord around my various repos, and we can chat amongst ourselves.

## Thanks! I find this incredibly valuable. How can I support you?
Well shucks, I really appreciate the sentiment. You can sponsor me on github. You could also download some of my games and donate. I'm on tons of different platforms, so just choose whatever fits your budget and the amount of value that you believe I have contributed.

## Where to find me
🎮 [ITCH](https://tedbendixson.itch.io/cove-kid)
🐦 [TWITTER](https://twitter.com/TBendixson)
📱 [ITUNES](https://apps.apple.com/us/developer/send-it-apps-llc/id1139954390)
📄 [MEDIUM](https://theobendixson.medium.com)
🦉 [WYZANT](https://www.wyzant.com/tutor/profile)
💵 [FIVERR](https://www.fiverr.com/tedbendixson)
