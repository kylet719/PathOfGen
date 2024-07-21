# Team-08: Path of Gen

Milestone 1:

- Implemented overworld
- Simple avoid the virus game

Milestone 2:

- Upgrade game to Escape the Virus game (Pac-man)
- Created Minigame: Flow
- Added tutorial screens
- Added animated win/lose screens
- Added music and sound effects

Milestone 3:

- Created Minigame: Whack-a-Virus (whack-a-mole)
- Created Minigame: Acid Avoider
- Created Minigame: Clear the Phlegm
- Overhauled overworld to an organ system
- Added a title screen
- Added saving

Creative Component 1 - #1 Simple rendering effects:
In [Whack-a-Virus], after a certain amount of time the viruses start shaking which is done with a fragment shader that changes a uniform over time.

Creative Component 2 - #8 Basic physics:
In [Clear the Phlegm], we have elastic collisions with the (Gen) ball and the (phlegm) bricks.

Creative Component 3 - #9 Complex prescribed motion:
In [Clear the Phlegm], the motion of the oxygen molecules are determined by a quadratic Bézier curve function.

Creative Component 4 - #19 Reloadability:
The player can manually save the game at any point with "M" key. The game also auto saves after finishing a game. To reload the game in the title screen the player can select "Load Save" and the player can resume where they left off.
Things that are being saved:
- Current organ
- Completed organ
- Player position
- Tutorial completion
- Inventory (for future)

Milestone 4:

General:
- Screen Transitions between organ systems and games
- Movement calculation adjusted to take into account elapsed time

Organs: 
- Inventory system
- Text for organ name

MG1: Escape the Virus
- Randomized map creation using path finding
- Address clipping bug

MG3: Acid Avoider
- Gravity
- Double jump feature
- Finish line
- Glucose collectable

MG4: Whack-a-Virus
- Iron (Fe) collectable that moves on cubic bezier curve

MG5: Clear the Phlegm
- Created new maps that are randomly chosen
- Randomized Oxygen containing bricks
- 2 new Power Ups (3x Multiplier, Long Paddle)
- Particle System on brick destruction

Brain:
- After collecting the correct number of items the player can unlock the Brain
- Final path cutscene can be chosen, either "Kill Human" and "Enlighten Human"

Credits:
- End credits roll after Final Path cutscene


Creative Component - #14 Path Finding:
In [Escape the Virus], the wall grid is generated randomly using a DFS traversal. 

Creative Component - #5 Particle System:
In [Clear the Phlegm], brick destruction is indicated with particles that appear in there place.


## Attributions
### Music
All copyright free music
- Title screen: Day After Day https://uppbeat.io/music/genre/8-bit 
- Minigame 1: Getting It Done https://uppbeat.io/music/genre/8-bit
- Minigame 2: Sky City https://uppbeat.io/music/genre/8-bit
- Minigame 3: POwerup! - Jeremy Blake https://www.youtube.com/watch?v=l7SwiFWOQqM
- Minigame 4: Up In My Jam - Kubbi https://www.youtube.com/watch?v=cLX0cyh6_Ro
- Minigame 5: Operatic 3 - Vibe Mountain https://www.youtube.com/watch?v=2oifEEBB5UQ
- Intestine: Night Shade: AdhesiveWombat https://www.youtube.com/watch?v=mRN_T6JkH-c
- Kidney: Floating Also: William Rosati https://www.youtube.com/watch?v=HHYOBwzT4u4
- Stomach: 8 Bit Think!: HeatleyBros https://www.youtube.com/watch?v=hdZLNZBZFlY
- Liver: Retro Bowl Theme Song: HeatleyBros https://www.youtube.com/watch?v=TdXFHKjJopI
- Lung: Wonderful Life!: HeatleyBros https://www.youtube.com/watch?v=yif-nXPKuis
- Brain: Frogs: https://uppbeat.io/music/genre/8-bit 
- Kill Ending: The Fastest Snail: Pix https://www.youtube.com/watch?v=NNRiQQQiTOo
- Help Ending: Corkscrew https://uppbeat.io/music/genre/8-bit 
- Credits: Pixel Dream: Suno (AI) https://suno.com/song/078b2dd1-8703-47d2-a877-048afbb246bc
- Sound effects: https://uppbeat.io/sfx/tag/8-bit

## Art
Most art was drawn over using Aseprite for more detail
- Organs: https://www.123rf.com/photo_115025354_pixel-art-human-organs-vector-icon-set.html (Stock image, free trial)
- Transport nodes: https://www.shutterstock.com/image-illustration/red-warp-portal-pixel-art-illustration-2201294671 (Stock image)
- Man dying street background: https://surrogateself.tumblr.com/post/130627012954 (Creative commons)
- Man dying/de-zombifying: https://pngtree.com/freepng/success-pixel-villain-illustration_4634769.html (Free monthly download)
- Graveyard background: https://in.pinterest.com/pin/art-is-subjective--220606081737488319/ (No owner found)
- Acid: https://pimen.itch.io/acid-spell-effect
- The rest: 
    * ChatGPT generated art
    * Original freehands

## Fonts
- QuinqueFive: https://www.dafont.com/quinquefive.font (Free)
- Kenny Pixel: https://www.kenney.nl/assets/kenney-fonts (Free)

## Programs
- Blender
- Aseprite
- Audacity