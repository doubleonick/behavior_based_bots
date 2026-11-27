/* The guide's content for the opening: cover, inside cover, map, Meet Val and Val's
   button. Words, places and creatures only: no colours, fonts or layout (those are in
   style/notebook.css) and no drawings (those are files in art/).

   Anything in [square brackets] is a placeholder for the owner to write or decide.
   Art is named by path, relative to art/<style>/ unless it starts with "val/". */

window.GUIDE = {
  title: {
    kicker: '[A robot adventure]',
    name: ["Val's", 'Expedition'],
    tagline: '[Build a robot. Meet the creatures. Explore a living world.]',
    pill: 'build · meet · explore'
  },

  // The cover. "journal" (2026-11-27): a field journal, not a game box: a printed
  // label with a small ink-and-wash picture from the prologue, the Ci pressed into the
  // leather; bright colour is earned (stickers go on as the explorer travels).
  // The earlier options are kept: ?cover=mix (or stickers, or picture).
  cover: {
    variant: 'journal',
    label: { kicker: 'Field notebook', vignette: 'prologue/earth-after.svg',
             explorer: 'Explorer', begun: 'Begun' },
    // stuck round the title, in this order (the style decides where each one goes)
    stickers: ['ant', 'jelly', 'val', 'woodlouse', 'moth', 'bee', 'map-scrap', 'question'],
    picture: 'cover-picture.svg',
    colours: [
      { id: 'black', name: 'black' },
      { id: 'navy', name: 'navy blue' },
      { id: 'orange', name: 'burnt orange' },
      { id: 'maroon', name: 'maroon' },
      { id: 'green', name: 'forest green' },
      { id: 'purple', name: 'deep purple' }
    ],
    defaultColour: 'navy',
    chooseColour: 'Choose your notebook',
    open: 'Open the notebook'
  },

  inside: {
    promisesKicker: 'Your expedition',
    promisesTitle: 'On this journey you will…',
    promises: [
      { art: 'val', text: 'Build Val, a robot that grows: [eleven] robots from one.' },
      { art: 'creatures/jelly.svg', text: 'Meet creatures older than the dinosaurs, and some that live in your garden.' },
      { art: 'icons/bulb.svg', text: 'Find out how a brain can be made of a few wires.' },
      { art: 'icons/flag.svg', text: 'Take on a challenge at every stop.' },
      { art: 'icons/star.svg', text: 'Collect a sticker for every place you reach.' },
      { art: 'icons/pencil.svg', text: 'Write what you discover in this notebook.' }
    ],
    belongsTo: 'This notebook belongs to',
    namePlaceholder: 'your name',
    begun: 'Expedition begun',
    ifFound: 'If found, please return to',
    turn: 'Turn the page'
  },

  // The map's places, in map coordinates (the map is 1040 wide, 840 high; the fold is
  // at 520). Creatures are placed relative to the place: dx, dy, size, turn.
  // [Places and most creatures are placeholders until the creatures are chosen.]
  map: {
    title: '[The Living World]',
    uncharted: 'Uncharted…',
    slide: 'Slide the map to see all of it.',
    // The owner's own map drawing (2026-11-22), cleaned; places are in its pixels.
    // More places come when the owner redraws the map with them.
    drawing: { art: 'map-owner.jpg', w: 917, h: 632,
               alt: "A hand-drawn map: the sea on the left, a pale shore, a dark ridge, then grassland where a river runs from the mountains, through a lake and forest, down to the sea." },
    places: [
      { id: 'jelly-shallows', name: 'The Jelly Shallows', x: 95, y: 250,
        creatures: [['jelly', -30, -60, 40], ['jelly', 22, -48, 28]] },
      { id: 'workshop', name: "Val's Workshop", x: 372, y: 478,
        creatures: [['val', 0, -52, 40]] }
    ],
    // where each stop happens, and the route to it (drawn only once it is unlocked)
    stops: [
      { id: 'meet-val', n: 0, name: 'Meet Val', place: 'workshop' },
      { id: 'v1', n: 1, name: 'Vehicle 1', place: 'jelly-shallows',
        route: [[350, 440], [300, 432], [250, 390], [205, 340], [160, 300], [120, 280]] }
    ],
    startHere: 'Start here',
    nextStop: 'Next: stop 1',
    notYet: 'Not yet: the route will reach here.'
  },

  meetVal: {
    heading: 'Meet Val',
    sub: 'Stop 0: Val wakes up',
    story: '[Val has a face, but it can\'t feel anything or move yet, like the very first living things. The story for this stop goes here.]',
    tryIt: '[TRY IT] Switch Val on and listen: what does it say? Watch its face.',
    predict: 'What I think Val will say:',
    labels: {
      face: ['the face', "(it's the button too!)"],
      sockets: ["sockets for Val's senses", '(later)'],
      wheel: ['the wheel', '']
    },
    valArt: 'val/val-top.svg',
    valAlt: 'Val seen from above: a black frame with a blue battery, a face screen at the back, and wheels at the sides',
    next: "Next: learn Val's one button.",
    toButton: "Learn Val's button",
    grownUp: '[For the grown-up: before stop 0 the firmware goes on Val once, from a computer, and the battery needs charging. The Handbook has the steps.]'
  },

  button: {
    heading: "Val's one button",
    lede: "Val's face is also its only button. You answer Val's questions by pressing it in three ways.",
    presses: [
      { kind: 'short', name: 'Short press', how: 'a quick tap', means: 'change the answer' },
      { kind: 'long', name: 'Long press', how: 'hold, count "one", let go', means: 'keep the answer' },
      { kind: 'hold', name: 'Hold 2 seconds', how: 'while Val is driving or resting', means: 'start setup again' }
    ],
    tryHeading: 'Try it on Val',
    tryIt: '[TRY IT: what to try with the button at stop 0, for the owner to write.]',
    later: 'At every stop, the setup cards show each picture Val will show you, and which press answers it.',
    toMap: 'Back to the map'
  },

  nav: { back: 'back', turn: 'turn the page', page: 'page' }
};
