## 0.2.0 - Hunt
 - Forage: a one-tap hunt yields a surprise catch - prey (feeds it), treasure
   (builds a hoard), or a rare egg (hatchery) - with a cooldown + catch banner.
 - Hoard rank/title and egg counts on the Stats screen.
 - Legacy: when your dragon dies, hatch an heir egg to continue with a care
   head-start (hoard persists across lineage).
 - Note: catch luck currently uses a safe internal source; true sub-GHz
   "airwave" sensing is a separate upcoming spike.

## 0.1.2
 - Heart meters: Hunger / Joy / Health now show as rows of 4 hearts.
 - Dedicated eating (open-mouth chomp) and sleeping (eyes-closed + Zzz) sprites,
   plus a poop glyph on screen and state motion/overlays.
 - Night is now 20:00-08:00 (was 22:00).

## 0.1.1
 - Fix: pet could die within hours / overnight. Two causes fixed:
   - Uncleaned poop no longer drains health directly (it is a sickness risk,
     via higher illness odds, not an instant bleed).
   - The pet now truly sleeps at night: hunger, happiness, health drain, and
     pooping all pause while it sleeps (22:00-08:00), so overnight is safe.
     Lights-out is a care bonus; lights-on a mild penalty, never lethal.

## 0.1 (Dragotchi, fork of MrModd's Matagotchi)
 - Dragon theme with care-driven branching evolution:
   Egg -> Hatchling -> Wyrmling -> Drake -> Adult (White / Grey / Black by care).
 - New needs & interactions: Hunger, Happiness, Health, Hygiene (poop),
   Sickness, Sleep (day/night), Discipline. Actions: Feed, Play, Clean,
   Medicine, Scold, Lights.
 - Care score drives which adult you get; impeccable care (>=90) makes an adult
   immortal (revocable). Neglect kills.
 - Deepened offline simulation (time passes while closed) + an attention alarm
   when you reopen to a pet in trouble, and live attention/discipline calls.
 - Custom main view with meters + animated dragon + action selector.
 - Hand-drawn 1-bit dragon sprites (white=well raised, black=neglected).
 - New save format (fresh start; not compatible with Matagotchi saves).
