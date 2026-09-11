# Dragotchi

A dragon-raising virtual pet for the [Flipper Zero](https://flipperzero.one/).
Raise an egg into a dragon — feed it, play with it, keep it clean and healthy,
discipline it, and let it sleep. **How well you care for it decides who it
becomes:** a radiant **white dragon** if you raise it well, a **grey** dragon
for middling care, or a **black dragon** if you neglect it. Care for an adult
impeccably and it can live *forever*.

## Status

Work in progress (v0.1). Built for Flipper Zero firmware **1.4.3** (API 87.1).

## Credits & license

Dragotchi is a fork of **[MrModd's Matagotchi](https://github.com/MrModd/Matagotchi)**
and is released, like the original, under the **GPLv3**. All of MrModd's
original copyright and the `LICENSE` file are retained. Thank you to MrModd for
the excellent foundation (two-thread engine, offline state fast-forward, and
save system) that Dragotchi builds on.

## Building

Requires [`ufbt`](https://github.com/flipperdevices/flipperzero-ufbt) pinned to
the release SDK matching your firmware:

```sh
pip install ufbt
ufbt update --channel release      # must resolve to API 87.1 for fw 1.4.3
ufbt                               # builds dist/dragotchi.fap
ufbt launch                        # build + install + run on a connected Flipper
```

The compiled app installs to `/ext/apps/Games/dragotchi.fap`.
