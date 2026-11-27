/* The guide's content for the prologue: words only. From the proposed third draft
   (web/mockups/guide/prologue_draft3.html, 2026-11-26), with the owner's own drawings.
   The owner is writing the final text; anything in [square brackets] is a placeholder. */

window.GUIDE = window.GUIDE || {};
window.GUIDE.prologue = {
  spreads: [
    {
      left: [
        { big: 'Billions of years ago, Earth was a planet of dull grey rock.' },
        { p: 'Then, CRASH! Another young world slammed right into it.' },
        { art: 'prologue/collision.svg', alt: 'Young grey Earth struck at its scar by another young world, with a burst of light' }
      ],
      right: [
        { p: "The crash boiled Earth's oceans away. Deep inside, Earth grew hotter and began to churn." },
        { p: 'Pieces of the broken world flew out into space, and slowly gathered into something new: **the Moon**.' },
        { art: 'prologue/earth-after.svg', alt: 'Earth after the crash, grey with molten orange craters and a scarred starburst, and the new Moon' }
      ]
    },
    {
      left: [
        { p: 'Slowly, the new Earth cooled. Rain fell for a very, very long time, and new oceans filled.' },
        { p: 'In that warm water, some __molecules__ could do something amazing: make copies of themselves. First **RNA**, then **DNA**. They are the chemical codes of life.' },
        { art: 'prologue/rna-dna.svg', alt: 'A single strand labelled RNA, and a twisted double strand labelled DNA' }
      ],
      right: [
        { p: 'The copies were never quite perfect. Most tiny mistakes, called __mutations__, changed nothing. A few made things worse. And a few made things better.' },
        { p: 'Somewhere in those oceans, a thin wall wrapped around some of these copying molecules and kept the rest of the ocean out. That wall turned them into the first true living things: __cells__.' },
        { cell: 'prologue/cell.svg', alt: 'The first cell: a pinched round wall with strands of RNA and DNA inside',
          say: "Hi! I'm __alive__! I can keep myself going… but I can't go anywhere I choose. Not yet." }
      ]
    },
    {
      left: [
        { p: 'It took life a very long time to learn to feel the world around it, to move through it, and to choose where to go.' },
        { p: 'How did it happen? To find out, we need a very special explorer.' },
        { art: 'val/val-top.svg', alt: 'Val Brooks seen from above: a robot with a battery, a face screen, and wheels', small: true },
        { p: 'This is **Val Brooks**, named after two scientists who built machines to understand living things.' }
      ],
      right: [
        { p: 'Val is a robot. Val is **not** alive. And, like that first cell, Val starts with almost nothing: no senses, no way to choose where to go.' },
        { p: 'But Val can be built, one step at a time, the way life built itself.' },
        { p: 'Living things that can act on the world, and not just float along in it, are called __agents__. An agent can:' },
        { checks: ['navigate', 'communicate', 'manipulate'] },
        { big: "Can something that isn't alive become an agent?" },
        { p: "Let's find out, and maybe learn something about **you**, too." }
      ]
    }
  ],
  turn: 'turn the page',
  toMap: 'Turn the page for the map'
};
