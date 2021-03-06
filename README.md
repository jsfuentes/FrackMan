# MANfrack
In FrackMan, the player starts out a new game with three lives and continues to play until
all of his/her lives have been exhausted. There are multiple levels in FrackMan,
beginning with level 0, and each level has its own unique oil field configuration. During
each level, the FrackMan (controlled by the player) must dig and find all of the oil
hidden within the oil field in order to move on to the next level.
Upon starting each level, the player’s FrackMan avatar (a miner) is placed at the topmiddle
of the oil field just above the mineshaft. Every time the player starts (or re-starts)
a level, the FrackMan is restored to 100% health, given fresh water for their squirt gun
(enough for 5 squirts), and given a single sonar charge that they can use to search for
nearby items within the dirt. Each oil field has a number of Boulders, Gold Nuggets, and
Barrels of oil randomly distributed throughout the field. Only the Boulders will be visible
initially – the other items must be discovered by the FrackMan before they become
visible. The player must use the arrow keys to move the FrackMan left, right, up and
down through the oil field; the FrackMan digs through any dirt in the direction of motion.
The FrackMan may move/dig anywhere in the oil field except for locations occupied by
Boulders.
The FrackMan must dig through the oil field in search of every Barrel of oil and pick
each one up by moving to its location. Once the FrackMan has picked up all of the
Barrels of oil in the current oil field, the level ends and the player advances to the next
level.
Unfortunately, the FrackMan is not alone. A band of Regular and Hardcore Protesters
roam around the oil field and shout at the FrackMan any time they get near to him. Each
time the FrackMan is shouted at, he becomes more irritated (and less healthy) and
eventually he gives up and quits (loses a life). At this point, if the FrackMan has more
lives, the player can continue the game on the current level. If not, the game is over.
In addition to digging and walking around the oil field, the FrackMan may also shoot
their squirt gun (if it’s filled with water) by pressing the space bar. Squirts from the squirt
gun will irritate the Protesters and temporarily stun them. A Protester who is sufficiently
irritated (their hit points go to zero) will leave the oil field out of frustration.
Occasionally, Water Pools will randomly bubble up within the mineshafts, and the
FrackMan can move onto them to use this water to refill the squirt gun. But beware – a
Water Pool disappears quickly back into the mineshaft’s dirt, so the FrackMan needs to
siphon it off into the squirt gun quickly lest they disappear.
The FrackMan will also discover Gold Nuggets randomly distributed throughout the oil
field. If the FrackMan picks up one or more gold Nuggets, he can use them to bribe the
Protesters by dropping a Nugget in front of a Protester. If a Protester runs into such
dropped gold, it will pick it up and accept it as a bribe. A bribed Regular Protester leaves
6
the oil field immediately. However, a Hardcore Protester simply pockets the gold and
after briefly stopping to gloat over this new treasure, resumes the hunt to protest against
the FrackMan. If the player drops a Gold Nugget and it’s not picked up quickly, it will
sink into the muck of the oil field, never to be discovered again.
In addition to squirting and bribing the Protesters, the FrackMan can also cause Boulders
to drop on them. If the FrackMan digs all of the dirt supporting a Boulder from
underneath it, then after a short delay, the Boulder will fall until it crashes into the dirt or
another Boulder below. A Protester who is bonked by a Boulder as it falls becomes so
irritated that it will immediately leave the oil field. But beware, the FrackMan is also
susceptible to being bonked. If the FrackMan is bonked, he also becomes super irritated
(lose all of his hit points), and the level will end. If the player still has lives remaining,
the level is restarted.
Of course, the FrackMan is not alone – he has buddies that will help him from time to
time. Friends of the FrackMan will occasionally drop new Sonar Kits in the upper-left
corner of the oil field for the FrackMan to pick up. If the FrackMan has one or more
sonar charges in his inventory, the player can press the Z key to use a charge to map the
contents of the oil field within an extended radius around them, eliminating the need to
dig into the dirt to determine its contents. In addition to the Sonar Kits dropped by the
FrackMan’s friends, he also starts out with one sonar charge at the start of each level.
There are two types of protesters: Regular Protesters and Hardcore Protesters. As the
gameplay progresses on each level, the game will introduce various new Regular and
Hardcore Protesters in the upper-right corner of the screen, who will then seek out the
FrackMan to yell at him.
Regular Protesters are not too dedicated, and are more likely to leave the oil field if they
are either irritated by being squirted or bribed with gold. Regular Protesters wander
around essentially randomly through the tunnels of the oil field. However, if a Regular
Protester sees the FrackMan, he will run right up to him and begin shouting at him (to
irritate him). Each shout will cause the FrackMan’s health to go down, and when it
reaches zero, the FrackMan will give up (and lose a life). If a Regular Protester is hit by
a Boulder, it will instantly give up and head directly for the upper-right corner of the
screen, where it will disappear from the game forever (although a different, new
Protester will likely soon replace him as the game progresses).
In contrast to the Regular Protesters, Hardcore Protesters are much nastier. They behave
almost identically to Regular Protesters, with a few key differences. First, they’re much
more dedicated and so it takes more squirts from the FrackMan’s squirt gun to irritate
them sufficiently to make them leave the oil field. Second, Hardcore Protesters cannot
be bribed with Gold Nuggets to leave the oil field. Like Regular Protesters, they will
stop to pick up Nuggets dropped by the FrackMan if they run into them. However, after a
brief delay to examine the Nugget (giving the FrackMan valuable time to get away) they
will pocket the gold and continue to hunt through the oil field for the FrackMan. Third,
Hardcore Protesters are so dedicated that they have found a way to track the FrackMan’s
7
location from his cell phone, and if they are within a short range of the FrackMan (e.g.,
less than 16 horizontal/vertical moves away – this distance increases with each level of
the game), they will run toward him. Assuming they’re close enough, the HardCore
Protesters will home in on the FrackMan and move toward him even if they have no
direct line of sight to him (Hint: use a breadth-first search like we learned about in the
Queue lecture). Like Regular Protesters, HardCore Protesters are instantly superannoyed
if they’re bonked by a Boulder and will leave the oil field immediately. And
Hardcore Protesters will exhibit the same movement behavior as Regular Protesters if
they're not within cell-phone-detecting range of the FrackMan.
If the FrackMan’s health reaches zero because they are repeatedly shouted at and
irritated, he gives up and loses one “life.” If, after losing a life, the player has one or more
remaining lives left, a new oil field is generated for them from scratch (with the same
difficulty as the previous oil field they just tried to solve, but with a random, new layout),
the FrackMan is placed back on this level and they must again solve the entire level from
scratch. The FrackMan will restart the level with full health points, as well as their initial
water supply for 5 squirt gun squirts, and one sonar charge.
