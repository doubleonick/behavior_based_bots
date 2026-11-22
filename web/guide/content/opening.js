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

  // The cover. "mix" leans on the stickers, with the adventure picture as a postcard
  // among them; "stickers" and "picture" are the two pure options, kept to choose from.
  // Try one with ?cover=picture (or stickers, or mix) at the end of the address.
  cover: {
    variant: 'mix',
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
    turn: 'Turn the page for the map'
  },

  // The map's places, in map coordinates (the map is 1040 wide, 840 high; the fold is
  // at 520). Creatures are placed relative to the place: dx, dy, size, turn.
  // [Places and most creatures are placeholders until the creatures are chosen.]
  map: {
    title: '[The Living World]',
    uncharted: 'Uncharted…',
    slide: 'Slide the map to see all of it.',
    places: [
      { id: 'jelly-shallows', name: 'The Jelly Shallows', x: 150, y: 220,
        creatures: [['jelly', -36, -40, 60], ['jelly', 24, -20, 42], ['jelly', 60, -60, 30]] },
      { id: 'worm-flats', name: 'The Worm Flats', x: 170, y: 640,
        creatures: [['flatworm', -26, -50, 44, -30], ['flatworm', 30, -44, 36, 20]] },
      { id: 'tide-pools', name: 'The Tide Pools', x: 395, y: 330,
        creatures: [['seahare', -26, -78, 44]] },
      { id: 'workshop', name: "Val's Workshop", x: 380, y: 745,
        creatures: [['val', 40, -70, 44]] },
      { id: 'falls', name: 'The Falls', x: 560, y: 640, creatures: [] },
      { id: 'lake', name: 'The Great Lake', x: 705, y: 590,
        creatures: [['frog', 40, -112, 36]] },
      { id: 'mossy-log', name: 'The Mossy Log', x: 660, y: 765,
        creatures: [['woodlouse', -40, -100, 34, 80], ['roundworm', 40, -20, 40]] },
      { id: 'bee-garden', name: 'The Bee Garden', x: 895, y: 765,
        creatures: [['bee', 10, -110, 34, -15]] },
      { id: 'moth-meadow', name: 'Moth Meadow', x: 940, y: 545,
        creatures: [['moth', -30, -100, 28, -20], ['moth', 24, -110, 24, 15], ['moth', 10, -60, 20, 30]] },
      { id: 'trail-wood', name: 'Trail Wood', x: 690, y: 365,
        creatures: [['ant', 50, -40, 22, 90], ['ant', 74, -40, 18, 90], ['ant', 94, -38, 16, 95]] },
      { id: 'high-peaks', name: 'The High Peaks', x: 830, y: 200, creatures: [] }
    ],
    // where each stop happens, and the route to it (drawn only once it is unlocked)
    stops: [
      { id: 'meet-val', n: 0, name: 'Meet Val', place: 'workshop' },
      { id: 'v1', n: 1, name: '[Vehicle 1]', place: 'jelly-shallows',
        route: [[372, 700], [330, 662], [286, 652], [240, 600], [200, 470], [170, 330], [160, 250]] }
    ],
    startHere: 'Start here',
    nextStop: 'Next: stop 1',
    comingSoon: '[coming soon]',
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
    valArt: 'val/v1-above-rear.png',
    valAlt: 'Val seen from above and just behind: a grey box with a face screen, a blue battery at the front and one wheel',
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
